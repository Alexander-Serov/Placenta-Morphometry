#ifndef EXTRACT_CONTOURS_H
#define EXTRACT_CONTOURS_H


#include <string>		// std::string
#include "Population.h"


void extract_contours(const pop::Mat2UI8& img_3_objects,
					  pop::Mat2UI8& IVS_outside_segment,
					  pop::Mat2UI8& IVS_inside_segment,
					  pop::Mat2UI8& IVS_inside_tmp,
					  char *resultDir, 
					  std::string& img_name_withoutExtension
					  );









#endif