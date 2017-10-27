#pragma once
#ifndef REMOVE_IVS_INSIDE_VILLI_H
#define REMOVE_IVS_INSIDE_VILLI_H


#include <string> 
#include "Population.h"



//using namespace pop;
//using namespace std;



void remove_IVS_inside_villi (pop::Mat2UI8& working_image
										 //float thresholdArea, 
										 //const std::string resultDir,
										 //const std::string img_name_withoutExtension
							);

pop::VecF64 calcualte_percentage_of_a_color_on_contours (pop::Mat2UI8 colored_pixels_image, pop::Mat2UI32 labeled_contours_image,
														int color_to_search_on_the_border);


#endif