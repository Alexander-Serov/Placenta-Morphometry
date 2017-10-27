#ifndef OPEN_REGIONS_H
#define OPEN_REGIONS_H


#include "Population.h"


// This function performs erosion and then dilation of the white regions (>0) in a binary image with a given erosion radius
void open_regions(pop::Mat2UI8& img_binary,
				   int erosion_radius
				   );


/*


				   // This function performs dilation and then erosion of the regions of the given color with a given erosion radius
void open_regions(pop::Mat2UI8 img_3_objects,
				   int int_region_color,
				   int erosion_radius
				   );


*/










#endif