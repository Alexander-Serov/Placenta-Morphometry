#ifndef OUTPUT_STATISTICS_H
#define OUTPUT_STATISTICS_H



#include <string>		// std::string
#include "Population.h"



void output_statistics(//const char *resultDir, 
					   //const std::string& img_name_withoutExtension,
					   const int size_i,
					   const int size_j,
					   const float perIVS,
					   const float villi_density,
					   const pop::VecF64& varea_img_segment,
					   const pop::VecF64& vperimeter_img_segment,
					   const pop::VecF64& varea_IVS_inside_segment,
					   const pop::VecF64& vperimeter_IVS_inside_segment,
					   const pop::VecF64& varea_GR_segment,
					   const pop::VecF64& vperimeter_GR_segment,
					   const pop::VecF64& varea_IVS_outside_segment,
					   const pop::VecF64& vperimeter_IVS_outside_segment,
					   const pop::VecF64& varea_GR_insideIVSoutside,
					   const pop::VecF64& vperimeter_GR_insideIVSoutside,
					   const pop::VecF64& varea_GR_insideIVSinside,
					   const pop::VecF64& vperimeter_GR_insideIVSinside,
					   const std::vector <std::vector <int> >& perimeter_img_segment_frame,
					   const std::vector <std::vector <int> >& numberIVSinside,
					   const std::vector <std::vector <int> >& numberGRinsideRegion,
					   const std::vector <std::vector <int> >& perimeter_IVS_outside_frame,
					   const std::vector <std::vector <int> >& numberGR_insideIVSoutside,
					   const std::vector <std::vector <int> >& numberGR_insideIVSinside
					   );











#endif