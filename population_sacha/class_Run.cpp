


#include "class_Run.h"
#include <algorithm>





Run::Run()
	: iLeft(0),
	iRight(0),
	j(0)
{
	// Run::Run(0,0,0);
}


Run::Run (int iLeft_in, int iRight_in, int j_in) 
	: iLeft(iLeft_in),
	iRight(iRight_in),
	j(j_in)
{
	
}


// Copy constructor
Run::Run (const Run& other)
	: iLeft(other.iLeft),
	iRight(other.iRight),
	j(other.j)
{
	
}



// Copy assignment constructor
Run& Run::operator=(Run other)
{
	swap (*this, other);

	return *this;
}



// Swap function
void Run::swap (Run& run1, Run& run2)
{
	std::swap(run1.iLeft, run2.iLeft);
	std::swap(run1.iRight, run2.iRight);
	std::swap(run1.j, run2.j);

}



