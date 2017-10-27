


#include "Population.h"
#include "Global_variables.h"


// Platform-specific commands
#ifdef _WIN32
	#include"population_sacha\Log_steps.h"
#elif __linux__ || defined __APPLE__ || __APPLE__
	#include"population_sacha/Log_steps.h"
#endif


using namespace pop;




// Declarations of functions
VecF64 matrix_histogram (const Mat2F64& input_matrix, double& min_value, double& max_value, double& bin_width);




// ========== Function: find_thresholds_for_a_matrix ==========
VecF64 find_thresholds_for_a_matrix (const Mat2F64 &input_matrix, Mat2F64& absolute_histogram)
{
	// Local variables
	int maxima_counter=0;
	int half_local_interval;
	int i;
	int j;
	int left_boundary;
	int right_boundary;
	int number_of_maxima_found;
	
	double local_maximal_value;
	double max_value;
	double min_value;
	double tmpFloat;
	double bin_width; 
		
	Mat2F64 histogram_larger;

	std::stringstream str_stream;
	
	VecI32 positions_of_local_maxima;
	
	VecF64 histogram;
	VecF64 segmentation_thresholds_locations;
	VecF64 selected_maxima_locations;
	VecF64 selected_maxima_values;
	VecF64 values_of_local_maxima;


	// Initialization
	number_of_maxima_found=0;
	half_local_interval=floor(((double)number_of_neighboring_bins_to_consider-1)/2); // How many bins to consider on each side of the current one, when looking for a local maximum
	positions_of_local_maxima=VecI32(histogram_bins_number, 0);		// Initialize the vector with zeros
	values_of_local_maxima=VecF64(histogram_bins_number, 0.);		// Initialize the vector with zeros
	selected_maxima_locations = VecF64 (3, -1);		// Initialize the vector with -1
	selected_maxima_values = VecF64 (3, -1);		// Initialize the vector with -1
	segmentation_thresholds_locations = VecF64 (2, -1);		// Initialize the vector with -1

	
	// Calculating a relative normalized histogram
	histogram = matrix_histogram(input_matrix, min_value, max_value, bin_width);		// A histogram is just a vector


	// Locating local maxima in the relative normalized histogram
	log_steps("Locating local maxima of the histogram");
	for (int i=0; i<histogram_bins_number; i++)
	{
		// Determining the boundaries of the neighboring region
		left_boundary = (i-half_local_interval)>=0 ? (i-half_local_interval) : 0;
		right_boundary = (i+half_local_interval)<histogram_bins_number ? (i+half_local_interval) : (histogram_bins_number-1);
		
		local_maximal_value=histogram(left_boundary);
		for (j=left_boundary+1; j<=right_boundary; j++)		// Analyzing the values of the neighbors
		{
			local_maximal_value=local_maximal_value < histogram(j) ? histogram(j) : local_maximal_value;
		}

		if (local_maximal_value==histogram(i)) 
		{
			positions_of_local_maxima(number_of_maxima_found)=i;	// Saving the position of the local maxium
			values_of_local_maxima(number_of_maxima_found++) = local_maximal_value;	// Saving the value of the local maxium and increasing the number of found maxima
		}
	}


	// Analyzing the locations of the maxima. Choosing the highest maximum in each subregion
	for (int i=0; i<number_of_maxima_found; i++)
	{
		if (positions_of_local_maxima (i) <= histogram_regions_borders[1] * histogram_bins_number
			&& selected_maxima_values(0) <= values_of_local_maxima (i))	// looking at the maxima of the first region
		{
			selected_maxima_locations(0) = positions_of_local_maxima (i) +1/2;		// Taking the highest maximum of the interval. 1/2 is used to get the middle of the bin
			selected_maxima_values(0) = values_of_local_maxima (i);
		}

		if (positions_of_local_maxima (i) > histogram_regions_borders[1] * histogram_bins_number
			&& positions_of_local_maxima (i) <= histogram_regions_borders[2] * histogram_bins_number
			&& selected_maxima_values(1) <= values_of_local_maxima (i))	// looking at the maxima of the second region
		{
			selected_maxima_locations(1) += (positions_of_local_maxima (i) + 1/2);		// Taking the highest maximum of the interval
			selected_maxima_values(1) = values_of_local_maxima (i);
		}

		if (positions_of_local_maxima (i) > histogram_regions_borders[2] * histogram_bins_number
			&& selected_maxima_values(2) <= values_of_local_maxima (i))	// looking at the maxima of the third region
		{
			selected_maxima_locations(2) = positions_of_local_maxima (i) + 1/2;		// Taking the highest maximum of the interval
			selected_maxima_values(2) = values_of_local_maxima (i);
		}
	}
	//if (maxima_counter>0) selected_maxima_locations(1)/=maxima_counter;		// Finishing the calculation of the mean value for the second interval


	// Assigning default values in case no maxima were found in a certain region
	if (selected_maxima_locations(0) < 0)	// Missing first region maximum
	{
		selected_maxima_locations(0) = default_histogram_maxima_locations [0] * histogram_bins_number;
	}

	if (selected_maxima_locations(1) < 0)	// Missing second region maximum
	{
		selected_maxima_locations(1) = default_histogram_maxima_locations [1] * histogram_bins_number;
	}

	if (selected_maxima_locations(2) < 0)	// Missing first region maximum
	{
		selected_maxima_locations(2) = default_histogram_maxima_locations [2] * histogram_bins_number;
	}


	// Converting relative to absolute values
	selected_maxima_locations = min_value + selected_maxima_locations * bin_width;

	//segmentation_thresholds_locations = min_value + segmentation_thresholds_locations * bin_width;

	// Calculating segmentation thresholds as mean values between the maxima
	segmentation_thresholds_locations(0) = (selected_maxima_locations(0) + selected_maxima_locations(1))/2;
	segmentation_thresholds_locations(1) = (selected_maxima_locations(1) + selected_maxima_locations(2))/2;


	// Converting relative to absolute values
	//segmentation_thresholds_locations = min_value + segmentation_thresholds_locations * bin_width;

	// Preparing a histogram in absolute values for output
	for (int i=0; i<histogram_bins_number; i++)
	{
		absolute_histogram (0,i) = min_value + i * bin_width;		// The first row contains the absolute positions of the bins borders
		absolute_histogram (1,i) = histogram(i);		// The second row contains the bin values
	}
	absolute_histogram (0, histogram_bins_number) = min_value + histogram_bins_number * bin_width;
	absolute_histogram (1, histogram_bins_number) = 0;

	log_steps();

	return segmentation_thresholds_locations;
}






