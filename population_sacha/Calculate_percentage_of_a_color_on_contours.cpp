


#include "Population.h"
#include "Shift_vector_by_one.h"



// For each contour calculating the percentage of the provided color on it in the original image
pop::VecF64 calculate_percentage_of_a_color_on_contours (pop::Mat2UI8 colored_pixels_image, pop::Mat2UI32 labeled_contours_image,
														int color_to_search_on_the_border)

{
	// Declaration
	int i;
	int j;
	int label; 
	int number_of_contours;
	int size_i;
	int size_j;
	pop::VecF64 percentage_of_the_analyzed_color_on_the_border;
	pop::VecI32 contours_lengths; 


	// Initialization
	size_i=colored_pixels_image.sizeI();
	size_j=colored_pixels_image.sizeJ();
	number_of_contours=pop::Analysis::maxValue (labeled_contours_image) + 1;
	percentage_of_the_analyzed_color_on_the_border = pop::VecF64 (number_of_contours, 0);
	
	
	
	// Calculating contour lenghts
	contours_lengths= pop::Analysis::areaByLabel (labeled_contours_image);
	contours_lengths = shift_vector_by_one(contours_lengths);	// Letting the 0-th length correspond to the label 0
	

	// Calculating the percentage of the required color
	for(i=0;i<size_i;i++)
	{
		for(j=0;j<size_j;j++)
		{
			if(labeled_contours_image(i,j)>0)	// If not background
			{
				label = labeled_contours_image(i,j);		// Identifying the current label
				if (colored_pixels_image(i,j) == color_to_search_on_the_border)
					percentage_of_the_analyzed_color_on_the_border(label) += (double) 1/contours_lengths(label);
			}
		}
	}

	return percentage_of_the_analyzed_color_on_the_border;

}