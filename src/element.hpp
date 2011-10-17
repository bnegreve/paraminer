/*
** element.h
** 
** Made by Benjamin Negrevergne
** Email   [firstname].[name]@imag.fr
** 
** Started on  Thu Sep  2 10:20:03 2010 Benjamin Negrevergne
*/

#ifndef   	_ELEMENT_H_
#define   	_ELEMENT_H_

#include <vector>
#include <cassert>

typedef int element_t; 
extern element_t ELEMENT_RANGE_START;
extern element_t ELEMENT_RANGE_END; 

const element_t element_null = ELEMENT_RANGE_START - 1; //must not be equal to any valid element 

/** 
 * Deprecated, elements are assumed to be equals iff they have the same representation in memory */
bool element_equal(const element_t a, const element_t b); 
bool element_compare_ge(const element_t a, const element_t b); 

element_t element_first(); 
element_t element_lbound(); 
element_t element_ubound();
element_t element_next(const element_t current); 

void element_print(const element_t element); 


#endif 	    /* !_ELEMENT_H_ */
