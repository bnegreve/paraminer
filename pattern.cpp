// pattern.cpp
// Made by Benjamin Negrevergne
// Started on  Thu Sep  2 11:42:23 2010
#include "pattern.hpp"

int membership_oracle(const set_t &set){
  return 1; 
}



set_t clo(const set_t &set){
  return set;
}

set_t canonical_form(set_t set){
  return set; 
}

element_t canonical_transform_element(const set_t &set, element_t &element){
  return element; 
}

set_t canonical_form(set_t set, element_t *element){
  return set; 
}
