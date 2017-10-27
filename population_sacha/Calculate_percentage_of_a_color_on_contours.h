#pragma once
#ifndef CALCULATE_PERCENTAGE_OF_A_COLOR_ON_CONTOURS
#define CALCULATE_PERCENTAGE_OF_A_COLOR_ON_CONTOURS



#include "Population.h"


pop::VecF64 calculate_percentage_of_a_color_on_contours (pop::Mat2UI8 colored_pixels_image, pop::Mat2UI32 labeled_contours_image,
														int color_to_search_on_the_border);


#endif