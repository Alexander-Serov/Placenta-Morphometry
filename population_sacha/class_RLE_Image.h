#ifndef CLASS_RLE_IMAGE_H
#define CLASS_RLE_IMAGE_H



#include "Population.h" 
#include <string>
#include "class_Run.h"





class RLE_Image
{
public:
	RLE_Image();		// A deafult empty constructor
	RLE_Image(const RLE_Image&);	// Copy constructor
	RLE_Image& operator=(RLE_Image other);		// Copy assignment constructor
	~RLE_Image();		// A default destructor
	RLE_Image(pop::Mat2UI8, int origin_i=0, int origin_j=0);		// Constructor from a Mat2UI8 image with a specified position of the origin within the initial picture
	RLE_Image(int size_i, int size_j);		// Constructor by the size of the output image
	void swap (RLE_Image& image1, RLE_Image& image2);		// Swapping two instances of a class to be able to provide an assignment constructor


	void addNewRun(int iLeft, int iRight, int j);		// Adding a new run to the image
	void addNewRun(Run run);		// Adding a new run to the image
	pop::Mat2UI8 output();		// Output into a (binary) Mat2UI8 image
	RLE_Image dilate (int radius, std::string type = "round");		// Dilate the image with a circular structuring element
	RLE_Image erode (int radius, std::string type = "round");		// Erode the image with a circular structuring element
	void clear();		// Clearing the allocated memory
	void setSize(int size_i, int size_j);		// Setting the size of the initial image
	void sort();		// Sorting the runs by the j coordinate
	void combine_runs ();			// Combine overvlapping or inlaid runs
	void deleteRun (long number);		// Deleting a run with a certain number


private:
	long runs_number;
	long runs_pointer_array_size;
	int size_i;
	int size_j;
	Run **image_runs;

};




// Definition of functions
pop::Mat2UI8 elementwiseIntersect(pop::Mat2UI8 image1, pop::Mat2UI8 image2);		// Calculate the intersection of two binary images

/*
void swapRuns (Run **runs_heap, long swap1, long swap2);
void siftDown (Run **runs_heap, long start, long end);
void heapify(Run **runs_heap, long runs_count);
*/



#endif












