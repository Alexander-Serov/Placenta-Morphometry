


#include "Population.h"
#include "class_RLE_Image.h" 
#include <algorithm>		// std::swap
#include "Log_steps.h"








// The default empty constructor
RLE_Image::RLE_Image()
{
	runs_number=0;
	runs_pointer_array_size=0;
	size_i=0;
	size_j=0;
}



// Constructor from a Mat2UI8 image
RLE_Image::RLE_Image(pop::Mat2UI8 input_image, int origin_i, int origin_j)
{
	// Local variables
	bool last_on;

	int i;
	int j;
	int i_run_start;
	int i_run_end;
	int j_run;
	int size_i;
	int size_j;


	// Initialization
	runs_number=0;
	size_i=input_image.sizeI();
	size_j=input_image.sizeJ();
	this->size_i=size_i;
	this->size_j=size_j;
	runs_pointer_array_size=size_i*size_j;
	image_runs=new Run*[runs_pointer_array_size];


	// Parsing the image
	last_on=false;
	
	for (j=0; j<size_j; j++)
	{
		for (i=0; i<size_i; i++)
		{
			if (last_on==false)		// If the previous pixel was off
			{
				if (input_image(i,j)>0)		// If the current pixel is on, marking the starting position of a new run
				{
					i_run_start=i;
					j_run=j;
					last_on=true;
				}
			}
			else    // If the previous pixel was on
			{
				if (input_image(i,j)==0)		// If the current pixel is off, saving the run
				{
					i_run_end=i-1;
					addNewRun(i_run_start-origin_i, i_run_end-origin_i, j_run-origin_j);
					last_on=false;
				}
			}

		}
		if (last_on) // If the row ended, but the last pixel was on, save a new run
		{
			i_run_end=size_i-1;
			addNewRun(i_run_start-origin_i, i_run_end-origin_i, j_run-origin_j);
			last_on=false;
		}
	}
}







// Copy constructor
RLE_Image::RLE_Image (const RLE_Image& other)
{
	int i;
	
	// Clearing existing data
	//clear();

	// Initialization
	runs_number=0;
	runs_pointer_array_size=other.runs_pointer_array_size;
	size_i=other.size_i;
	size_j=other.size_j;
	image_runs=new Run*[runs_pointer_array_size];


	for (i=0; i<other.runs_number; i++)
	{
		addNewRun(*other.image_runs[i]);
	}
}

// Adding a new run to the image
void RLE_Image::addNewRun(Run run)
{
	image_runs[runs_number] = new Run(run);	// Creating a new run and saving it
	runs_number++;
}


// Adding a new run to the image
void RLE_Image::addNewRun(int iLeft, int iRight, int j)
{
	image_runs[runs_number] = new Run(iLeft, iRight, j);	// Creating a new run and saving it
	runs_number++;
}


// Constructor by the size of the output image
RLE_Image::RLE_Image(int size_i, int size_j)
{
	runs_number=0;
	this->size_i=size_i;
	this->size_j=size_j;
	runs_pointer_array_size=size_i*size_j;
	image_runs=new Run*[runs_pointer_array_size];

}

// A default destructor
RLE_Image::~RLE_Image ()
{
	clear();
}


// Clearing the allocated memory
void RLE_Image::clear()
{
	int i;

	if (runs_number>0)
	{

		// Clearing all allocated Runs memory
		for (i=0; i<runs_number; i++)
		{
			delete image_runs[i];
		}
		runs_number=0;	
	}

	if (runs_pointer_array_size>0)
	{
		// Clearing the memory allocated to the Run pointers array
		delete image_runs;
		runs_pointer_array_size=0;
	}
	
	
}


// Swapping two instances of a class to be able to provide an assignment constructor
void RLE_Image::swap(RLE_Image& image1, RLE_Image& image2)
{
	std::swap(image1.runs_number, image2.runs_number);
	std::swap(image1.runs_pointer_array_size, image2.runs_pointer_array_size);
	std::swap(image1.size_i, image2.size_i);
	std::swap(image1.size_j, image2.size_j);
	std::swap(image1.image_runs, image2.image_runs);

}



