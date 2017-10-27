#pragma once
#ifndef SHIFT_VECTOR_BY_ONE
#define SHIFT_VECTOR_BY_ONE


#include "Population.h"



// This function shifts all elements to the left and insersts a zero as a first element of the vector
// The whole function needs to be included since it's a template
template <typename TYPE>
pop::Vec<TYPE> shift_vector_by_one (const pop::Vec<TYPE>& input_vector)
{
	int i;
	int old_size;
	int new_size;
	pop::Vec<TYPE> new_vector;

	// Calculating sizes
	old_size=input_vector.size();
	new_size=old_size+1;

	// Initializing a new vector
	new_vector = pop::Vec<TYPE> (new_size);

	// Filling the vector with the values of the old vector and 0 in the first position
	new_vector (0)=0;
	for (i=1; i<new_size; i++)
		new_vector(i)=input_vector(i-1);

	return new_vector;
}

#endif