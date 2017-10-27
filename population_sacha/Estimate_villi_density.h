#ifndef ESTIMATE_VILLI_DENSITY
#define ESTIMATE_VILLI_DENSITY


#include "Population.h"




float estimate_villi_density(pop::Mat2UI8& img_3objets_gray,
							//float& thresholdArea,
							const double Minimal_villi_area_pixels,
							float& percentage_threshold_of_IVS_on_the_internal_IVS_boundary_for_a_capillary,
							bool& bl_image_is_complicated
							);



#endif