// Copy assignment constructor
RLE_Image& RLE_Image::operator=(RLE_Image other)
{
	swap(*this, other);

	return *this;
}





// Setting the maximum possible number of runs
void RLE_Image::setSize(int size_i, int size_j)
{
	// Clearing previous data
	clear();

	// Initializing the runs pointers array
	this->size_i=size_i;
	this->size_j=size_j;
	runs_pointer_array_size=size_i*size_j;
	image_runs=new Run*[runs_pointer_array_size];
}



// Output into a (binary) Mat2UI8 image
pop::Mat2UI8 RLE_Image::output()
{
	// Local variables
	int i;
	//int j;
	int k;

	// Initialization
	pop::Mat2UI8 output_image(size_i, size_j);
	output_image.fill(0);


	for (k=0; k<runs_number; k++)
	{
		for (i=image_runs[k]->iLeft; i<=image_runs[k]->iRight; i++)
			output_image(i, image_runs[k]->j)=255;
	}

	
	


	return output_image;
}








// Dilate the image with a circular structuring element
// The algorithm is taken from the article 
// Kim, W.J., Kim, S.D., Kim, K., 2005. Fast algorithms for binary dilation and erosion using run-length encoding. ETRI J. 27, 814–817. doi:10.4218/etrij.05.0205.0013
RLE_Image RLE_Image::dilate (int radius, std::string type)
{
	// Local variables
	// pop::Mat2UI8 SE_image;
	long i;
	long j;
	int iLeft;
	int iRight;
	int jSave;
	long new_runs_number;
	int *number_of_runs_with_certain_j;
	int size_SE;
	//RLE_Image dilated_image;
	
	

	
	log_steps("Dilation");
	if (type=="round")		// Dilation with a round structuring element
	{
		// Preparing the structuring element
		size_SE=2*radius+1;
		pop::Mat2UI8 SE_image(size_SE, size_SE);
		for (i=0; i<size_SE; i++)
		{
			for (j=0; j<size_SE; j++)
			{
				SE_image(i,j) = pow((radius-i), 2) + pow((radius-j), 2) <= pow(radius,2);
			}
		}
		RLE_Image structuring_element (SE_image, radius, radius);


		// Initializing the dilated image
		RLE_Image dilated_image (size_i, size_j);
		new_runs_number= runs_number*structuring_element.runs_number;
		//dilated_image.setSize(this->size_i, this->size_j);		// Setting the size of the final image

		// Calculating the primary dilation
		for (i=0; i<runs_number; i++)
		{
			for (j=0; j<structuring_element.runs_number; j++)
			{
				iLeft=image_runs[i]->iLeft + structuring_element.image_runs[j]->iLeft;
				iLeft = iLeft>0 ? iLeft : 0;	// Limitting the zone of interest to the original image size
				iRight=image_runs[i]->iRight + structuring_element.image_runs[j]->iRight;
				iRight = iRight < size_i-1 ? iRight : size_i-1;
				jSave=image_runs[i]->j + structuring_element.image_runs[j]->j;
				//jSave = jSave > 0 ? jSave : 0;
				//jSave = jSave < size_j-1 ? jSave : size_j-1;
				
				if (jSave >=0 && jSave<size_j)		// Save only those runs which are inside the original image
					dilated_image.addNewRun(iLeft, iRight, jSave);
			}
		}


		// Combine where possible the runs with the same j
		dilated_image.combine_runs();


		log_steps();		// End: "Dilation"
		// The dilation is completed
		return dilated_image;

	}
	else
	{
		// Non-round SE dilation is not supported in this version.
		// Returning the original image
		log_steps();		// End: "Dilation"
		return *this;
	}
}





