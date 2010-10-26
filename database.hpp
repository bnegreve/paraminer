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



typedef std::vector<element_t> Transaction;
typedef std::vector<Transaction> TransactionTable; 
typedef std::vector<int> Occurence; 
typedef std::vector<Occurence> OccurenceTable; 
typedef std::vector<int> SupportTable; 

void read_transaction_table(TransactionTable *tt, const char *filename); 

void print_transaction(const Transaction &t); 

void print_transaction_table(const TransactionTable &tt); 

void transpose(const TransactionTable &tt, TransactionTable *ot); 


/** 
 * \brief Build a new database from a subset of the database \tt
 * 
 */
void database_build_reduced(TransactionTable *new_tt, const TransactionTable &tt,
			    const Transaction &occurence); 


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


#endif	    /* _ITEMSET_SPECIFIC_HPP_ */
