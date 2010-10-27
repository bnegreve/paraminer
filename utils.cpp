/*
** utils.c
** 
** Made by (Benjamin Negrevergne)
** Email   [firstname].[name]@imag.fr
** 
** Started on  Thu Sep  2 13:15:21 2010 Benjamin Negrevergne
*/

#include <iostream>
#include <algorithm>
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

int set_member_index(const set_t &set, const element_t &e){
  int i = 0; 
  for(set_t::const_iterator it = set.begin(); it  != set.end(); ++it,++i){
    if(element_equal(*it, e))
      return i; 
  }
  return -1; 
}

int set_is_frequent(const set_t &set, const TransactionTable &tt, int min_sup){
  int freq = count_inclusion_2d(tt, set);
  if(freq >= min_sup)
    return 1; 
  return 0; 
}

int set_is_frequent_in_occurences(const set_t &set, const TransactionTable &tt,
				  const Transaction &occurences, int min_sup){
  return count_inclusion_2d(tt, occurences, set) >= min_sup;
}

// set_t support_based_closure(TransactionTable &tt, const set_t &s){

//   Occurence oc;
//   set_t clo; 
//   set_t set(s); 
//   std::sort(set.begin(), set.end()); 
//   get_occurences_2d(tt, set, &oc); 

//   set_t dummy_set; 
//   for(int i = 0 ; i < oc.size(); i++)
//     dummy_set.push_back(oc[i]); 

//   for(int i = 0; i < ot.size(); i++){ 
//     if(get_set_presence_1d((ot)[i], dummy_set) == 1)
//       clo.push_back(i); //
//   }
//   return clo; 
// }

set_t support_based_closure(const set_t &set, int set_support, const SupportTable &support){
  set_t c(set); 
  for(int i = 0; i < support.size(); i++){
    if(support[i] == set_support)
      c.push_back(i);
  }
  return c; 
}