/*
// Sorting the runs by the j coordinate by the heapsort method
// See https://en.wikipedia.org/?title=Heapsort
// Input: an unordered array a of length count
void RLE_Image::sort()
{
	// Local variables
	long end;



	// Building a heap in the input array, so that largest value is at the root
    heapify(image_runs, runs_number);

	// The following loop maintains the invariants that a[0:end] is a heap and every element
	// beyond end is greater than everything before it (so a[end:count] is in sorted order).
	// Starting with the last element
    end = runs_number - 1;
	while (end > 0)
	{
		// Element 0 is the root and the largest value. The swap function moves it in front of the sorted elements.
		swap(image_runs, end, 0);
		end--;		// The heap size is reduced by one
		// The swap ruined the heap property, so restore it
		siftDown(image_runs, 0, end);
	}
	// The runs array is now sorted by the j coordinate

}




// Swap two runs in a heap (swap their addresses in the runs array)
void swapRuns (Run **runs_heap, long swap1, long swap2)
{
	Run* temp_run_pointer;

	temp_run_pointer = runs_heap[swap1];
	runs_heap[swap1] = runs_heap[swap2];
	runs_heap[swap2] = temp_run_pointer;
}




// Repair the heap whose root element is at index 'start', assuming the heaps rooted at 0 and its children are valid
// The heap ends at 'end' 
void siftDown (Run **runs_heap, long start, long end)
{
	// Local variables
	long current_child;
	long current_root;
	long swap_with;

	
	// Initialization
	current_root=start;
	while (current_root * 2 + 1 <= end)		// While the root has at least one child
	{
        current_child = current_root * 2 + 1;		// Left child
        swap_with = current_root;		// Initializes the swap_with with the parent index

		// If there is a left child, and the child is greater, mark for swap
		if (runs_heap[swap_with]->j < runs_heap[current_child]->j)
			swap_with=current_child;
            
        // If there is a right child and the child is greater
        if (current_child+1 <= end && 
			runs_heap[swap_with]->j < runs_heap[current_child+1]->j)
			
			swap_with = current_child + 1;

		if (swap_with == current_root)
            // The root holds the largest element. Since we assume the heaps rooted at the
            // children are valid, this means that we are done.
            return;
		else    // We need to swap elements
		{
			swap (runs_heap, current_root, swap_with);
			current_root=swap_with;
			// Repeat to continue sifting down the child branch now
		}

	}
		
}




// Put elements of 'a' in the heap order, from bottom upwards
void heapify(Run **runs_heap, long runs_count)
{
    long start;
	
	
	// Start is assigned the index in 'a' of the last parent node
    // The last element in a 0-based array is at index count-1; find the parent of that element
    start = floor ((runs_count - 1) / 2.0);
    
    while (start >= 0)
	{
        // Sift down the node at the index 'start' to the proper place such that all nodes below
        // the start index are in heap order
        siftDown(runs_heap, start, runs_count - 1);
        // Go to the next parent node
        start--;
	}
    // After sifting down the root all nodes/elements are in heap order

}
*/


// Combine overvlapping or inlaid runs with the coordinate j
void RLE_Image::combine_runs()
{
	/*
	// Local variables
	bool last_on;
	bool *temp_row;
	int i;
	int j;
	int k;
	int i_run_start;
	int i_run_end;
	*/


	// Converting runs to an image, which eliminates redundant runs
	pop::Mat2UI8 image=output();

	// Converting the image back to runs
	RLE_Image temp_RLE_image (image);

	// Replacing the old class instance with the new one
	swap(*this, temp_RLE_image);


	
	
	/*
	
	// Initialize an empty image of the necessary size
	
	
	// Initialize with zeros a pixel row to which all runs will be projected 
	//temp_row = new bool [size_i];		

	// Parsing all the runs looking for j0 and plotting them on the row
	for (k=runs_number-1; k>=0; k--)
	{
		if (image_runs[k]->j==j0)
		{
			// Projecting this run
			for (i=image_runs[k]->iLeft; i<=image_runs[k]->iRight; i++)
				temp_row[i]=1;
			
			// Deleting the run with j0
			deleteRun(k);
		}
	}

	// Saving the obtained row as runs
	last_on=false;
	for (i=0; i<size_i; i++)
	{
		if (last_on==false)		// If the previous pixel was off
		{
			if (temp_row[i])		// If the current pixel is on, marking the starting position of a new run
			{
				i_run_start=i;
				last_on=true;
			}
		}
		else    // If the previous pixel was on
		{
			if (temp_row[i]==false)		// If the current pixel is off, saving the run
			{
				i_run_end=i-1;
				addNewRun(i_run_start, i_run_end, j0);
				last_on=false;
			}
		}

	}
	if (last_on) // If the row ended, but the last pixel was on, save a new run
	{
		i_run_end=size_i-1;
		addNewRun(i_run_start, i_run_end, j0);
		last_on=false;
	}

	*/
	
	
}





