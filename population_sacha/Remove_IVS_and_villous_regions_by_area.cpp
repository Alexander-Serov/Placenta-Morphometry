


#include "Population.h"
#include "Global_variables.h"
#include "Small_functions.h"
#include "Log_steps.h"
#include "Statistics_placenta.h"





void remove_IVS_and_villous_regions_by_area(pop::Mat2UI8& working_image
				  //const std::string resultDir,				  
				  //const std::string img_name_withoutExtension
				  )
{
	// Local variables
	pop::Mat2RGBUI8 img_3objets;
	
	log_steps("Removing IVS and villous regions by area");

	// Initialization
	

	// In this cycle the RBCs are temporarily replaced by the IVS color for the subsequent processing
	// The new image is black and white (boolean)
	// This is necessary to consider the IVS together with its RBCs. RBCs should later be restored, but only on the IVS and not on villi (??)
	for(int i=0;i<working_image.sizeI();i++)
	{
		for(int j=0;j<working_image.sizeJ();j++)
		{
			if(working_image(i,j)!=int_black_pixel_color)
			{
				working_image(i,j)=int_white_pixel_color;
			}
		}
	}
		
	// Interesting, which of the following replacements should I perform first...

	// Deleting small villous regions by area criterion. Too small villi are not considered functional
	conditionally_replace_color_1_by_color_2 (working_image, int_black_pixel_color, int_white_pixel_color, 
		Remove_villi_area_threshold_pixels);	
	
	// Saving
	saveFileColor (working_image, resultDir, str_img_name_withoutExtension+string_output_picutre_number()+\
		"_Small_villous_regions_deleted.png");
	
	
	// Deleting small IVS regions by area criterion. RBCs are considered as a part of the IVS at this point
	conditionally_replace_color_1_by_color_2 (working_image, int_white_pixel_color, int_black_pixel_color, Remove_IVS_area_threshold_pixels);
	
	// Saving
	saveFileColor (working_image, resultDir, str_img_name_withoutExtension + string_output_picutre_number()+\
		"_Small_IVS_plus_RBC_regions_deleted.png");

	log_steps();


}



