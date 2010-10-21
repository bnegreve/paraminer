/*
** utils.h
** 
** Made by Benjamin Negrevergne
** Email   [firstname].[name]@imag.fr
** 
** Started on  Thu Sep  2 13:14:01 2010 Benjamin Negrevergne
*/

#ifndef   	_UTILS_H_
#define   	_UTILS_H_

#include "pattern.hpp"

void set_print(const set_t &set);

bool set_member(const set_t &set, const element_t &e); 

int set_member_index(const set_t &set, const element_t &e); 

template <typename T>
bool is_sorted(const std::vector<T> &v){
  if(v.size() < 2)
    return true; 

  for(typename std::vector<T>::const_iterator it = ++v.begin(); it != v.end(); ++it){
    if(*it < *(it-1))
       return false; 
  }
  return true; 
}

#endif 	    /* !_UTILS_H_ */
