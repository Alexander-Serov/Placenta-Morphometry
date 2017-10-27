

#include "Population.h"
#include "class_RLE_Image.h"
#include "Log_steps.h"


void open_regions(pop::Mat2UI8& img_binary,
				   int erosion_radius
				   )
{
	log_steps ("Opening regions");
	// The input is a binary image. This function works on its white (>0) regions
	RLE_Image rle_working_image (img_binary);

	// Eroding
	rle_working_image = rle_working_image.erode(erosion_radius);

		// Dilating
	rle_working_image = rle_working_image.dilate(erosion_radius);

	// Saving and returning the result

	img_binary = rle_working_image.output();

	log_steps();	// End

}




/*
void close_regions(pop::Mat2UI8 img_3_objects,
				   int int_region_color,
				   int erosion_radius
				   )
{
	// Preparing an image consisting only of the given color


}
*/




