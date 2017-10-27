#ifndef CLASS_RUN_H
#define CLASS_RUN_H







// One run of a run-length encoded image
class Run
{
public:
	int iLeft;
	int iRight;
	int j;

	// Constructors
	Run();
	Run (int, int, int);
	Run (const Run& other);		// Copy constructor
	Run& operator=(Run other);		// Copy assignment constructor
	void swap (Run& run1, Run& run2);		// Swap function

private:
	
};




#endif