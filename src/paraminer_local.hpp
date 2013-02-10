/**
 * @file   paraminer_local.hpp
 * @author Benjamin Negrevergne <benjamin@neb.dyn.cs.kuleuven.be>
 * @date   Tue Oct 19 17:59:52 2010 
 * 
 * @brief  Public header for ParaMiner instance source code.
 * 
 * 
 */
#ifndef   	_PARAMINER_LOCAL_HPP_
#define   	_PARAMINER_LOCAL_HPP_
#include "database.hpp"
#include "pattern.hpp"
#include "element.hpp" 

typedef struct{
  const TransactionTable &tt;	  
  /**< A database containing at least all the transaction including
     \base_set union \extension*/
  const Occurence &base_set_occurences;
  /**< Tids in \tt of the transactions including \base_set */
  const Occurence &extension_occurences;
  /**< Tids in \tt of the transactions including \extension */
  const SupportTable &support;
  /**< support of all element computed in the transaction including
     base_set. The support of e is the sum of the transactions weight
     of each transactions containing e. */
}membership_data_t; 

/* must be defined in paraminer_local_*.cpp*/
extern element_t ELEMENT_RANGE_START; 
extern element_t ELEMENT_RANGE_END; 

extern bool show_tids; /**< this boolean value is set to true if the
			  program is called with the -l option
			  (display tids) */

/** 
 * \brief Membership! Must return 1 iff the given \base_set union
 * \extension is a interesting set.

 * \warning The set does not need to
 * be closed. The closure operator will be applyed afterward. For
 * instance, if one wants to mine all closed frequent itemsets, the
 * function must return 1 iff the give \base_set union \extension is
 * frequent in the database.
 *
 * \warning Elements in \base_set have been removed from the
 * TransactionTable \tt. Since \tt is restricted to the occurences of
 * \base_set, all elements in \base_set should occures in all transactions of
 * \tt. Therefore it is useless to keep them.
 * 
 * @param base_set The base set. This set is guarenteed be a closed
 * interesting pattern in the initial database.

 * @param extension The extension of \base_set. The membership must return 1
 * iff \base_set union \extension is an interesting pattern.

 * @param tt A transaction table restricted to the occurences of \base_set 
 * (therefore it contains all occurences of \base_set U \extension). 

 * @param occurences The occurences of \extension in \tt. 
 * 
 * @return 1 iff \base_set union \extension is an interesting set pattern.
 */

int membership_oracle(const set_t &base_set, const element_t extension, const membership_data_t &data);



typedef struct{
  const TransactionTable &tt;	  
  /**< A database containing at least all the transactions of the set to be closed*/
  const Occurence &occurences;
  /**< Tids in \tt of the transactions including the set to be closed */  
  const SupportTable &support;
  /**< support of all element computed in the transaction including
     base_set. The support of e is the sum of the transactions weight
     of each transactions containing e. */
  int set_support;
  /**< The support of the set to be closed (sum of the transaction weight of each transactions) */  
}closure_data_t;


set_t clo(const set_t &set, const closure_data_t &data); 


int main(int argc, char **argv); 


extern int num_threads;

#endif	    /* _PARAMINER_LOCAL_HPP_ */