// ========== Function: histogram ==========
VecF64 matrix_histogram (const Mat2F64& input_matrix, double& min_value, double& max_value, double& bin_width)
{
	// Declaration
	double current_value;
	
	int current_bin_number;
	int i;
	// int indice;
	int j;
	
	VecF64 histogram;

	
	log_steps("Calculating a histogram");
	

	// Initialization
	histogram = VecF64(histogram_bins_number, 0);		// Initializing the histogram with 0s
	max_value=pop::Analysis::maxValue(input_matrix);
	min_value=pop::Analysis::minValue(input_matrix);
	bin_width=(max_value-min_value)/histogram_bins_number*1.01;		// The bins are slightly enlarged to fit the maximal value in the bins


	// Parsing the matrix
	for(i=0; i<input_matrix.sizeI(); i++)
	{
		// str_stream << "Current bin number and image coordinates: " << current_value << " (" << i << "," << j << ")";
		// str_stream << "Current i: " << i;
		// log_steps(str_stream.str(), "general");
		for(j=0; j<input_matrix.sizeJ(); j++)
		{
			current_value=(input_matrix(i,j)-min_value);	// Shifting the minimal value to zero
			current_value/=bin_width;						// Dividing by the bin width to get an approximate bin number
			current_value = floor(current_value);		// Rounding the bin number down getting a number from 0 to histogram_bins_number-1
			histogram((int)current_value)+=1;			// Counting the necessary bin
		}
	}

	
	log_steps();
	return histogram;
}

