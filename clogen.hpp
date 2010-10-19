//
// clogen.hpp
// 
// Made by Benjamin Negrevergne
// Email   <@[firstname].[name]@@@imag.fr@>
// 
// Started on  Fri Sep  3 14:44:21 2010 Benjamin Negrevergne
//

#ifndef   	_CLOGEN_HPP_
#define   	_CLOGEN_HPP_

#include "pattern.hpp"

extern TransactionTable tt; 
extern TransactionTable ot; 

int clogen(set_t initial_pattern);

void parse_clogen_arguments(int *argc, char **argv); 

#endif	    /* _CLOGEN_HPP_ */
