


#include"Population.h"
//#include<iostream>
//#include<string>
//#include <sstream>		// std::stringstream
#include"Global_variables.h"
#include "Small_functions.h"
#include "Find_thresholds_for_a_matrix.h"
#include "Open_regions.h"
#include "Close_regions.h"




// Platform-specific commands
#ifdef _WIN32
//	#include"direntvc.h"
	#include"population_sacha\Log_steps.h"	
//	#include"population_sacha\Statistics_placenta.h"
#elif __linux__ || defined __APPLE__ || __APPLE__
//	#include"dirent.h"
	#include"population_sacha/Log_steps.h"
//	#include"population_sacha/Statistics_placenta.h"
#endif




using namespace std; 




// Section 4.2.2 Thresholding and the initial segmentation

// Zhenzhen is using both a red matrix and a blue matrix to perform the initial segmentation
// The scheme described in the thesis is a bit simplified.
// But do we actually need two matrices ?

void initial_segmentation(Mat2RGBUI8& subImg, 
						  Mat2UI8& three_regions_segmentation_matrix 
						  //const string resultDir, 
						  //const string img_name_withoutExtension
						  )

{
	// Local variables
	// int bins=30;
	// int minDistance=3;
	float blue_matrix_threshold;
	float red_matrix_threshold;
	float tmpFloat;

	Mat2F64 blue;
	Mat2F64 red;

	//Mat2RGBUI8 three_regions_segmentation_matrix_colored;

	Mat2UI8 Border;
	Mat2UI8 RBCs_matrix;
	Mat2UI8 IVS_matrix;
	//Mat2UI8 villous_stroma_matrix;

	Mat2F64 h_red;
	Mat2F64 h_blue;
	Mat2F64 maxLocal;
	Mat2F64 blue_histogram;
	Mat2F64 red_histogram;

	stringstream str_stream;

	VecF64 thresholds_location_blue;
	VecF64 thresholds_location_red;

	// Variables initialization
	int size_i=subImg.sizeI();
	int size_j=subImg.sizeJ();
	blue = Mat2F64 (subImg.sizeI(),subImg.sizeJ());
	Border = Mat2UI8 (subImg.sizeI(),subImg.sizeJ());
	RBCs_matrix = Mat2UI8 (subImg.sizeI(),subImg.sizeJ());
	h_red = Mat2F64(histogram_bins_number,2);
	h_blue = Mat2F64(histogram_bins_number,2);	
	red_histogram = Mat2F64 (2, histogram_bins_number+1);
	blue_histogram = Mat2F64 (2, histogram_bins_number+1);

	//three_regions_segmentation_matrix_colored = Mat2RGBUI8 (subImg.sizeI(),subImg.sizeJ());
	// three_regions_segmentation_matrix = Mat2UI8 (subImg.sizeI(),subImg.sizeJ());
	// villous_stroma_matrix = Mat2UI8 (subImg.sizeI(),subImg.sizeJ());
	IVS_matrix = Mat2UI8 (subImg.sizeI(),subImg.sizeJ());
	maxLocal = Mat2F64(3,2);
	red = Mat2F64 (subImg.sizeI(),subImg.sizeJ());
	

	log_steps("Performing initial segmentation");

	// Calculating the blue and the red matrices:	 R=r^2-b^2-g^2;		B=b^2-r^2-g^2
	for(int i=0;i<subImg.sizeI();i++)
	{
		for(int j=0;j<subImg.sizeJ();j++)
		{
			red(i,j) = subImg(i,j).r()*subImg(i,j).r()-subImg(i,j).b()*subImg(i,j).b()-subImg(i,j).g()*subImg(i,j).g();
			red(i,j) = (red(i,j)/255/255 + 2)/3;		// Normalizing
			blue(i,j) = subImg(i,j).b()*subImg(i,j).b()-subImg(i,j).r()*subImg(i,j).r()-subImg(i,j).g()*subImg(i,j).g();
			blue(i,j) = (blue(i,j)/255/255 + 2)/3;		// Normalizing
		}
	}

	
	// Looking for the local maxima of the histograms
	log_steps("Calculating segmentation thresholds of the 'red' and 'blue' matrices");
	thresholds_location_red = find_thresholds_for_a_matrix (red, red_histogram);		// Gives an array of two numbers for the two thresholds
	thresholds_location_blue = find_thresholds_for_a_matrix (blue, blue_histogram);
	log_steps();		// End: "Calculating segmentation thresholds of the 'red' and 'blue' matrices"
	
	
	// Thresholding the 'red' and the 'blue' matrices
	str_stream << "Thresholding the 'red' matrix with a threshold set at " << thresholds_location_red(1) <<
		" and the 'blue' matrix with a threshold at " << thresholds_location_blue(0);
	log_steps(str_stream.str());
	//log_steps();		// str_stream.str());
	str_stream.clear();

	RBCs_matrix.fill(int_black_pixel_color);
	IVS_matrix.fill(int_black_pixel_color);
	for(int i=0;i<subImg.sizeI();i++)
	{
		for(int j=0;j<subImg.sizeJ();j++)
		{
			if(red(i,j)>thresholds_location_red(1))			// The most red pixels will be considered as RBCs
			{
				RBCs_matrix(i,j)=int_white_pixel_color;
			}
			if(blue(i,j)<thresholds_location_blue(0))
			{
				IVS_matrix(i,j)=int_white_pixel_color;		// The least blue pixels will be considered as IVS
			}
		}
	}

	log_steps();	// End: Thresholding
	

	// Smoothing the boundaries of the obtained IVS and RBC regions, so that the villous regions do not get patchy
	log_steps("Smoothing out RBC regions");
	open_regions(RBCs_matrix, RBC_smoothing_radius_pixels);
	log_steps();		// "Smoothing out RBC regions"


	log_steps("Smoothing out IVS regions");
	open_regions(IVS_matrix, IVS_smoothing_radius_pixels);
	log_steps();		// "Smoothing out RBC regions"


	/*
	// The villous stroma is defined as the pixels, which are not IVS and not RBCs
	// Calculating the villous stroma matrix
	Mat2UI8 villous_stroma_matrix(size_i, size_j);
	for(int i=0;i<subImg.sizeI();i++)
	{
		for(int j=0;j<subImg.sizeJ();j++)
		{
			if(IVS_matrix(i,j)>0 || RBCs_matrix(i,j)>0)	
			{
				villous_stroma_matrix(i,j)=0;
			}
			else
			{
				villous_stroma_matrix(i,j)=1;
			}
		}
	}


	// Smoothing out villi contours.
	// Smoothed out parts will be later replaced by IVS
	log_steps("Smoothing out villous regions");
	close_regions(villous_stroma_matrix, Villi_smoothing_radius_pixels);
	log_steps();		// "Smoothing out RBC regions"
	*/

	
	// The initial segmentation is finished
	// Combining the obtained components in the same picture
	// All pixels that are not one of these components are set to the third one
	// IVS --- white; RBCs --- gray; villous stroma --- black
	// At the same time generating the missing component matrix
	//Mat2UI8 three_regions_segmentation_matrix (size_i, size_j); //.fill(int_black_pixel_color);
	Mat2UI8 villous_stroma_matrix (size_i, size_j);
	for(int i=0;i<subImg.sizeI();i++)
	{
		for(int j=0;j<subImg.sizeJ();j++)
		{
			if(RBCs_matrix(i,j)>0)	
			{
				three_regions_segmentation_matrix(i,j)=int_fetal_RBCs_pixel_color;
				villous_stroma_matrix(i,j)=0;
			}
			else if(IVS_matrix(i,j)>0)
			{
				three_regions_segmentation_matrix(i,j)=int_white_pixel_color;
				villous_stroma_matrix(i,j)=0;
			}
			else
			{
				three_regions_segmentation_matrix(i,j)=int_black_pixel_color;
				villous_stroma_matrix(i,j)=1;
			}
		}
	}
	



	// colorize_image(three_regions_segmentation_matrix_colored, three_regions_segmentation_matrix);		// Colorizing the matrix containing the three object types


	log_steps ("Saving thresholding results");
	save_histogram (red_histogram, resultDir, str_img_name_withoutExtension + string_output_picutre_number()+"_red_histogram.dat");
	save_histogram (blue_histogram, resultDir, str_img_name_withoutExtension + string_output_picutre_number()+"_blue_histogram.dat");
	saveFileBinary(RBCs_matrix, resultDir, str_img_name_withoutExtension + string_output_picutre_number()+"_RBCs.png");
	saveFileBinary(IVS_matrix, resultDir, str_img_name_withoutExtension + string_output_picutre_number()+"_IVS.png");
	saveFileBinary(villous_stroma_matrix, resultDir, str_img_name_withoutExtension + string_output_picutre_number()+"_Villous_stroma.png");
	saveFileColor(three_regions_segmentation_matrix, resultDir, str_img_name_withoutExtension + string_output_picutre_number()+"_Initial_segmentation.png");
	log_steps ();		// "Saving thresholding results";

	
	


	// Cleaning up
	blue.clear();
	Border.clear();
	RBCs_matrix.clear();
	h_blue.clear();	
	h_red.clear();
	IVS_matrix.clear();
	red.clear();
	villous_stroma_matrix.clear();

	
	log_steps();		// "Performing initial segmentation";

}







