// Code developed by Zhenzhen YOU and Alexander SEROV 
// at PMC Laboratory of Ecole Polytechnique, France
//
// Contact e-mail: alexander.serov@polytechnique.edu



#include"Population.h"		// Include before other files or get some missing ";" error
#include<fstream>
#include<iostream>
#include<sys/types.h>
#include<errno.h>
#include<string>			// std:: string
#include<algorithm>
#include<utility>  
#include<ctime>
#include"CImg.h"
#include"population_sacha/Choose_input_folder.h"
#include"population_sacha/Copy_original_image.h"
#include"population_sacha/Create_dot_finished_file.h"	
#include"population_sacha/Create_results_folder.h"
#include"population_sacha/Discriminate_capillaries_from_IVS.h"
#include"population_sacha/Estimate_villi_density.h"
#include"population_sacha/Initial_segmentation.h"
#include"population_sacha/Load_placenta_picture.h"
#include"population_sacha/Log_steps.h"
#include"population_sacha/Read_next_image.h"		
#include"population_sacha/Segmentation_parameters.h"
#include"population_sacha/Small_functions.h"
#include "population_sacha/Smooth_out_the_contours.h"
#include"population_sacha/Statistics_placenta.h"
#include "population_sacha/Remove_IVS_inside_villi.h"
#include "population_sacha/Remove_IVS_and_villous_regions_by_area.h"

// Platform-specific commands
#ifdef _WIN32
	#include"direntvc.h"
	#include <direct.h>
    #define getcwd _getcwd
	#define DIRECTORY_SEPARATOR "\\"
	#define DIRECTORY_SEPARATOR_SEARCH "/\\"
	#define SHELL_COPY_COMMAND "copy"
//	#define DEFAULT_RESULTS_DIRECTORY ".//results//"
#elif __linux__
	#include"dirent.h"
	#include"unistd.h"
	#define DIRECTORY_SEPARATOR "/"
	#define DIRECTORY_SEPARATOR_SEARCH "/"
	#define SHELL_COPY_COMMAND "cp"
//	#define DEFAULT_RESULTS_DIRECTORY ".\results\"
#endif

//using namespace cimg_library;
//using namespace std; //std namespace // cin/cout // string
using namespace pop; //Population namespace


// Global constants
#define restart_tries 2 // Number of times to restart cycle to check for new pictures
#define nbResults 9 //number of results of every image

// Global variables - Defined in this file
string extension;
string str_img_name_withoutExtension;
char *mainDir;
char *resultDir;

// Global variables - Externally defined (initialized here)
//extern const bool bl_delete_small_villi=true; 
extern const bool bl_consider_RBCs_as_villi=false; // 
extern const int FILENAME_LENGTH=512; // FILENAME_LENGTH
extern const int int_black_pixel_color=0; //Villi
extern const int int_fetal_RBCs_pixel_color=50; //GR inside Villi
extern const int int_maternal_RBCs_pixel_color=150; //GR outside Villi
extern const int int_white_pixel_color=255; //IVS






namespace {
	
	// ===== Local primary constants =====
	const int radiusCells=4;
	const int int_noise_erosion_radius_pixels=5;  // Performing erosion-dilation to remove smaller than RBCs (6 px) noise
	//const double Pi_number=3.14159256;
	const int remove_areas_smaller_than_pixels=100; // Removing noise by region area

	// ===== Local derived constants =====
	const double area_GR=Pi_number*radiusCells*radiusCells;
//	const double Minimal_villi_area_pixels=Pi_number*Minimal_villi_radius_pixels*Minimal_villi_radius_pixels;

	// ===== Local variables =====
	bool analyze=true;
	bool bl_image_is_complicated;
	bool processed_a_new_picture;
	bool stopProgram=false;
	
	char *fileInDir;
	char *fileProcessedDir;
	char *tmp_number_string;
	char *tmpStr;

	DIR *dir;

	double texec=0.;

	float percentage_threshold_of_IVS_on_the_internal_IVS_boundary_for_a_capillary;
	float IVS_percentage_in_the_image; 
	float tmpFloat;
	float thresholdArea;

	int loop=0;
	int nbAllFilesIgnore=0;
	int nbSingleFileIgnore=0;
	int nbAllFiles=0;
	int output_picture_number=1;  // Not used any more?
	int restart=restart_tries;

	//Mat2RGBUI8 img_3objets;
		
	//Mat2UI8 working_image;
	//Mat2UI8 img_initial_3_objects;
	//Mat2UI8 img_final;
	//Mat2UI8 img_segmentation;

	ofstream file_txt;
	
	string fileCuttedImageInDir;
	string str_img_In;
	string lock;
	string resultFileDir;
	string s_tmp;	

	stringstream ss_tmp;
	
	time_t tbegin;
	time_t tend;
}


		
		
		


