
#include "Global_variables.h"
#include "Population.h"
#include "core/algorithm/GeometricalTransformation.h"
#include <string>
#include "Small_functions.h"


// Platform-specific commands
#ifdef _WIN32
	#include"population_sacha\Log_steps.h"	
	/*
	#include"direntvc.h"
	#include <direct.h>
    #define getcwd _getcwd
	#include"population_sacha\Create_dot_finished_file.h"
	#include"population_sacha\Discriminate_capillaries_from_IVS.h"
	#include"population_sacha\Fill_holes_in_the_villi.h"
	#include"population_sacha\Initial_segmentation.h"	
	#include"population_sacha\Load_placenta_picture.h"
	#include"population_sacha\Remove_noise.h"	
	#include"population_sacha\Statistics_placenta.h"
	#define DIRECTORY_SEPARATOR "\\"
	#define DIRECTORY_SEPARATOR_SEARCH "/\\"
	#define SHELL_COPY_COMMAND "copy"
//	#define DEFAULT_RESULTS_DIRECTORY ".//results//"
	*/
#elif __linux__
	#include"population_sacha/Log_steps.h"
	/*
	#include"dirent.h"
	#include"unistd.h"
	#include"population_sacha/Create_dot_finished_file.h"	
	#include"population_sacha/Discriminate_capillaries_from_IVS.h"
	#include"population_sacha/Fill_holes_in_the_villi.h"
	#include"population_sacha/Initial_segmentation.h"
	#include"population_sacha/Load_placenta_picture.h"
	#include"population_sacha/Remove_noise.h"	
	#include"population_sacha/Statistics_placenta.h"	
	#define DIRECTORY_SEPARATOR "/"
	#define DIRECTORY_SEPARATOR_SEARCH "/"
	#define SHELL_COPY_COMMAND "cp"
//	#define DEFAULT_RESULTS_DIRECTORY ".\results\"
	*/
#endif



using namespace pop;
using namespace std;










//perimeter according to label
void perimeter(VecF64 &perimeter, Mat2UI32 &label)
{
	log_steps("Getting the perimeter");
	//cout<<endl<<"start: Perimeter"<<endl;
	time_t tbegin, tend;
	double texec=0.;	
	//start timer
	tbegin=time(NULL);

	int lengthLabel=pop::Analysis::maxValue(label);
	int m;//tmp_label;
	for(int i=0;i<lengthLabel;i++)
	{
		perimeter(i)=0;
	}
	for(int lin=0;lin<label.sizeI();lin++)
	{
		for(int col=0;col<label.sizeJ();col++)
		{
			m = label(lin,col);
			if(m!=0)
			{
				if(lin==0||lin==label.sizeI()-1||col==0||col==label.sizeJ()-1)
				{
					perimeter(m-1)=perimeter(m-1)+1;
				}
				else if((label(lin, col-1)!=m)||(label(lin-1, col)!=m)||(label(lin+1, col))!=m||(label(lin, col+1))!=m)
				{
					perimeter(m-1)=perimeter(m-1)+1;
				}
			}
		}
	}
	//end timer
	tend = time(NULL);
	//Compute exectution time
	texec = difftime(tend, tbegin);
	//cout<<"It takes "<<texec<<"s to get the perimeter!"<<endl;
	//cout<<"end: Perimeter"<<endl<<endl;
	log_steps();
}




