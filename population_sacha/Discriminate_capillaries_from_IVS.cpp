


#include"Population.h"
#include<string>
#include"Global_variables.h"
#include "Small_functions.h"
#include "Log_steps.h"
#include "Statistics_placenta.h"
#include "Calculate_percentage_of_a_color_on_contours.h"
#include "Shift_vector_by_one.h"






void discriminate_capillaires_from_IVS(pop::Mat2UI8& working_image,
									   // pop::Mat2RGBUI8& img_3objets,
									   // pop::Mat2UI8& img_3objets_gray,
									   // pop::Mat2UI8& img_final,
									   // pop::Mat2UI8& img_villi_and_fetal_RBCs,
									   //const std::string img_name_withoutExtension,
									   const float percentage_threshold_of_IVS_on_the_internal_IVS_boundary_for_a_capillary,
									   const pop::Mat2RGBUI8& initial_3_regions_segmentation
									   //const std::string resultDir
									   )
{
	log_steps("Discriminating betweent RBCs inside and outside the villi");
	
	// Initialization
	int size_i = working_image.sizeI();
	int size_j = working_image.sizeJ();


	// Labelling the input image containing only villi and IVS
	pop::Mat2UI32 img_IVS_regions_labeled (pop::Processing::clusterToLabel(working_image,1));
	int number_of_IVS_regions = pop::Analysis::maxValue(img_IVS_regions_labeled)+1;


	// Eroding villi to get their internal contours by substracting the eroded objects from the original objects
	// The contours will be then used to calculate the persentage of RBCs on the internal boundary
	pop::Mat2UI32 img_internal_contours_labeled(size_i, size_j);
	img_internal_contours_labeled.fill(0);

	pop::Mat2UI8 img_erosion=pop::Processing::erosion(working_image,1,2);	// Eroding IVS with a circular structuring element (2) of radius 1 to go just 1 pixel inside

	pop::Mat2UI8 bl_img_internal_IVS_contours (size_i,size_j);
	bl_img_internal_IVS_contours.fill(false);
	for(int i=0;i<size_i;i++)
	{
		for(int j=0;j<size_j;j++)
		{
			bl_img_internal_IVS_contours(i,j)=working_image(i,j) > img_erosion(i,j);

			// Labeling different contours from the original regions, so some of the separated contours will have the same labels,
			// because they correspond to one initial contour
			if (bl_img_internal_IVS_contours(i,j))
				img_internal_contours_labeled(i,j)=img_IVS_regions_labeled(i,j);
		}
	}
	// The labels and the number of the contours are the same as of the original regions

	
	// Calculating the percentage of RBCs on each contour
	pop::VecF64 percentage_of_RBCs_on_each_contour = calculate_percentage_of_a_color_on_contours(initial_3_regions_segmentation, 
		img_internal_contours_labeled, int_fetal_RBCs_pixel_color);
	

	// Marking labels to replace by the maternal IVS according to a threshold in the percentage of RBCs in each contours
	pop::VecI32 bl_original_IVS_region_is_a_capillary(number_of_IVS_regions, false);
	for (int region_label=1; region_label<number_of_IVS_regions; region_label++)		// Ignoring the label 0
		bl_original_IVS_region_is_a_capillary(region_label) = percentage_of_RBCs_on_each_contour (region_label) >= 
			percentage_threshold_of_IVS_on_the_internal_IVS_boundary_for_a_capillary;


	// Perform the actual replacement on the original image.
	// The original (working) image has villi in black and IVS and RBCs both in white
	for(int i=0;i<size_i;i++)
	{
		for(int j=0;j<size_j;j++)
		{
			int region_label=img_IVS_regions_labeled(i,j);
			if (region_label>0 &&
				bl_original_IVS_region_is_a_capillary(region_label))
				working_image(i,j)=int_fetal_RBCs_pixel_color;
		}
	}


	// Saving the result
	saveFileColor(working_image, resultDir, str_img_name_withoutExtension+string_output_picutre_number()+"_Fetal_capillaries_identified.png");

	log_steps();		// End: "Discriminating betweent RBCs inside and outside the villi");
	
}
