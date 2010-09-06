/*
** clogen.cpp
** 
** Made by (Benjamin Negrevergne)
** Email   [firstname].[name]@imag.fr
** 
** Started on  Thu Sep  2 10:21:29 2010 Benjamin Negrevergne
*/
#include <iostream>
#include <algorithm>
#include "utils.hpp"
#include "element.hpp"
#include "pattern.hpp"
int nb_patterns = 0; 

using std::cout; 
using std::endl;


/* Return the maximum element e so set \ {e} is in F */
/* TODO rewrite this extreamly inefficient function a smarter way */
element_t get_tail(const set_t &set){
  
  element_t max = element_null; 
  
  for(int i = 0; i < set.size(); i++){
    /* Generate a set without element i */
    set_t s; 
    for(int j = 0; j < set.size(); j++){
      if(i != j)
	s.push_back(set[j]);
    }
    if(membership_oracle(s)){
      if(max == element_null || element_compare_ge(set[i], max))
	max=set[i];
    }
  }
  return max; 
}

 struct element_cmp {
   /* return true if e1 < e2 */
   bool operator()(const element_t &e1, const element_t &e2) const{
     return !element_compare_ge(e1, e2);
   }
 };

std::pair<set_t, element_t> get_first_parent(const set_t &set){
  set_t z(set); 
  while(z.size() != 0){
    element_t e = get_tail(z); 
    set_t x;
    for(int i = 0 ; i < z.size(); i++)
      if(z[i] != e)
	x.push_back(z[i]); 

    /*If the closure of z \ {e} is not z itself it's his parent.*/
    if( clo(x) != z ){
      set_t xx(clo(x)); 
      std::sort(xx.begin(), xx.end(), element_cmp()); 
      return make_pair(canonical_form(xx, &e), e);
    }
    z = x; 
  }
  assert(false); 
}


void expand(set_t c){
  set_print(c); nb_patterns++; 

  for(element_t  current = element_first(); current != element_null; current = element_next(current)){

    if(set_member(c, current))
      continue; 
    set_t d = c; 

    d.push_back(current);
      d = clo(d); 
      if(membership_oracle(d) != 1)
	continue; 
    
      /* retreive real parent of d and compare with the set of the current branch*/
      std::pair<set_t, element_t> first_parent = get_first_parent(d); 
      
      if(first_parent.first == c && first_parent.second == current){
	expand(d);
      }
  }
}
