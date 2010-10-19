// pattern.cpp
// Made by Benjamin Negrevergne
// Started on  Thu Sep  2 11:42:23 2010
#include <iostream>
#include <cstdlib>
#include <algorithm>

#include "itemset_specific.hpp" 
#include "clogen.hpp"
#include "utils.hpp"
#include "pattern.hpp"

using namespace std; 

extern TransactionTable tt; 
extern TransactionTable ot; 

extern int threshold; 

int get_set_presence_1d(const Transaction &t, const set_t &set){
  return is_included_1d(t, set); 
}

int is_included_1d(const Transaction &t, const set_t &set){
  assert(is_sorted(t)); 
  if(set.size()==0)
    return 1; 
  if(t.size() == 0)
    return 0; 
    
  //  assert(is_sorted(t));
  // if(!is_sorted(set))
  //   set_print(set); 
  assert(is_sorted(set)); 
  Transaction::const_iterator it = t.begin(); 
  set_t::const_iterator set_it = set.begin(); 

  do{
    if(*set_it == *it)
      if(++set_it == set.end())
	return 1;
    ++it; 
  }while(it != t.end());
 
  return 0; 
}

int count_inclusion_1d();

int count_inclusion_2d(const TransactionTable &tt, const set_t &set){
  int count  = 0; 
  for(TransactionTable::const_iterator it = tt.begin(); it != tt.end(); ++it){
    if(is_included_1d(*it, set))
      count++; 
  }
  return count; 
}

void get_occurences_2d(const TransactionTable &tt, const set_t &set, Occurence *oc){
  int i = 0;
  for(TransactionTable::const_iterator it = tt.begin(); it != tt.end(); ++it, ++i){
    if(get_set_presence_1d(*it, set))
      oc->push_back(i); 
  }
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
