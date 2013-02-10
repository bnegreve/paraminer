/**
 * @file   database.hpp
 * @author Benjamin Negrevergne <benjamin@neb.dyn.cs.kuleuven.be>
 * @date   Fri Sep  3 13:59:43 2010
 * 
 * 
 * @brief  Public header for database related types and functions.
 *
 * 
 */
#ifndef   	_DATABASE_HPP_
#define   	_DATABASE_HPP_

#include <iostream>
#include <vector>
#include "pattern.hpp"
#include "element.hpp"

/* Various database options. Depending on the problem solved, these
 options may be mandatory.  Ideally this should be dynamic options but
 it's not. */

#define TRACK_TIDS		/**< Enable TID tracking in ParaMiner. */
#define DATABASE_MERGE_TRANS	/**< Enable merging of identical transactions. */
#define SORT_DATABASE		/**< Sort elements in transactions.  */
#define REMOVE_NON_CLOSED	/**< Remove elements that cannot be
				   part of any closure (el-reduce)*/


/* Basic data types in ParaMiner */
typedef element_t tid_t;	/**< Transaction identifier type (tid). */

/** 
 * @class Transaction
 * Used to store a transaction.
 */
struct Transaction : set_t{ 
  int weight; 	    /**< Weight of the transaction (involved in support counting) */
#ifdef TRACK_TIDS
  set_t tids;	    /**< Transaction id (tid) of the transaction. */
#endif
  /* Index of the first element of the second sort */
  int limit; 
}; 

/** 
 * @class TransactionTable
 * Used to store a set of transactions (dataset).
 */
struct TransactionTable : std::vector<Transaction>{
  element_t max_element;	/**< Maximum element among this TransactionTable. */
};

/** 
 * @typedef Occurence
 * Set of tids.
 * Ocurrences of an element are the list of transaction ids (tid)
 * of the transactions in which it occurrs.
 */
typedef set_t Occurence;

/** 
 * @typedef OccurenceTable
 * Array of Occurrences.  
 * Used to store the occurrences of each
 * element of a TransactionTable (see transpose functions.) 
 */
typedef std::vector<Occurence> OccurenceTable;

/** 
 * @typedef SupportTable 
 * Array of support count. Used to store the support count of a set of element. 
 */
typedef std::vector<element_t> SupportTable;







/** 
 * @brief Read the the file \filename and fill the TransactionTable
 * \tt with the values in this file.
 * 
 * Transactions are sorted if SORT_DATABASE is defined.
 *
 * @param tt The TransactionTable to fill. 
 * @param filename Name of the file containing the data. 
 * 
 * @return The value of the maximal element in the dataset.
 */
element_t read_transaction_table(TransactionTable *tt, const char *filename); 

/** 
 * @brief Print the transaction t on stdout. 
 * 
 * @param t 
 */
void print_transaction(const Transaction &t); 

/** 
 * @brief Print the transaction table \tt on stdout.
 * 
 * @param tt The TransactionTable to print.
 */
void print_transaction_table(const TransactionTable &tt); 

/** 
 * \brief Build the transposed (i.e. vertical) representation of the TransactionTable \tt.
 * 
 * Given a transaction table, build the transposed transaction table
 * \ot that contains for each elements i the set of transactions in
 * which it occurs in tt.
 *
 * After a call to transpose_tids, 
 * (*ot)[e] contains i iff i is a tid in \tids such that tt[i] contains e. 
 *
 * @param tt The input transaction table. 
 * @param ot The output, transposed transaction table. 
 */
void transpose(const TransactionTable &tt, TransactionTable *ot); 

/** 
 * \brief Build the transposed (i.e. vertical) representation of the TransactionTable \tt.
 * 
 * Given a transaction table and a set of tids, build the transposed
 * transaction table \ot that contains for each elements i the set of
 * transactions in which it occurs in tt. 
 * \warning Only the tids contained in \tids are regarded.
 *
 * After a call to transpose_tids, 
 * (*ot)[e] contains i iff i is a tid in \tids such that tt[i] contains e. 
 * 
 * @param tt The input transaction table. 
 * @param tids Set of tids of the transactions in tt that must be considered.
 * @param ot The output, transposed transaction table. 
 */
void transpose_tids(const TransactionTable &tt, const Transaction &tids, TransactionTable *ot); 

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

/** 
 * \brief Build a reduced dataset (in place process).
 *
 * The inplace reduce is computationaly more intensive but reduces the
 * memory consumption and preserve cache usage.  On a computing
 * platform with a sufficient amount of cores, it is worse using this
 * implementations. 
 * 
 */
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

/** 
 * @brief Count the support of each element in the transactions
 * described by \occs and store it in \support array.
 *
 * After a call, support[i] contains the support of item i.
 * \warning Transactions whose tid doesn't belong to \occs are ignored.  
 * \warning The weight of each transaction containing an element i is
 * added to the support count.
 *
 * @param support The output array.
 * @param tt The TransactionTable. 
 * @param occs List of tids in \tt that must be considered in the support count. 
 */
void compute_element_support(SupportTable *support, const TransactionTable &tt,
			     const Occurence &occs); 

/** 
 * @brief Print the transaction table tt. 
 */
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

#endif	    /* _DATABASE_ */