void decideContourisObject(Mat2UI8 &contour, Mat2UI32 label_contour, Mat2UI8 &img_original, unsigned char colorObjet, 
						   float perEdge, float thresholdArea, bool considerInOutsideContour, string resultDir)
{
	log_steps("Decision making (?)");
	//cout<<endl<<"start: Decision"<<endl;
	time_t tbegin, tend;
	double texec=0.;	
	//start timer
	tbegin=time(NULL);

	for(int i=0;i<img_original.sizeI();i++)
	{
		for(int j=0;j<img_original.sizeJ();j++)
		{
			if(contour(i,j)==int_black_pixel_color)
			{
				label_contour(i,j)=0;
			}
		}
	}
	
	Mat2RGBUI8 rgb = pop::Visualization::labelToRandomRGB(label_contour);
	//saveFileColor(rgb, resultDir, string_output_picutre_number()+"201.png");

	VecI32 areaContour=pop::Analysis::areaByLabel(label_contour);
	int length_label_contour = pop::Analysis::maxValue(label_contour);
	VecI32 nbVilli(length_label_contour), nbIVS(length_label_contour), nbGRinside(length_label_contour), nbGRoutside(length_label_contour);
	for(int i=0;i<img_original.sizeI();i++)
	{
		for(int j=0;j<img_original.sizeJ();j++)
		{
			if(label_contour(i,j)!=0)
			{
				if(img_original(i,j)==int_white_pixel_color)
				{
					nbIVS(label_contour(i,j)-1)=nbIVS(label_contour(i,j)-1)+1;//the percentage of frequency of color IVS to a fixed label
				}
				else if(img_original(i,j)==int_black_pixel_color)
				{
					nbVilli(label_contour(i,j)-1)=nbVilli(label_contour(i,j)-1)+1;
				}
				else if(img_original(i,j)==int_maternal_RBCs_pixel_color)
				{
					nbGRoutside(label_contour(i,j)-1)=nbGRoutside(label_contour(i,j)-1)+1;
				}
				else if(img_original(i,j)==int_fetal_RBCs_pixel_color)
				{
					nbGRinside(label_contour(i,j)-1)=nbGRinside(label_contour(i,j)-1)+1;
				}
			}
		}
	}
	
	float perIVS, perVilli, perGRinside, perGRoutside, perObjet;
	VecI32 label_contour_is_Objet(length_label_contour);
	for(int i=0;i<length_label_contour;i++)
	{
		perIVS = (float) nbIVS(i)/areaContour(i);
		perVilli = (float) nbVilli(i)/areaContour(i);
		perGRinside = (float) nbGRinside(i)/areaContour(i);
		perGRoutside = (float) nbGRoutside(i)/areaContour(i);

		if (colorObjet==int_white_pixel_color)	perObjet=perIVS;
		else if (colorObjet==int_black_pixel_color)	perObjet=perVilli; 
		else if (colorObjet==int_maternal_RBCs_pixel_color) perObjet=perGRoutside;// GR outside
		else if (colorObjet==int_fetal_RBCs_pixel_color) perObjet=perGRinside; // GR inside
		else cout<<"Error: this object doesn't exist!"<<endl;

		if(perEdge!=0)
		{
			if(perObjet>=perEdge)
			{
				label_contour_is_Objet(i)=1;//this zone is Object
			}
			else
			{
				label_contour_is_Objet(i)=0;//not
			}
		}
		else
		{
			if(perObjet==0)
			{
				label_contour_is_Objet(i)=1;//this zone is inside Object
			}
			else
			{
				label_contour_is_Objet(i)=0;//not
			}
		}
	}

	for(int i=0;i<img_original.sizeI();i++)
	{
		for(int j=0;j<img_original.sizeJ();j++)
		{
			if(label_contour(i,j)!=0)
			{
				if(label_contour_is_Objet(label_contour(i,j)-1)==0)
				{
					contour(i,j)=int_black_pixel_color;
				}
			}
		}
	}
	
	label_contour=Processing::clusterToLabel(contour,0);
	rgb = pop::Visualization::labelToRandomRGB(label_contour);
	//saveFileColor(rgb, resultDir, string_output_picutre_number()+"202.png");

	//end timer
	tend = time(NULL);
	//Compute exectution time
	texec = difftime(tend, tbegin);
	//cout<<"It takes "<<texec<<"s to get the decision!"<<endl;
	//cout<<"end: Decision"<<endl<<endl;
	log_steps();
}

// Conditionally replacing one color by another. Condition: surface area
void conditionally_replace_color_1_by_color_2(Mat2UI8 &img_original, unsigned char old_color, unsigned char new_color, 
						   float area_threshold)
{
	// Declaration
	int i;
	int j;
	int label;
	int number_of_labels;

	Mat2UI8 bl_img_old_color_only;
	Mat2UI32 region_labels;
	VecF64 region_areas;
	VecI32 bl_labels_to_replace;


	log_steps("Conditionally replacing one color by another. Condition: surface area");

	// Initialization
	bl_img_old_color_only = Mat2UI8 (img_original.sizeI(),img_original.sizeJ());
	bl_img_old_color_only.fill (int_black_pixel_color);

	// Copying only the required color in a matrix to be able to analyze the surfaces
	for(int i=0;i<img_original.sizeI();i++)
	{
		for(int j=0;j<img_original.sizeJ();j++)
		{
			if(img_original(i,j)==old_color)
			{
				bl_img_old_color_only(i,j) = true;
			}
		}
	}


	// Calculating the areas of the regions to be able to apply the area threshold
	region_labels = pop::Processing::clusterToLabel (bl_img_old_color_only, 1);	// Numerates from left to right, from top to bottom. Labels are counted from 0
	number_of_labels = pop::Analysis::maxValue (region_labels)+1;	// Including the label 0
	region_areas = pop::Analysis::areaByLabel (region_labels);	// Calculate the area according to region_labels. Areas are counted from 1. Very confusing

	
	// Marking the labels that should be replaced by analyzing regions areas
	bl_labels_to_replace = VecI32 (number_of_labels, false);
	for (label=1; label<number_of_labels; label++)
	{
		if (region_areas(label-1) < area_threshold)		// -1 because label 1 is the 0-th element
			bl_labels_to_replace(label)=true;
	}
	

	// Replacing the color in the original image
	for(int i=0;i<img_original.sizeI();i++)
	{
		for(int j=0;j<img_original.sizeJ();j++)
		{
			label=region_labels(i,j);
			if(bl_labels_to_replace(label))
				img_original(i,j)=new_color;
		}
	}

	log_steps();
}





