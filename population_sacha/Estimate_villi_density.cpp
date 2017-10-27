


#include "Population.h"
#include <string>			// std:: string
#include <sstream>			// std::stringstream
#include "Global_variables.h"

#include "Log_steps.h"





using namespace pop;
using namespace std;







float estimate_villi_density(Mat2UI8& img_3objets_gray,
							//float& thresholdArea,
							const double Minimal_villi_area_pixels,
							float& percentage_threshold_of_IVS_on_the_internal_IVS_boundary_for_a_capillary,
							bool& bl_image_is_complicated
							)
{

	// Local variables
	float percent_of_IVS;
	float thresholdArea;

	int IVS_pixels_number=0;

	stringstream str_stream;


	// Calculating the number of IVS-assigned pixels
	for(int i=0;i<img_3objets_gray.sizeI();i++)
	{
		for(int j=0;j<img_3objets_gray.sizeJ();j++)
		{
			if(img_3objets_gray(i,j)==int_white_pixel_color)
			{
				IVS_pixels_number += 1;
			}
		}
	}


	percent_of_IVS= (float) IVS_pixels_number/(img_3objets_gray.sizeI()*img_3objets_gray.sizeJ());
	str_stream << "The percentage of the white space is estimated at " << percent_of_IVS << ".\n " << 
		"The current threshold to consider the image as complicated is " << IVS_percentage_threshold_to_consider_the_image_as_complicated;
	//log_steps (str_stream.str());
	
	if(percent_of_IVS<=IVS_percentage_threshold_to_consider_the_image_as_complicated)
	{
		str_stream << "The image is considered complicated";
		percentage_threshold_of_IVS_on_the_internal_IVS_boundary_for_a_capillary = Percentage_of_IVS_on_the_internal_IVS_boundary_complicated_image;
		//thresholdArea = area_threshold_complicated_image;
		bl_image_is_complicated=true;
	}
	else
	{
		str_stream << "The image is considered simple";
		percentage_threshold_of_IVS_on_the_internal_IVS_boundary_for_a_capillary = Percentage_of_IVS_on_the_internal_IVS_boundary_simple_image;
		//thresholdArea = area_threshold_simple_image;
		bl_image_is_complicated=false;
	}

	log_steps (str_stream.str());
	log_steps();		// End: str_stream.str()
	return percent_of_IVS;
}