int main(int argc, char *argv[])
{
	
	
	pop::CollectorExecutionInformationSingleton::getInstance()->setActivate(true);
    try
	{
		// Variables initialization
		fileInDir  = new char[FILENAME_LENGTH];
		fileProcessedDir = new char[FILENAME_LENGTH];
		mainDir= new char[FILENAME_LENGTH];
		resultDir= new char[FILENAME_LENGTH];
		tmp_number_string=new char[FILENAME_LENGTH];
		tmpStr = new char[FILENAME_LENGTH];
		
		
		// Starting the timer (is it still used?)
		tbegin=time(NULL);

		log_steps("======================= PROGRAM STARTED =======================", "general");

		// Choosing the input folder
		choose_input_folder(fileInDir, mainDir, argc, argv);
		
		// Creating the results folder
		create_results_folder(fileInDir, resultDir);


		// This cycle is necessary to restart the code and check if new pictures were added during the execution
		while(restart>0 && !stopProgram)
		{
			// Initializing variables
			processed_a_new_picture=false;
			nbAllFilesIgnore=0;
			nbAllFiles=0;
			loop++;
			
			// Processing new images while there are some
			pop::Mat2RGBUI8 img_input_image_color;
			while (read_next_image(fileInDir, nbAllFiles, str_img_name_withoutExtension, str_img_In,
				lock, extension, mainDir, resultDir, img_input_image_color, processed_a_new_picture)==true)
			{

								
				// *** Image Processing *** //
				log_steps("Processing image: " + str_img_name_withoutExtension + extension);
				
				copy_original_image(str_img_In);		// Create a copy in the results folder

				// Performing the initial segmentation
				// Input: img_input_image_color
				// Output: img_initial_3_objects
				pop::Mat2UI8 img_initial_3_objects (img_input_image_color.sizeI(), img_input_image_color.sizeJ());
				initial_segmentation(img_input_image_color, img_initial_3_objects);
				img_input_image_color.clear();		// Don't need the initial RGB image any more
				pop::Mat2UI8 img_3_objects (img_initial_3_objects);
				
				
				// Making calculations of the villi density and selecting one of the two sets of parameters for what follows
				// Input: img_3_objects
				IVS_percentage_in_the_image = estimate_villi_density(img_3_objects, Minimal_villi_area_pixels, 
					percentage_threshold_of_IVS_on_the_internal_IVS_boundary_for_a_capillary, bl_image_is_complicated);


				// Filtering out noise		
				// Both modify: img_3_objects
				// The second function actually returns a 2-colors image consisting of (IVS+RBCs) and villi
				//close_regions();
				remove_IVS_inside_villi(img_3_objects);
				remove_IVS_and_villous_regions_by_area (img_3_objects);
									
					
				// Discriminating fetal capillaries from IVS
				// Input: img_3_objects (in 2 colors, only (IVS+RBCs) and villi)
				// Output: img_3_objects (3 colors)
				discriminate_capillaires_from_IVS(img_3_objects, 
					percentage_threshold_of_IVS_on_the_internal_IVS_boundary_for_a_capillary, img_initial_3_objects);
				img_initial_3_objects.clear();		// Don't need the initial segmentation any more


				// Smoothing out the contours
				// Modifies: img_3_objects
				smooth_out_the_contours(img_3_objects);
				
				
				// Performing statistical measurements
				// Check the images that are send to this function
				statistics_placenta(img_3_objects, IVS_percentage_in_the_image, lock, tmpStr);


				// Creating a .finished file
				create_dot_finished_file(resultDir, str_img_name_withoutExtension);


				// Deleting the .lock file
				copy(lock.begin(), lock.end(), tmpStr);
				tmpStr[lock.size()] = '\0';
				remove(tmpStr);

				log_steps();		// End: "Processing image: " + str_img_name_withoutExtension + extension);
				//log_steps();		
							
			}
			if (!processed_a_new_picture || stopProgram) restart--;
			
		}//while(restart)


		//end timer
		tend = time(NULL);
		//Compute exectution time
		texec = difftime(tend, tbegin);
		cout<<"It takes "<<texec<<"s to execute this program!"<<endl<<endl;
		log_steps("======================= PROGRAM SUCCESSFULLY TERMINATED =======================", "general");


	}	// try
	catch (const std::exception& e) 
	{
		if (str_img_name_withoutExtension=="")
		{
			str_img_name_withoutExtension="Error_log";
		}
		else
		{
			str_img_name_withoutExtension=str_img_name_withoutExtension+"_Error_log_";
		}
		
		log_steps(e.what());
		log_steps();
	}
	catch (...) 
	{
		if (str_img_name_withoutExtension=="")
		{
			str_img_name_withoutExtension="Error_log";
		}
		else
		{
			str_img_name_withoutExtension=str_img_name_withoutExtension+"_Error_log_";
		}
		log_steps("Caught an unknown exception");
		log_steps();
		log_steps("======================= PROGRAM TERMINATED =======================", "general");
	}

	#ifdef _WIN32
		system("pause");
	#endif
	return 0;
}


