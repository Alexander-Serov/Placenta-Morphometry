#ifndef SEGMENTATION_PARAMETERS
#define SEGMENTATION_PARAMETERS


// Include this file only once in the main.cpp
// For other files use Global_variables.h


// General parameters (non-related to images themselves)
extern const bool bl_clear_results_folder=false;		// Normally keep at false



// Basic parameters
extern const float Pi_number = 3.1415926f;
extern const float Image_resolution = 0.498f; // µm/Pixel
//extern const float Absolute_precision = 1/10000.0;	//10^-5
extern const float Relative_precision = 1/10000.0f;	//10^-5
extern const float Minimal_villi_radius_microns = 25;	// \mu m
extern const float Minimal_villi_radius_pixels = Minimal_villi_radius_microns / Image_resolution;	// Pixels
extern const double Minimal_villi_area_pixels = Pi_number * Minimal_villi_radius_pixels * Minimal_villi_radius_pixels;
extern const float RBC_radius_microns = 3;	// \mu m
extern const float RBC_radius_pixels = RBC_radius_microns / Image_resolution;	// Pixels
extern const float Typical_RBC_area_pixels = Pi_number * RBC_radius_pixels * RBC_radius_pixels;


// Histogram analysis
extern const int number_of_neighboring_bins_to_consider=3;		// Number of neighbouring bins to consider when looking
	//for a local maximum (including the central one). Must be an odd number
extern const float histogram_regions_borders [] = {0.0f, 0.42f, 0.7f, 1.0f};	// Regions of the histogram to search for the local maxima (relative to the width of the histogram)
extern const float default_histogram_maxima_locations [] = {0.42f, 0.65f, 0.85f};	// Relative to the width of the histogram
extern const int histogram_bins_number=30;


//Smoothing the initial RBC and IVS regions segmentations
extern const float RBC_smoothing_radius_microns = RBC_radius_microns/2;
extern const int RBC_smoothing_radius_pixels = ceil(RBC_smoothing_radius_microns/Image_resolution);

extern const float IVS_smoothing_radius_microns = RBC_radius_microns /2;
extern const int IVS_smoothing_radius_pixels = ceil(IVS_smoothing_radius_microns/Image_resolution);

//extern const float Villi_smoothing_radius_microns = RBC_radius_microns / 2;
//extern const int Villi_smoothing_radius_pixels = ceil(Villi_smoothing_radius_microns/Image_resolution);


// Deciding which parameters to take based on the complexity of the image
extern const float IVS_percentage_threshold_to_consider_the_image_as_complicated=0.2f;		// Below this percentage the image is considered as complicated
//extern const float area_threshold_simple_image = Minimal_villi_area_pixels / 2;	// IVS regions with not more than half the area of the smallest villus are removed. This number may be to large. To think
extern const float Area_threshold_IVS_without_RBCs = Minimal_villi_area_pixels / 2;	// IVS regions with not more than half the area of the smallest villus are removed. This number may be to large. To think
// extern const float area_threshold_complicated_image = Minimal_villi_area_pixels / 2;		// The threshold is switched off in complicated images, because 
																// IVS regions are already small, and it happens that they may not 
																// contain RBCs, so we should not remove them
extern const float Percentage_of_IVS_on_the_internal_IVS_boundary_complicated_image = 0.7f;	// Percentage threshold of RBCs on the internal boundary of an IVS region below which it is considered as true maternal IVS and above - a fetal capillary
extern const float Percentage_of_IVS_on_the_internal_IVS_boundary_simple_image = 0.5f;



// Removing noise
extern const float Remove_villi_area_threshold_pixels = Minimal_villi_area_pixels /2;
extern const float Remove_IVS_area_threshold_pixels = Typical_RBC_area_pixels;


// Smoothing out the contours
extern const float erosion_radius_villi_microns = (float) RBC_radius_microns * 2;
extern const int erosion_radius_villi_pixels = ceil (erosion_radius_villi_microns / Image_resolution);

extern const float Minimal_eroded_villi_radius_pixels = Minimal_villi_radius_pixels - erosion_radius_villi_pixels;
extern const float Remove_eroded_villi_area_threshold_pixels = Pi_number* Minimal_eroded_villi_radius_pixels * 
	Minimal_eroded_villi_radius_pixels /2 ;

extern const float erosion_radius_fetal_capillaries_microns = (float) RBC_radius_microns / 2;
extern const int erosion_radius_fetal_capillaries_pixels = ceil (erosion_radius_fetal_capillaries_microns / Image_resolution);


// Statistics
extern const bool bl_extract_contours=false;


#endif
