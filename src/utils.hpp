/**
 * @file   utils.hpp
 * @author Benjamin Negrevergne <benjamin@neb.dyn.cs.kuleuven.be>
 * @date  Thu Sep  2 13:14:01 2010
 * 
 * @brief Convinience functions for various usages in ParaMiner. 
 */
#ifndef   	_UTILS_H_
#define   	_UTILS_H_

#include <string>
#include <vector>
#include "database.hpp" 
#include "pattern.hpp"

//#define TIMING 

void set_print(const set_t &set);


void pattern_print(const set_t &set, int u_data, const set_t &tids); 
  
/** \brief Print a set without calling \element_print(); 
 */
void set_print_raw(const set_t &set);


/** \brief check if \e occures in \set 
 */
bool set_member(const set_t &set, const element_t &e); 

/** \brief check if \e occures in \sorted_set assuming sorted_set is sorted. 
 */
bool set_member_sorted(const set_t &sorted_set, const element_t &e); 

void set_insert_sorted(set_t *sorted_set, const element_t e); 

int set_member_index(const set_t &set, const element_t &e); 

/** \brief returns true if the two sets are equals */ 
bool set_equal(const set_t &s1, const set_t &s2);

/** \brief returns true if the two sets are equals up to l1 in s1 and l2 in l2*/
bool set_equal_limited(const set_t &s1, int l1, const set_t &s2, int l2); 

/** \brief returns true if the two sets are equals ignoring element in excluded*/
bool set_equal_with_excluded_elements(const set_t &s1, const set_t &s2, const std::vector<bool> &excluded); 

/** \brief returns wether the first set is smaller than the other
    prioritizing elements that are not exlcuded. */
bool set_compare_with_excluded_elements(const set_t &s1, const set_t &s2,
					const std::vector<bool> &excluded); 


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

/** \brief Removes every element strictly greater than v
    \warning assumes that s is sorted. */
void set_truncate_above(set_t *s, element_t v); 

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


/** 
 * \brief For each elements in \permutations stores value at i at cell T[i]
 * 
 * @param permutations 
 */
void reverse_permutations(set_t *permutations);


const int EVENT_NODELAY = 0;
const int EVENT_START = 1;
const int EVENT_END = 2;

/** 
 * \brief Write the current time, the thread id and an info string into a trace file.
 * 
 * There is one trace file per thread.  Each info is comma separated.
 * The first field is the time stamp, the second field is a thread
 * identifier the last field is an info string (given)
 * 
 */
void trace_timestamp_print(const std::string &info, int start_end = EVENT_NODELAY); 


/** 
 * \brief Must be called before any usage of trace_*() function
 * 
 * \warning Not thread safe ! Must be called only once.
 */

void trace_init(int nb_threads); 
/** 
 * \brief Must be called before end of program in order to ensure that all traces were written. 
 * 
 */
void trace_exit(); 

/** 
 * \brief Build the bit representation of a set.
 *
 * after the call 
 * e in \set <=> \(*set_bit)[e] = true; 
 * 
 * @param set 
 * @param max_element maximum value in \set. 
 * @param set_bit out parameter, the bit representation of \set. 
 */
void set_to_bit_representation(const set_t &set, element_t max_element, 
			       std::vector<bool> *set_bit);

/** 
 * \brief Returns true if s1 is strictly smaller that s2 w.r.t <pl.
 *
 * let s1' be a set such that s1'[i] = s1[permuted[i]] and s2' defined
 * similarly w.r.t. s2.  This function returns true if s1' < s2'
 * (w.r.t. the lexicographical order) considering only the elements
 * strictly below \limit.
 *
 * s1 <lp s2 <=>
 * s1' \ {e >= limit} is lexigraphically smaller that s2' \ {e >=
 * limit}
 * 
 * @param s1 first set to compare.
 * @param s2 second set to compare. 
 * @param permutations
 * @param limit 
 * 
 * @return true if s1 <pl s2
 */
bool set_permuted_limited_compare(const set_t &s1, const set_t &s2, 
				  const set_t &permutations, element_t limit);


/** 
 * \brief Sort transaction, non el element first. 
 */
void elsort_transaction(Transaction *t, int max_element, const set_t &el); 
#endif 	    /* !_UTILS_H_ */