void labelObjet(Mat2RGBUI8& img_colored_regions, const Mat2UI32& img_labeled_regions, int textSize, int position_start)
{
	log_steps("Labeling objects");
	
	int nb, X_c, Y_c;
	float Somme_X,Somme_Y;
	char* chr_pnt_label = new char[FILENAME_LENGTH];

	int number_of_regions (pop::Analysis::maxValue(img_labeled_regions) + 1);

	// First making calculations, then will be putting numbers in the image

	// Initializing the arrays of region centers and areas
	long *regions_centers_i = new long [number_of_regions];
	long *regions_centers_j = new long [number_of_regions];
	long *regions_areas = new long [number_of_regions];
	// Filling them with zeros
	for (int k=0; k<number_of_regions; k++)
	{
		regions_centers_i[k]=0;
		regions_centers_j[k]=0;
		regions_areas[k]=0;
	}


	// Parsing the image and noting the necessary values
	int size_i=img_labeled_regions.sizeI();
	int size_j=img_labeled_regions.sizeJ();
	for (int i=0; i < size_i; i++)
	{
		for (int j=0; j < size_j; j++)
		{
			int current_label=img_labeled_regions(i,j);
			regions_centers_i[current_label]+=i;
			regions_centers_j[current_label]+=j;
			regions_areas[current_label]+=1;
		}
	}


	// Drawing the labels. The 0 label is skipped since it is the background
	RGBUI8 label_color (255, 255, 255);
	for (int current_label=1; current_label<number_of_regions; current_label++)
	{

		sprintf(chr_pnt_label,"%i",current_label+position_start);
		Draw::text(img_colored_regions, chr_pnt_label, 
			Vec2I32(regions_centers_i[current_label]/regions_areas[current_label],
				regions_centers_j[current_label]/regions_areas[current_label]), 
			label_color, textSize);

	}

	// Cleaning up
	delete [] regions_centers_i;
	delete [] regions_centers_j;
	delete [] regions_areas;
	delete [] chr_pnt_label;

	log_steps();
}

void restoreRotation(pop::Mat2UI8 &img)
{
// Restore image rotation before saving to make it correspond to the input
	img = pop::GeometricalTransformation::mirror(img, 0);
	img = pop::GeometricalTransformation::rotateMultPi_2(img, -1);
}

