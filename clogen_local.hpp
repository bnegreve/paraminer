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


/* must be defined in clogen_local_*.cpp*/
extern element_t ELEMENT_RANGE_START; 
extern element_t ELEMENT_RANGE_END; 

int membership_oracle(const set_t &set); 
int membership_oracle(const set_t &set, const TransactionTable &tt, 
		      const Transaction &occurences); 

set_t clo(const set_t &set); 
set_t clo(const set_t &set, int set_support, const SupportTable &support); 


int main(int argc, char **argv); 

#endif	    /* _CLOGEN_LOCAL_HPP_ */
