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

#include "database.hpp" 
#include "pattern.hpp"


void set_print(const set_t &set);


void pattern_print(const set_t &set, int u_data); 
  
/** \brief Print a set without calling \element_print(); 
 */
void set_print_raw(const set_t &set);


/** \brief check if \e occures in \set 
 */
bool set_member(const set_t &set, const element_t &e); 

/** \brief check if \e occures in \sorted_set assuming sorted_set is sorted. 
 */
bool set_member_sorted(const set_t &sorted_set, const element_t &e); 

int set_member_index(const set_t &set, const element_t &e); 

/** \brief returns true if the two sets are equals */ 
bool set_equal(const set_t &s1, const set_t &s2);

bool set_equal_limited(const set_t &s1, int l1, const set_t &s2, int l2); 

int set_lexical_compare(const set_t &t1, const set_t &t2); 

void set_intersect(set_t *out, const set_t &t1, const set_t &t2); 

int set_lexical_compare_limited(set_t::const_iterator s1, int end1, 
				set_t::const_iterator s2, int end2); 

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


/** 
 * \brief Returns 1 if occures \min_sup times or more in \tt, 0 otherwise. 
 */
int set_is_frequent(const set_t &set, const TransactionTable &tt, int min_sup); 

/** 
 * \brief Returns 1 if occures \min_sup times or more in \tt
 * restricted to transactions refered in \occurences, 0 otherwise.
 */
int set_is_frequent_in_occurences(const set_t &set, const TransactionTable &tt,
				  const Transaction &occurences, int min_sup); 


/** 
 * \brief Compute the support_based closure of set \s 
 */
set_t support_based_closure(const set_t &s); 

/** 
 * \brief Compute the support_based closure of set \s given the
 * support of each item in the occurences of set.
 * \warning support must be the exact support of each item in the occurences of set.
 * 
 */
set_t support_based_closure(const set_t &set, int set_support, const SupportTable &support); 


#endif 	    /* !_UTILS_H_ */
