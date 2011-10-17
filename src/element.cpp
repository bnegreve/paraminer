// element.cpp
// Made by Benjamin Negrevergne
// Started on  Thu Sep  2 10:43:59 2010
#include "element.hpp"
#include <iostream>

using std::cout;
using std::endl; 

element_t ELEMENT_RANGE_START = 0; 

bool element_equal(const element_t a, const element_t b){
  assert(false); 
  return a == b; 
}

bool element_compare_ge(const element_t a, const element_t b){
  assert(a < ELEMENT_RANGE_END && a >= ELEMENT_RANGE_START); 
  assert(b < ELEMENT_RANGE_END && b >= ELEMENT_RANGE_START); 
  return a>=b;
}


element_t element_first(){
  return ELEMENT_RANGE_START; 
}


element_t element_lbound(){
  return ELEMENT_RANGE_START - 1;
}

element_t element_ubound(){
  return ELEMENT_RANGE_END;
}

element_t element_next(const  element_t current){
  assert(current < ELEMENT_RANGE_END); 
  assert(current >= ELEMENT_RANGE_START); 
  if(current < ELEMENT_RANGE_END - 1)
    return current+1; 
  else 
    return element_null; 
}

// void element_print(const element_t element){
//   cout<<element; 
// }

