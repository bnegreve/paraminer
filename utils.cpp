/*
** utils.c
** 
** Made by (Benjamin Negrevergne)
** Email   [firstname].[name]@imag.fr
** 
** Started on  Thu Sep  2 13:15:21 2010 Benjamin Negrevergne
*/
#include <iostream>
#include "utils.hpp"

using namespace std; 

void set_print(const set_t &set){
  if(set.size() == 0){
    cout<<"{emptyset}"<<endl; 
    return ; 
  }
  for(int i = 0 ; i < set.size(); i++){
    element_print(set[i]); 
    cout<<" : "; 
  }
  cout<<endl; 
}

bool set_member(const set_t &set, const element_t &e){
  for(set_t::const_iterator it = set.begin(); it  != set.end(); ++it){
    if(element_equal(*it, e))
      return true; 
  }
  return false; 
}
