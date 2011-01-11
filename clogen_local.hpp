/*
** clogen_local.hpp
** 
** Made by Benjamin Negrevergne
** Email   [firstname].[name]@imag.fr
** 
** Started on  Tue Oct 19 17:59:52 2010 Benjamin Negrevergne
*/

#ifndef   	_CLOGEN_LOCAL_HPP_
#define   	_CLOGEN_LOCAL_HPP_
#include "database.hpp"
#include "pattern.hpp"
#include "element.hpp" 

//int main(int argc, char **argv);

typedef struct{
  const TransactionTable &tt;	  
  /**< A database containing at least all the transaction including
     \base_set union \extension*/
  const Occurence &base_set_occurences;
  /**< Tids in \tt of the transactions including \base_set */
  const Occurence &extension_occurences;
  /**< Tids in \tt of the transactions including \extension */
  const SupportTable &support;
  /**< support of all element computed in the transaction including base_set */
}membership_data_t; 

/* must be defined in clogen_local_*.cpp*/
extern element_t ELEMENT_RANGE_START; 
extern element_t ELEMENT_RANGE_END; 

int membership_oracle(const set_t &set); 
int membership_oracle(const set_t &set, const TransactionTable &tt, 
		      const Transaction &occurences); 

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
int membership_oracle(const set_t &base_set, const element_t extension, 
		      const TransactionTable &tt, const Transaction &occurences, const SupportTable &support);


int membership_oracle(const set_t &base_set, const element_t extension, const membership_data_t &data);

set_t clo(const set_t &set); 
set_t clo(const set_t &set, int set_support, const SupportTable &support, 
	  const membership_data_t &data); 


int main(int argc, char **argv); 

#endif	    /* _CLOGEN_LOCAL_HPP_ */
