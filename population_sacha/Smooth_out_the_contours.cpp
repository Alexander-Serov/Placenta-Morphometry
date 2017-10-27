


#include "Global_variables.h"
#include "Population.h"
#include "Small_functions.h"
#include "Log_steps.h"
//#include "Fast_erosion_and_dilation.h"
#include "class_RLE_Image.h"








void smooth_out_the_contours (pop::Mat2UI8& input_image
							  //const std::string resultDir,
							  //const std::string img_name_withoutExtension
							  )
{
	// Local variables
	int i;
	int j;
	int size_i;
	int size_j;

	pop::Mat2UI8 bl_villi_and_capillaries;
	pop::Mat2UI8 bl_fetal_capillaries;
	
	pop::Mat2UI8 output_image;



	// In a simple image, we need to smooth out two types of regions only: villous stroma and fetal capillairies.
	// Everything else is IVS by default

	log_steps("Smoothing out the contours");
	
	// Initialization
	size_i=input_image.sizeI();
	size_j=input_image.sizeJ();

	bl_villi_and_capillaries = pop::Mat2UI8 (size_i, size_j);
	bl_villi_and_capillaries.fill(false);
	
	bl_fetal_capillaries = pop::Mat2UI8 (size_i, size_j);
	bl_fetal_capillaries.fill(false);

	output_image = pop::Mat2UI8 (size_i, size_j);
	output_image.fill(int_white_pixel_color);
	
	
	// Getting boolean-type images for villi_and_capillaries and only capillaries
	for (i=0; i<size_i; i++)
	{
		for (j=0; j<size_j; j++)
		{
			if (input_image(i,j)==int_black_pixel_color ||
				input_image(i,j)==int_fetal_RBCs_pixel_color)
					bl_villi_and_capillaries(i,j)=true;

			if (input_image(i,j)==int_fetal_RBCs_pixel_color)
				bl_fetal_capillaries(i,j)=true;

		}
	}

	
	log_steps("Smoothing out the villi");
	// Smoothing out villous outside contours by eroding and dilating with the same radius
	RLE_Image rle_image_villi_and_capillaries (bl_villi_and_capillaries);
	log_steps("Erosion");
	rle_image_villi_and_capillaries = rle_image_villi_and_capillaries.erode(erosion_radius_villi_pixels, "round");
	//saveFileColor(invertBinaryImage(rle_image_villi_and_capillaries.output()), resultDir, img_name_withoutExtension+string_output_picutre_number()+"_Eroded_villi_contours.png");
	log_steps();

	// Once again applying the same area threshold to the villi as before. Too small villi are not considered functional
	// A conversion needs to be performed to a normal representation to allow for area thresholding
	// Remember that villi are white in this image
	log_steps("Removing small eroded villi based on area threshold");
	bl_villi_and_capillaries = rle_image_villi_and_capillaries.output();
	conditionally_replace_color_1_by_color_2 (bl_villi_and_capillaries, int_white_pixel_color, int_black_pixel_color,
		Remove_eroded_villi_area_threshold_pixels);	
	rle_image_villi_and_capillaries = RLE_Image (bl_villi_and_capillaries);
	log_steps(); // End: Removing small villi based on area threshold

	log_steps("Dilation");
	rle_image_villi_and_capillaries=rle_image_villi_and_capillaries.dilate(erosion_radius_villi_pixels, "round");
	bl_villi_and_capillaries = rle_image_villi_and_capillaries.output();
	log_steps();
	//saveFileColor(invertBinaryImage(bl_villi_and_capillaries), resultDir, img_name_withoutExtension+string_output_picutre_number()+"_Dilated_villi_contours.png");
	log_steps();		// End: "Smoothing out the villi"




	// Smoothing out fetal capillary contours
	log_steps("Smoothing out the fetal capillaries");
	RLE_Image rle_fetal_capillaries(bl_fetal_capillaries);
	log_steps("Erosion");
	rle_fetal_capillaries = rle_fetal_capillaries.erode (erosion_radius_fetal_capillaries_pixels, "round");
	log_steps();
	//saveFileColor(invertBinaryImage(rle_fetal_capillaries.output()), resultDir, img_name_withoutExtension+string_output_picutre_number()+"_Eroded_fetal_capillaries.png");
	log_steps("Dilation");
	rle_fetal_capillaries = rle_fetal_capillaries.dilate (erosion_radius_fetal_capillaries_pixels, "round");
	bl_fetal_capillaries=rle_fetal_capillaries.output();
	//saveFileColor(invertBinaryImage(bl_fetal_capillaries), resultDir, img_name_withoutExtension+string_output_picutre_number()+"_Dilated_fetal_capillaries.png");
	log_steps();		// End: Dilation
	log_steps();		// End: "Smoothing out the fetal capillaries"



	// Smoothing out completed. Reconstructing the image
	for (i=0; i<size_i; i++)
	{
		for (j=0; j<size_j; j++)
		{
			if (bl_villi_and_capillaries(i,j))
			{
				output_image(i,j)=int_black_pixel_color;
				if(bl_fetal_capillaries(i,j))		// Fetal capillaries can only lie inside villi
					output_image(i,j)=int_fetal_RBCs_pixel_color;
			}
		}
	}


	// Saving results
	saveFileColor(output_image, resultDir, str_img_name_withoutExtension+string_output_picutre_number()+"_Final_result.png");


	// Returning the results to the calling procedure
	input_image=output_image;

	log_steps();		// End: "Smoothing out the contours"



}



