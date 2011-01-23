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

void pattern_print(const set_t &set, int u_data){
  if(set.size() == 0){
    cout<<"{emptyset}"<<endl; 
    return ; 
  }
  for(int i = 0 ; i < set.size(); i++){
    element_print(set[i]); 
    cout<<" "; 
  }  
  cout<<"("<<u_data<<")"<<endl;
}

void set_print_raw(const set_t &set){
  if(set.size() == 0){
    cout<<"{emptyset}"<<endl; 
    return ; 
  }
  for(int i = 0 ; i < set.size(); i++){
    cout<<set[i]; 
    cout<<" : "; 
  }
  cout<<endl; 
}


bool set_equal(const set_t &s1, const set_t &s2){
  if(s1.size() != s2.size())
    return false; 
  set_t::const_iterator s1_end = s1.end(); 
  for(set_t::const_iterator it1 = s1.begin(), it2 = s2.begin(); it1 != s1_end; ++it1, ++it2){
    if(*it1 != *it2)
      return false; 
  }
  return true; 
}

bool set_equal_limited(const set_t &s1, int l1, const set_t &s2, int l2){
  if(l1 != l2)
    return false; 

  set_t::const_iterator end1 = s1.begin()+l1;
  set_t::const_iterator end2 = s2.begin()+l2; 

  for(set_t::const_iterator it1 = s1.begin(), it2 = s2.begin(); it1 != end1; ++it1, ++it2){
    if(*it1 != *it2)
      return false; 
  }
  return true; 
}

bool set_member(const set_t &set, const element_t &e){
  for(set_t::const_iterator it = set.begin(); it  != set.end(); ++it){
    if(*it == e)
      return true; 
  }
  return false; 
}

int set_lexical_compare(const set_t &t1, const set_t &t2){
  set_t::const_iterator tp = t1.begin(), tpEnd = t1.end();   
  set_t::const_iterator op = t2.begin(), opEnd = t2.end(); 
  int i = 0; 
  for(; tp < tpEnd && op < opEnd; ++tp, ++op, i++){
    if(*tp == *op){
      //      if(unlikely (*tp == prefixBound))
      //	return i+10; /* if the transactions are equals up to the prefix */
      continue; 
    }
    if(*tp > *op)
      return 1; 
    else
      return -1; 
  }
    
  if(tp == tpEnd && op == opEnd)
    return 0; 
  else
    if(tp != tpEnd) 
      return 1; 
    else
      return -1; 
}

void set_intersect(set_t *out, const set_t &t1, const set_t &t2){
  assert(is_sorted(t1) && is_sorted(t2));
  out->resize(std::min(t1.size(), t2.size()));
  Transaction::iterator it;
  it=set_intersection (t1.begin(), t1.end(), t2.begin(), t2.end(), out->begin());
  
  out->resize(it-out->begin());

}


int set_lexical_compare_limited(set_t::const_iterator s1, int end1, 
				set_t::const_iterator s2, int end2){
  set_t::const_iterator s1end = s1+end1; 
  set_t::const_iterator s2end = s2+end2; 

  int i = 0; 
  for(; s1 < s1end && s2 < s2end; ++s1, ++s2, i++){
    if(*s1 == *s2){
      //      if(unlikely (*s1 == limit_value))
      //      	return i+10; /* if the transactions are equals up to the prefix */
      continue; 
    }
    if(*s1 > *s2)
      return 1; 
    else
      return -1; 
  }
    
  if(s1 == s1end && s2 == s2end)
    return 0; 
  else
    if(s1 != s1end) 
      return 1; 
    else
      return -1; 
}


int set_member_index(const set_t &set, const element_t &e){
  int i = 0; 
  for(set_t::const_iterator it = set.begin(); it  != set.end(); ++it,++i){
    //    if(element_equal(*it, e))
    if(*it == e)
      return i; 
  }
  return -1; 
}

bool set_member_sorted(const set_t &sorted_set, const element_t &e){
  assert(is_sorted(sorted_set)); 
  for(set_t::const_iterator it = sorted_set.begin(); 
      it != sorted_set.end() && *it <= e; ++it){    
    if(*it == e)
      return true; 
  }
  return false; 
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

void set_insert_sorted(set_t *sorted_set, const element_t e){
  /* TODO improve efficiency */
  sorted_set->push_back(e); 
  set_t::iterator xlit = sorted_set->begin(); 
  while(xlit != sorted_set->end()){
    if(*xlit > e){
      std::swap(*xlit, (sorted_set->back())); 
    }
    ++xlit;
  }
 
}
