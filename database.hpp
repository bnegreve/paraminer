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
#include "element.hpp"

typedef std::vector<element_t> Transaction;
typedef std::vector<Transaction> TransactionTable; 
typedef std::vector<int> Occurence; 
typedef std::vector<Occurence> OccurenceTable; 

void read_transaction_table(TransactionTable *tt, const char *filename); 

void print_transaction(const Transaction &t); 

void print_transaction_table(const TransactionTable &tt); 

void transpose(const TransactionTable &tt, TransactionTable *ot); 


#endif	    /* _ITEMSET_SPECIFIC_HPP_ */