void restoreRotation(pop::Mat2RGBUI8 &img)
{
// Restore image rotation before saving to make it correspond to the input
	img = pop::GeometricalTransformation::mirror(img, 0);
	img = pop::GeometricalTransformation::rotateMultPi_2(img, -1);
}

 void saveFileBinary(pop::Mat2UI8 img, std::string fileDirectory, std::string fileName)
 {
	 // Rescaling the binary picture to 255 at the maximum and zero at the minimum
	 int min_value = pop::Analysis::minValue(img);
	 int max_value = pop::Analysis::maxValue(img);
	 
	 if (min_value > 0 && min_value != max_value)
		 img -= min_value;

	 if (max_value != min_value && max_value-min_value < 255)
		 img *= 255/(max_value-min_value);
	 
	 // Preparing the save path
	 stringstream ss_tmp;
	 string s_tmp;
	 char *tmpStr = new char[FILENAME_LENGTH];
	 ss_tmp << fileDirectory << fileName;
	 s_tmp=ss_tmp.str();
	 copy(s_tmp.begin(), s_tmp.end(), tmpStr);
	 tmpStr[s_tmp.size()] = '\0';

	 // Saving
	 restoreRotation(img);
	 img.save(tmpStr);
 }

 void saveFileColor(pop::Mat2RGBUI8 img, const string fileDirectory, const string fileName)
 {
	 stringstream ss_tmp;
	 string s_tmp;
	 char *tmpStr = new char[FILENAME_LENGTH];
	 ss_tmp << fileDirectory << fileName;
	 s_tmp=ss_tmp.str();
	 copy(s_tmp.begin(), s_tmp.end(), tmpStr);
	 tmpStr[s_tmp.size()] = '\0';

	 restoreRotation(img);
	 img.save(tmpStr);
 }

  void saveFileColor(const Mat2UI8 &img, const string fileDirectory, const string fileName)
 {
	 stringstream ss_tmp;
	 string s_tmp;
	 Mat2RGBUI8 color_image;

	 // Initializing
	 color_image = Mat2RGBUI8 (img.sizeI(), img.sizeJ());

	 // Coloring the image if it was provided in shades of gray
	 colorize_image(color_image, img);

	 char *tmpStr = new char[FILENAME_LENGTH];
	 ss_tmp << fileDirectory << fileName;
	 s_tmp=ss_tmp.str();
	 copy(s_tmp.begin(), s_tmp.end(), tmpStr);
	 tmpStr[s_tmp.size()] = '\0';

	 restoreRotation(color_image);
	 color_image.save(tmpStr);
 }

 void save_histogram (const Mat2F64 &histogram, string fileDirectory, string fileName)
 {
	// Variables
	int i;
	ofstream file_stream;
	stringstream ss_complete_path;

	log_steps("Saving a histogram");

	// Making the path
	ss_complete_path << fileDirectory << fileName;

	// Creating a char pointer of a string
	std::string temp_string (ss_complete_path.str());
	char * temp_char = new char[temp_string.size() + 1];
	std::copy(temp_string.begin(), temp_string.end(), temp_char);
	temp_char[temp_string.size()] = '\0'; // don't forget the terminating 0

	file_stream.open (temp_char);
	// Cleaning up
	delete[] temp_char;




	for (i=0; i< histogram.sizeJ(); i++)			
	{
		file_stream << histogram(0,i)	<<	"\t";
	}
	file_stream	<<	"\n";

	for (i=0; i< histogram.sizeJ(); i++)			
	{
		file_stream << histogram(1,i)	<<	"\t";
	}

	file_stream.close();

	log_steps();
	 


 }

 void colorize_image(Mat2RGBUI8 &img_color, const Mat2UI8 &img)
{
	
	for(int i=0;i<img.sizeI();i++)
	{
		for(int j=0;j<img.sizeJ();j++)
		{
			if(img(i,j)==int_white_pixel_color) // IVS - yellow
			{
				img_color(i,j).r()=int_white_pixel_color;
				img_color(i,j).g()=int_white_pixel_color;
				img_color(i,j).b()=int_black_pixel_color;
			}
			else if(img(i,j)==int_black_pixel_color)	// Villi without GR - blue
			{
				img_color(i,j).r()=int_black_pixel_color;
				img_color(i,j).g()=int_black_pixel_color;
				img_color(i,j).b()=int_white_pixel_color;
			}
			else if(img(i,j)==int_fetal_RBCs_pixel_color) // GR inside - magenta
			{
				img_color(i,j).r()=int_white_pixel_color;
				img_color(i,j).g()=int_black_pixel_color;
				img_color(i,j).b()=int_white_pixel_color;
			}
			else if(img(i,j)==int_maternal_RBCs_pixel_color) // GR outside - cyan
			{
				img_color(i,j).r()=int_black_pixel_color;
				img_color(i,j).g()=int_white_pixel_color;
				img_color(i,j).b()=int_white_pixel_color;
			}
		}
	}
}

void deletePointsCross(Mat2UI32 &label_region)
{
	log_steps("Deleting cross points");
	//cout<<endl<<"start: delete Points Cross"<<endl;
	int label;
	for(int i=1;i<label_region.sizeI()-1;i++)
	{
		for(int j=1;j<label_region.sizeJ()-1;j++)
		{
			label=label_region(i,j);
			if(label!=0 && label_region(i-1,j)==label && label_region(i+1,j)==label && label_region(i,j-1)==label && label_region(i,j+1)==label)
			{
				label_region(i,j)=0;
			}
		}
	}
	//cout<<"end: delete Points Cross"<<endl<<endl;
	log_steps();
}

