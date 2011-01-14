//
// itemset_specific.hpp
// 
// Made by Benjamin Negrevergne
// Email   <@[firstname].[name]@@@imag.fr@>
// 
// Started on  Fri Sep  3 13:59:43 2010 Benjamin Negrevergne
//

#ifndef   	_ITEMSET_SPECIFIC_HPP_
#define   	_ITEMSET_SPECIFIC_HPP_

#include <iostream>
#include <vector>
#include "pattern.hpp"
#include "element.hpp"

/* Enable merging of identical transactions */ 
#define DATABASE_MERGE_TRANS
#define TRACK_TIDS

typedef element_t tid_t; 

struct Transaction : set_t{
  int original_tid;
  int weight;
#ifdef TRACK_TIDS
  set_t tids;
#endif
  /* Index of the first element of the second sort */
  int limit; 
}; 
struct TransactionTable : std::vector<Transaction>{
  element_t max_element;
};
//typedef std::vector<Transaction> TransactionTable; 
typedef set_t Occurence; 
typedef std::vector<Occurence> OccurenceTable; 
typedef std::vector<element_t> SupportTable; 

element_t read_transaction_table(TransactionTable *tt, const char *filename); 

void print_transaction(const Transaction &t); 

void print_transaction_table(const TransactionTable &tt); 

void transpose(const TransactionTable &tt, TransactionTable *ot); 


/** 
 * \brief Sort the tids occording to the lexical order of the
 * corresponding transactions in tt
 * 
 * @param tt TransactionTable containing the transaction referenced by the tids in \tids
 * @param tids Array of tids to sort.
 * @param begin First element to sort in \tids
 * @param end Last element to sort in \tid
 */
void quick_sort_tids( const TransactionTable &tt, Occurence *tids, 
		      int begin, int end);


/** 
 * \brief Merge identical entries in \tt into a single one with a higher weight. 
 * 
 * @param tt the database to shrink
 */
void merge_identical_transactions(TransactionTable *tt);


/** 
 * \brief Build a new database from a subset of the database \tt
 * Includes in the new table the occurences whose tids are in \occurences
 * Removes the elements that have in null support in \support
 */


void database_build_reduced(TransactionTable *new_tt, const TransactionTable &tt,
			    const Transaction &occurence, const SupportTable &support, 
			    const set_t &exclusion_list); 


/** 
 * \brief Build a new database from a subset of the database \tt
 * Includes in the new table the occurences whose tids are in \occurences
 */
void database_build_reduced(TransactionTable *new_tt, const TransactionTable &tt,
			    const Transaction &occurence); 

/* Deprecated */ 
void database_occuring_elements(set_t *elements, 
				const TransactionTable &tt, const Transaction &occurences);


int is_included_1d(const Transaction &t, const set_t &set); 

int count_inclusion_2d(const TransactionTable &tt, const set_t &set); 
int count_inclusion_2d(const TransactionTable &tt, const Transaction &occs, const set_t &set); 

void get_occurences_2d(const TransactionTable &tt, const set_t &set, Occurence *oc);

int get_set_presence_1d(const Transaction &t, const set_t &set);

// int membership_oracle(const set_t &set); 


// set_t clo(const set_t &set); 

/* DEPRECATED */
set_t canonical_form(set_t set); 

/* DEPRECATED */
element_t canonical_transform_element(const set_t &set, element_t &element); 

/* return the canonical form of a pattern */
set_t canonical_form(set_t set, element_t *element = NULL);


void compute_element_support(SupportTable *support, const TransactionTable &tt,
			     const Occurence &occs); 

void all_occurences(Transaction *occs, const TransactionTable &tt); 

#endif	    /* _ITEMSET_SPECIFIC_HPP_ */
