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
#define SORT_DATABASE
#define TRACK_TIDS
#define REMOVE_NON_CLOSED

typedef element_t tid_t; 

struct Transaction : set_t{
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
 */
void quick_sort_tids( const TransactionTable &tt, Occurence *tids, 
		      int begin, int end);



/** 
 * \brief Merge identical entries in \tt into a single one with a higher weight. 
 * 
 * @param tt the database to shrink
 */
void merge_identical_transactions(TransactionTable *tt, bool remove_non_closed = false);


/** 
 * \brief Build a new database from a subset of the database \tt
 * Includes in the new table the occurences whose tids are in \occurences
 * Removes the elements that have in null support in \support
 */


void database_build_reduced(TransactionTable *new_tt, const TransactionTable &tt,
			    const Transaction &occurence, const SupportTable &support, 
			    const set_t &exclusion_list, int depth, bool merge=true); 


void database_build_reduced2(TransactionTable *new_tt, const TransactionTable &tt,
			    const Transaction &occurence, const set_t &pattern, 
			    const set_t &exclusion_list, int depth, bool el_reduce=true); 

void suffix_intersection(TransactionTable *tt, 
			 std::vector<tid_t> *input,
			 int begin, int end); 
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

void print_tt_info(const TransactionTable &tt); 


/** 
 * \brief Build a permutation array such that elements with lower
 * values are elements that do not belong to el.
 *
 * Given an exclusion list el, the following property is satisfied:
 * forall e in [0, max_element], if \permutations[e] >= \first_el,
 * then e is in el.
 *  
 * @param el_bit bit representation of the exlcusion list. 
 * @param el_size number of elements in the exclusion list. (i.e. number bits set in el_bit.)
 * @param max_element the maximal element, which is the last index of the permutation array.
 * @param first_el out parameter, value of the first element that belong to el. 
 * @param permutations out parameter, the permutation array. 
 */
void database_build_el_permutations(const std::vector<bool> &el_bit, int el_size, 
				    element_t max_element,
				    element_t *first_el, set_t *permutations); 

/** 
 *  \brief Given a database \tt, a set of \permutations, and a \limit,
 *  build the lexicographical order of the permuted transactions
 *  considering only elements in transactions below \limit.
 *
 * After a call to this function, the following property is true: For
 * every integer i and j, if i < j, tt[tids[i]] <pl tt[tids[j]], <pl
 * being the lexical order of the permuted transactions limited to the
 * elements below \limit.
 * (i.e. transaction where each element e is replaced by 
 * permutations[e], and every element above \first_el are considered equal).
 * 
 * @param tt Database, containing the transactions refered by the tids in \tids. 
 * @param permutations Permutation array. 
 * @param limit limit element. (First element to be ignored.)
 * @param tids in/out parameter, initially contain a set of tids,
 * after the call, contains the same set of tids ordered w.r.t the
 * <pl order of the refered transactions.
 */
  void database_sort_permuted_limited(const TransactionTable &tt, 
				      const set_t &permutations,
				      const element_t limit, 
				      Occurence *tids);


/** 
 * \brief Find the transaction partitions from a transaction table given a set of tids. 
 * 
 * A partition is a group of equivalent transactions. two transactions
 * are equivalent when they contains the same set of elements not
 * included in the exclusion list.  
 *
 * After a call to this function the partitions are available as pairs
 *  of index in the occurrence array. The first element refers to the
 *  index of the first tid in the partition, the second refers to the
 *  last.
 *
 * \warning the set of tids must be
 * ordered in the sens that two consecutives tids must refer to two
 * consecutives transaction w.r.t the lexicographical order. See
 * quick_sort_tids. 
 */
void find_partitions(const TransactionTable &tt, const set_t &tids, const std::vector<bool> &el,
		     std::vector<std::pair<int,int> > *partitions); 


/** 
 * \brief Builds the representative transaction from a partition of equivalent transactions
 * 
 * @param tt 
 * @param occurrence 
 * @param pattern the patter will be removed since it always belong to all transactions.
 * @param representative the unique representative transaction
 * @param max_element maximum element in the representative partition.
 */
void reduce_partition(const TransactionTable &tt, const Transaction &occurrences,
		      const std::pair<int,int> partition,
		      const set_bit_t &pattern, int pattern_size, 
		      Transaction *representative, element_t *max_element);

#endif	    /* _ITEMSET_SPECIFIC_HPP_ */
