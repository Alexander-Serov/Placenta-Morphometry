#include "Population.h"
#include <string>

#include "Log_steps.h"
#include "Global_variables.h"
#include "Small_functions.h"
#include "Calculate_percentage_of_a_color_on_contours.h"






// Every regions in the image is analyzed according to two criteria: its total area and the percentage of its border occupied
// by a certain type of pixels. The corresponding numerical criteria are different for "simple" and "complicated" images
//
// The calculated region contours are external contours. There are no pixels of other color on the inside contours
//
// The objects of the color objet1 are replaced by the color objet2 if they are inside an object of reference color and 
// if they satisfy the outlined criteria
//
// bl_ at the beginning of variable names means that they can contain only 1 or 0 (boolean variables)






template <typename TYPE>
pop::Vec<TYPE> shift_vector_by_one (const pop::Vec<TYPE>& input_vector);





void remove_IVS_inside_villi (pop::Mat2UI8& working_image 
										 //float thresholdArea, 
										 //const std::string resultDir,
										 //const std::string img_name_withoutExtension
							)

	/*
							pop::Mat2UI8 &working_image, const unsigned char old_object_color, 
										const unsigned char new_object_color, const unsigned char color_to_search_on_the_border, 
										const float thresholdArea)
										*/
{
	// Local variables
	float perObjet1;

	int dilated_label;
	int index;
	int i;
	int j;
	int label;
	int number_of_contours;
	int number_of_dilated_labels;
	int number_of_original_labels;
	int original_label;
	int size_i;
	int size_j;
	int old_object_color;
	int new_object_color;
	int color_to_search_on_the_border;

	pop::Mat2RGBUI8 img_3objets;

	pop::Mat2UI8 bl_regions_contours;
	pop::Mat2UI8 bl_dilated_image;
	pop::Mat2UI8 bl_old_color_matrix;

	pop::Mat2UI32 labeled_dilated_image;
	pop::Mat2UI32 labeled_original_image;
	pop::Mat2UI32 labeled_contours_image;
		
	pop::VecF64 region_areas_in_the_original_image;
	pop::VecF64 percentage_of_the_analyzed_color_on_the_border;

	pop::VecI32 contours_lengths; 
	pop::VecI32 bl_color_found_on_the_border;
	pop::VecI32 bl_dilated_labels_to_replace;
	pop::VecI32 bl_original_labels_to_replace;
	pop::VecI32 original_to_dilated_label_correspondence_vector;		
	
	
	
	log_steps("Removing IVS regions inside villi that are not in contact with RBCs");
	
	// Initialization
	old_object_color=int_white_pixel_color;
	new_object_color=int_black_pixel_color;
	color_to_search_on_the_border=int_fetal_RBCs_pixel_color;
	
	size_i=working_image.sizeI();
	size_j=working_image.sizeJ();
	bl_regions_contours = pop::Mat2UI8 (pop::Vec2I32(size_i, size_j));
	bl_regions_contours.fill(false);
	bl_old_color_matrix = pop::Mat2UI8 (pop::Vec2I32(size_i, size_j));
	bl_old_color_matrix.fill(false);
	img_3objets = pop::Mat2RGBUI8 (working_image.sizeI(),working_image.sizeJ());

	
	// Marking the regions of the desired old_object_color in a new matrix
	for(i=0;i<size_i;i++)
	{
		for(j=0;j<size_j;j++)
		{
			if(working_image(i,j)==old_object_color)
			{
				bl_old_color_matrix(i,j)=true;
			}
		}
	}	


	// Performing calculations for the original matrix
	labeled_original_image = pop::Processing::clusterToLabel (bl_old_color_matrix, 0);		// Label objects in the original image. 0 stays for 8 neighbors
	region_areas_in_the_original_image = pop::Analysis::areaByLabel(labeled_original_image);		// Calculate region area for each label. Used in the threshold condition. Attention: they are counted from zero
	number_of_original_labels = pop::Analysis::maxValue (labeled_original_image) + 1;

	
	// Obtaining the contours of the objects of interest by dilating the regions to larger shapes and substracting the original regions
	log_steps("Dilation");
	bl_dilated_image = pop::Processing::dilation (bl_old_color_matrix, 1, 2);	// Dilation of the regions of the new matrix with a circular structuring element of radius 1. It is done to obtain the contours of the regions later. Dilation acts on the white regions, so the white regions grow
	log_steps();
	labeled_dilated_image = pop::Processing::clusterToLabel (bl_dilated_image, 0);		// Label objects in the dilated image. 0 stays for 8 neighbors
	//region_areas_in_the_dilated_image = pop::Analysis::areaByLabel(labeled_dilated_image);		// Calculate region area for each label. Used in the threshold condition
	number_of_dilated_labels = pop::Analysis::maxValue(labeled_dilated_image) + 1;		// Calculating the number of regions in the dilated image

	
	// Calculating a label correspondence matrix between the original and the dilated image
	// Here we use the fact that there are <= regions in the dilated image than in the original
	original_to_dilated_label_correspondence_vector = pop::VecI32 (number_of_original_labels, 0);
	for(i=0;i<size_i;i++)
	{
		for(j=0;j<size_j;j++)
		{
			// if (labeled_original_image(i,j)!=0)		// Looking only at the regions of the necessary color
			original_label = labeled_original_image (i,j);
			original_to_dilated_label_correspondence_vector(original_label) = labeled_dilated_image (i,j);
		}
	}


		
	// Substracting the original regions from the dilated matrix to get external regions contours
	// and saving their labels
	labeled_contours_image = pop::Mat2UI32 (size_i, size_j);
	labeled_contours_image.fill(0);
	for(i=0;i<size_i;i++)
	{
		for(j=0;j<size_j;j++)
		{
			bl_regions_contours(i,j) = (bool) bl_dilated_image(i,j) > (bool) bl_old_color_matrix(i,j);
			
			// Copying the labels of these contours from dilated regions labels. Need it to calculate contours lengths
			if((bool) bl_regions_contours(i,j))
				labeled_contours_image(i,j)=labeled_dilated_image(i,j);
		}
	}

	
	// Counting the contours and calculating their lengths
	number_of_contours = pop::Analysis::maxValue(labeled_contours_image) + 1;	// and we will ignore 0 corresponding to the background
	// contours_lengths = pop::Analysis::areaByLabel(labeled_contours_image);		// Attention: they are counted from zero, but the labels are counted from 1
	
	
	// For each contour calculating the percentage of the provided color on it in the original image
	percentage_of_the_analyzed_color_on_the_border = calculate_percentage_of_a_color_on_contours(working_image, labeled_contours_image,
		color_to_search_on_the_border);

	
	// Checking if the analyzed color was found on the contours
	bl_color_found_on_the_border = pop::VecI32 (number_of_contours, false);
	for (i=1;i<number_of_contours;i++)		// Ignoring the label 0
		bl_color_found_on_the_border(i) = percentage_of_the_analyzed_color_on_the_border (i) > 0;


	// Marking the dilated labels that should be replaced. This snippet may be unnecessary. It is effectively just copying a variable
	bl_dilated_labels_to_replace = pop::VecI32(number_of_dilated_labels, false);	// Initializing
	for(i=0;i<size_i;i++)
	{
		for(j=0;j<size_j;j++)
		{
			if(labeled_contours_image(i,j)!=0)	// Only for non-background regions
			{
				label = labeled_contours_image(i,j);
				bl_dilated_labels_to_replace(label) = !bl_color_found_on_the_border(label);	// Replace regions with no analyzed color on the border
				/*if(bl_color_found_on_the_border(label)==false)	
				{
					bl_dilated_labels_to_replace(label) = true;	//	This region will be replaced
				}*/
			}
		}
	}


	
	// Marking the labels of the original image which should be replaced
	bl_original_labels_to_replace = pop::VecI32(number_of_original_labels, false);	// Initializing
	for (label=1; label<number_of_original_labels; label++)		// We don't care about the label 0
	{
		dilated_label = original_to_dilated_label_correspondence_vector(label);
		bl_original_labels_to_replace (label) =	bl_dilated_labels_to_replace (dilated_label);
		// Refining the labels to replace based on the area criterion
		if(region_areas_in_the_original_image(label-1) > Area_threshold_IVS_without_RBCs)
			bl_original_labels_to_replace(label)=false;		
	}
	
	// However, regions with pixels on the image boundary will never be replaced 
	for(i=0;i<size_i;i++)
	{
		for(j=0;j<size_j;j++)
		{
			if(i!=0 && i!=size_i-1 && j!=0 && j!=size_j-1) continue;

			if(labeled_original_image(i,j)>0)		// Not background. Is it necessary? Not anymore, I think
				bl_original_labels_to_replace(labeled_original_image(i,j)) = false;	
		}
	}

	// Perform the actual replacement on the original image
	for(i=0;i<size_i;i++)
	{
		for(j=0;j<size_j;j++)
		{
			label = labeled_original_image (i,j);
			if (label> 0 && bl_original_labels_to_replace(label))
			{
				working_image(i,j)=new_object_color;
			}
		}
	}


	// Saving the obtained results
	colorize_image(img_3objets,working_image);
	saveFileColor(img_3objets, resultDir, str_img_name_withoutExtension + string_output_picutre_number() + 
		"_Filled_holes_in_the_villi.png");
	
	log_steps();
	

	
}