// Deleting a run with a certain number
void RLE_Image::deleteRun (long number)
{
	int k;
	
	// Deleting the memory allocated for the run
	delete image_runs[number];
	
	// Shifting all the elements
	for (k=number; k<runs_number-1; k++)
		image_runs[k]=image_runs[k+1];
	
	// Reducing the total number of saved runs	
	runs_number--;

}




// Dilate the image with a circular structuring element
// The algorithm is taken from the article 
// Kim, W.J., Kim, S.D., Kim, K., 2005. Fast algorithms for binary dilation and erosion using run-length encoding. ETRI J. 27, 814–817. doi:10.4218/etrij.05.0205.0013
RLE_Image RLE_Image::erode (int radius, std::string type)
{
	// Local variables
	// pop::Mat2UI8 SE_image;
	long i;
	long j;
	int iLeft;
	int iRight;
	int jSave;
	long new_runs_number;
	int *number_of_runs_with_certain_j;
	int size_SE;
	//RLE_Image dilated_image;
	
	

	log_steps("Erosion");
	
	if (type=="round")		// Dilation with a round structuring element
	{
		// Preparing the structuring element
		size_SE=2*radius+1;
		pop::Mat2UI8 SE_image(size_SE, size_SE);
		for (i=0; i<size_SE; i++)
		{
			for (j=0; j<size_SE; j++)
			{
				SE_image(i,j) = pow((radius-i), 2) + pow((radius-j), 2) <= pow(radius,2);
			}
		}
		RLE_Image structuring_element (SE_image, radius, radius);


		// Initializing the eroded image
		pop::Mat2UI8 eroded_image(size_i, size_j);
		eroded_image.fill(1);
		
		
		// Calculating primary runs and adding the to the eroded_image
		for (i=0; i<structuring_element.runs_number; i++)
		{
			// Primary runs for a fixed run of the structuring element
			RLE_Image primary_runs(size_i, size_j);
			for (j=0; j<runs_number; j++)
			{
				iLeft=image_runs[j]->iLeft - structuring_element.image_runs[i]->iLeft;
				iLeft = iLeft>0 ? iLeft : 0;	// Limitting the zone of interest to the original image size
				iRight=image_runs[j]->iRight - structuring_element.image_runs[i]->iRight;
				iRight = iRight < size_i-1 ? iRight : size_i-1;
				jSave=image_runs[j]->j - structuring_element.image_runs[i]->j;

				bool size_condition = structuring_element.image_runs[i]->iRight - structuring_element.image_runs[i]->iLeft <=
					image_runs[j]->iRight - image_runs[j]->iLeft;

				// If the run of the structuring element is smaller than the run of the eroded image, saving a new run
				// Saving only those runs which lie inside the original image
				if (size_condition && jSave >=0 && jSave<size_j)
					primary_runs.addNewRun(iLeft, iRight, jSave);
			}
			
			// These runs are now intersected with the eroded_image
			eroded_image = elementwiseIntersect(eroded_image, primary_runs.output());
		}

		// The eroded image should be fully calculated by now
		// Preparing the RLE version
		RLE_Image RLE_eroded_image (eroded_image);

		
		log_steps();		// End: "Erosion"
		// The erosion is completed
		return RLE_eroded_image;

	}
	else
	{
		// Non-round SE dilation is not supported in this version.
		// Returning the original image
		log_steps();		// End: "Erosion"
		return *this;
	}
}






// Calculate the intersection of two binary images
pop::Mat2UI8 elementwiseIntersect(pop::Mat2UI8 image1, pop::Mat2UI8 image2)
{
	int size_i (image1.sizeI());
	int size_j (image1.sizeJ());
	pop::Mat2UI8 output_image(size_i, size_j);
	
	for (int i=0; i<size_i; i++)
		for (int j=0; j<size_j; j++)
			output_image(i,j) = image1(i,j) && image2(i,j);

	return output_image;
}