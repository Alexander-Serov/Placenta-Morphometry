//#pragma once		// Include only once
#ifndef SMALL_FUNCTIONS
#define SMALL_FUNCTIONS


#include <string>
#include "Population.h"





// Functions
void binaryOpposite(pop::Mat2UI8 &);
void colorize_image(pop::Mat2RGBUI8 &, const pop::Mat2UI8 &);
void curvature(pop::Mat2UI8, pop::Mat2UI32);
void decideContourisObject(pop::Mat2UI8 &, pop::Mat2UI32 , pop::Mat2UI8 &, unsigned char, float, float, bool, std::string);
//void remove_IVS_inside_villi(pop::Mat2UI8 &, unsigned char, unsigned char, unsigned char, float, float, bool, std::string);
void deletePointsCross(pop::Mat2UI32 &);
void deletePointsSingle(pop::Mat2UI32 &);
//void find_local_minima_in_the_histogram(Mat2F64 &, Mat2F64 &, Mat2F64 &, int, int);
//void histogram(Mat2F64 &, Mat2F64 &, int);
void labelObjet(pop::Mat2RGBUI8& img_colored_regions, const pop::Mat2UI32& img_labeled_regions, int textSize, int position_start);	// Print label numbers on the picture
void labelToRGB(pop::Mat2RGBUI8 &, pop::Mat2UI32 &);
void mixImages(int n_subFolder_lin, int n_subFolder_col, std::string imgResultFileDir, std::string fileName);
void ordreContour(pop::Mat2UI32 &, std::vector <std::vector <int> > &, std::vector <std::vector <int> > &);
void perimeter(pop::VecF64 &, pop::Mat2UI32 &);
void conditionally_replace_color_1_by_color_2(pop::Mat2UI8 &img_original, unsigned char old_color, unsigned char new_color, 
						   float area_threshold);
void saveFileBinary(pop::Mat2UI8 img, std::string fileDirectory, std::string fileName);
void saveFileColor(const pop::Mat2RGBUI8 &, const std::string, const std::string);
void saveFileColor(const pop::Mat2UI8 &img, const std::string fileDirectory, const std::string fileName);
void save_histogram (const pop::Mat2F64 &histogram, std::string fileDirectory, std::string fileName);
std::string string_output_picutre_number();
pop::Mat2UI8 invertBinaryImage(pop::Mat2UI8 image);		// Set 0 pixels to 255, and all others to 0


#endif