void deletePointsSingle(Mat2UI32 &label_region)
{
	log_steps("Deleting single points");
	//cout<<endl<<"start: delete Points Single"<<endl;
	int label, a, b, c, d, e, f, g, h, nb=1;
	while(nb!=0)
	{
		nb=0;
		for(int i=1;i<label_region.sizeI()-1;i++)
		{
			for(int j=1;j<label_region.sizeJ()-1;j++)
			{
				label=label_region(i,j);
				a=(label_region(i-1,j-1)==label);
				b=(label_region(i-1,j)==label);
				c=(label_region(i-1,j+1)==label);
				d=(label_region(i,j-1)==label);
				e=(label_region(i,j+1)==label);
				f=(label_region(i+1,j-1)==label);
				g=(label_region(i+1,j)==label);
				h=(label_region(i+1,j+1)==label);
				if(label!=0 && a+b+c+d+e+f+g+h==1)
				{
					label_region(i,j)=0;
					nb+=1;
				}
			}
		}
	}
	//cout<<"end: delete Points Single"<<endl<<endl;
}

void labelToRGB(Mat2RGBUI8 &img, Mat2UI32 &label)
{
	log_steps("Labeling the image to the RGB format");
	//cout<<endl<<"start: label an image to RGB format"<<endl;
	time_t tbegin, tend;
	double texec=0.;	
	//start timer
	tbegin=time(NULL);

	int value1=234, value2=27, value3=33, number;
	Mat2UI32 rgb(pop::Analysis::maxValue(label), 3);
	for(int i=0;i<label.sizeI();i++)
	{
		for(int j=0;j<label.sizeJ();j++)
		{
			number=label(i,j);
			if(number!=0)
			{
				switch(number)
				{
				case 1:	rgb(number-1,0)=255; rgb(number-1,1)=0; rgb(number-1,2)=0; break;
				case 2:	rgb(number-1,0)=0; rgb(number-1,1)=255; rgb(number-1,2)=0; break;
				case 3:	rgb(number-1,0)=0; rgb(number-1,1)=0; rgb(number-1,2)=255; break;
				case 255: rgb(number-1,0)=120; rgb(number-1,1)=120; rgb(number-1,2)=120; break;
				default: rgb(number-1,0)=abs((value1*number+56))%255; rgb(number-1,1)=abs((value2*number+53))%255; rgb(number-1,2)=abs((value3*number+11))%255; break;
				}
			}
		}
	}
	for(int i=0;i<label.sizeI();i++)
	{
		for(int j=0;j<label.sizeJ();j++)
		{
			if(label(i,j)!=0)
			{
				img(i,j).r()=rgb(label(i,j)-1,0);
				img(i,j).g()=rgb(label(i,j)-1,1);
				img(i,j).b()=rgb(label(i,j)-1,2);
			}
		}
	}
	log_steps();		// End: "Labeling the image to the RGB format"
	/*
	//end timer
	tend = time(NULL);
	//Compute exectution time
	texec = difftime(tend, tbegin);
	cout<<"It takes "<<texec<<"s to label an image to RGB format!"<<endl;
	cout<<"end: label an image to RGB format"<<endl<<endl;
	*/
}

