#ifndef GLOBAL_VARIABLES
#define GLOBAL_VARIABLES

#include <string>



extern const bool bl_consider_RBCs_as_villi;
extern const bool bl_clear_results_folder;
extern const int FILENAME_LENGTH;
extern const int int_white_pixel_color;
extern const int int_black_pixel_color;
extern const int int_maternal_RBCs_pixel_color;
extern const int int_fetal_RBCs_pixel_color;

// General parameters (non-related to images themselves)
extern const bool bl_clear_results_folder;		// Normally keep at false


// Name and path of the opened image
extern std::string extension;
extern std::string str_img_name_withoutExtension;
extern char *mainDir;
extern char *resultDir;


// Defined in Segementation_parameters.h

// Basic parameters
extern const float Pi_number;
extern const float Image_resolution;
extern const float Relative_precision;
extern const float Minimal_villi_radius_pixels;
extern const double Minimal_villi_area_pixels;
extern const float RBC_radius_microns;
extern const float RBC_radius_pixels;
extern const float Typical_RBC_area_pixels;


// Histogram analysis
extern const int number_of_neighboring_bins_to_consider;
extern const float histogram_regions_borders[];
extern const float default_histogram_maxima_locations [];
extern const int histogram_bins_number;


//Smoothing the initial RBC and IVS regions segmentations
extern const float RBC_smoothing_radius_microns;
extern const int RBC_smoothing_radius_pixels;

extern const float IVS_smoothing_radius_microns;
extern const int IVS_smoothing_radius_pixels;

extern const float Villi_smoothing_radius_microns;
extern const int Villi_smoothing_radius_pixels;


// Deciding which parameters to take based on the complexity of the image
extern const float IVS_percentage_threshold_to_consider_the_image_as_complicated;
extern const float Percentage_of_IVS_on_the_internal_IVS_boundary_complicated_image;
extern const float Percentage_of_IVS_on_the_internal_IVS_boundary_simple_image;
//extern const float area_threshold_complicated_image;
//extern const float area_threshold_simple_image;
extern const float Area_threshold_IVS_without_RBCs;


// Removing noise
extern const float Remove_villi_area_threshold_pixels;
extern const float Remove_IVS_area_threshold_pixels;


// Smoothing out the contours
extern const float erosion_radius_villi_microns;
extern const int erosion_radius_villi_pixels;

extern const float Minimal_eroded_villi_radius_pixels;
extern const float Remove_eroded_villi_area_threshold_pixels;

extern const float erosion_radius_fetal_capillaries_microns;
extern const int erosion_radius_fetal_capillaries_pixels;


// Statistics
extern const bool bl_extract_contours;


// Platform-specific commands
#ifdef _WIN32
	#define REMOVE_FOLDER_COMMAND "rmdir /S /Q "
#elif __linux__
	#define REMOVE_FOLDER_COMMAND "rm -rf "
#endif



#endif