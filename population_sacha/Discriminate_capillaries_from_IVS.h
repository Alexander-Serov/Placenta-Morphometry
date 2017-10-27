#ifndef DISCRIMINATE_CAPILLARIES_FROM_IVS
#define DISCRIMINATE_CAPILLARIES_FROM_IVS


#include"Population.h"
#include<string>


void discriminate_capillaires_from_IVS(pop::Mat2UI8& img,
									   // pop::Mat2RGBUI8& img_3objets,
									   // pop::Mat2UI8& img_3objets_gray,
									   // pop::Mat2UI8& img_final,
									   // pop::Mat2UI8& img_segmentation,
									   //const std::string img_name_withoutExtension,
									   const float percentage_threshold_of_IVS_on_the_internal_IVS_boundary_for_a_capillary,
									   const pop::Mat2RGBUI8& initial_3_regions_segmentation
									   //const std::string resultDir
									   );



#endif