void ordreContour(Mat2UI32 &label_region, vector <vector <int> > &x, vector <vector <int> > &y)
{
	log_steps("Extracting contours");
	//cout<<endl<<"start: extract contour"<<endl;
	time_t tbegin, tend;
	double texec=0.;	
	//start timer
	tbegin=time(NULL);

	Mat2UI32 img_enlarge(label_region.sizeI()+2, label_region.sizeJ()+2);
	for(int p=1;p<img_enlarge.sizeI()-1;p++)
	{
		for(int q=1;q<img_enlarge.sizeJ()-1;q++)
		{
			img_enlarge(p,q)=label_region(p-1,q-1);
		}
	}
	Mat2UI32 order(2,15);
	order(0,0)=-1; order(0,1)=0; order(0,2)=1; order(0,3)=1; order(0,4)=1; order(0,5)=0; order(0,6)=-1; order(0,7)=-1; order(0,8)=-1; order(0,9)=0; order(0,10)=1; order(0,11)=1; order(0,12)=1; order(0,13)=0; order(0,14)=-1; 
	order(1,0)=-1; order(1,1)=-1; order(1,2)=-1; order(1,3)=0; order(1,4)=1; order(1,5)=1; order(1,6)=1; order(1,7)=0; order(1,8)=-1; order(1,9)=-1; order(1,10)=-1; order(1,11)=0; order(1,12)=1; order(1,13)=1; order(1,14)=1; 
	
	int maxLabel=pop::Analysis::maxValue(label_region), nb, m, startPoint=0;// m: indice of matrix order;
	int i, j, a, b, c, d, e, f, g, h;

	for(int label=1;label<=maxLabel;label++)
	{		
		startPoint=0;
		for(int p=1;p<img_enlarge.sizeI()-1;p++)
		{
			for(int q=1;q<img_enlarge.sizeJ()-1;q++)
			{
				if(img_enlarge(p,q)==label)
				{
					i=p;
					j=q;
					startPoint=1;
					goto extraction;
				}
			}
		}
		extraction:
		while(img_enlarge(i,j)==label)
		{																									
			x[label-1].push_back(i-1);
			y[label-1].push_back(j-1);
			img_enlarge(i,j)=0;			
			nb=0;
			m=0;
			a=(img_enlarge(i-1,j-1)==label);
			b=(img_enlarge(i,j-1)==label);
			c=(img_enlarge(i+1,j-1)==label);
			d=(img_enlarge(i+1,j)==label);
			e=(img_enlarge(i+1,j+1)==label);
			f=(img_enlarge(i,j+1)==label);
			g=(img_enlarge(i-1,j+1)==label);
			h=(img_enlarge(i-1,j)==label);
			vector <int> neighbours(8);
			neighbours[0]=a; neighbours[1]=b; neighbours[2]=c; neighbours[3]=d; neighbours[4]=e; neighbours[5]=f; neighbours[6]=g; neighbours[7]=h;
			if(a+b+c+d+e+f+g+h>1 && startPoint>3 && ((a==1 && b==1 && h==1 && c==0 && d==0 && e==0 && f==0 && h==0) || (a==1 && d==1 && b==0 && c==0 && e==0 && f==0 && g==0  && h==0) || (c==1 && e==1 && b==0 && a==0 && d==0 && f==0 && g==0  && h==0) || (b==1 && d==1 && e==1 && a==0 && c==0 && f==0 && g==0  && h==0)   || (f==1 && g==1  && h==1 && b==0 && c==0 && e==0 && a==0 && d==0) || (a==1 && c==1 && b==0 && d==0 && e==0 && f==0 && g==0  && h==0)))
			{
				for(int ind=0; ind<7; ind++)
				{
					for(int step=ind+1; step<8;step++)
					if(neighbours[ind]!=0 && neighbours[ind]==neighbours[step])
					{
						m=m+step;
						while(nb!=1 && m<8+step)
						{
							if(img_enlarge(i+order(0,m),j+order(1,m))==label)
							{
								i=i+order(0,m);
								j=j+order(1,m);
								nb+=1;
							}
							m+=1;
						}
					}
				}
			}
			else
			{
				while(nb!=1 && m<8)
				{
					if(img_enlarge(i+order(0,m),j+order(1,m))==label)
					{
						i=i+order(0,m);
						j=j+order(1,m);
						nb+=1;
					}
					m+=1;
				}	
				startPoint+=1;
			}
			
		}
	}

	for(int i=0;i<x.size();i++)
	{
		if(x[i].size()==0)
		{
			x.erase(x.begin()+i);
			y.erase(y.begin()+i);
			i=i-1;
		}
	}
	/*
	//end timer
	tend = time(NULL);
	//Compute exectution time
	texec = difftime(tend, tbegin);
	cout<<"It takes "<<texec<<"s to get the contour!"<<endl;
	cout<<"end: extract contour"<<endl<<endl;
	*/
}

void binaryOpposite(Mat2UI8 &img)
{
	for(int i=0;i<img.sizeI();i++)
	{
		for(int j=0;j<img.sizeJ();j++)
		{
			if(img(i,j)==int_white_pixel_color)
			{
				img(i,j)=int_black_pixel_color;
			}
			else
			{
				img(i,j)=int_white_pixel_color;
			}
		}
	}
}


std::string string_output_picutre_number()
{
	static int output_picture_number=1;
	char *string_temp=new char[500];
	sprintf(string_temp,"_%02i",output_picture_number++);
	return string_temp;
}





// Set 0 pixels to 255, and all others to 0
pop::Mat2UI8 invertBinaryImage(pop::Mat2UI8 image)
{
	int size_i(image.sizeI());
	int size_j(image.sizeJ());
	
	for (int i=0; i<size_i; i++)
		for (int j=0; j<size_j; j++)
			image(i,j)=image(i,j)>0 ? int_black_pixel_color : int_white_pixel_color;

	return image;
}