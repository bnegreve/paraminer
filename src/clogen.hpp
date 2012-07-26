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

#include "database.hpp"
#include "pattern.hpp"

extern TransactionTable &tt; 
extern TransactionTable &ot; 

int clogen(set_t initial_pattern);

/** 
 * \brief Print help for clogen standard parameters. 
 *
 * @param bin_name Executable file name.
 * @param exit If set, clogen_usage will call exit(EXIT_FAILURE). 
 */
void clogen_usage(char *bin_name, bool exit=false); 

/** 
 * \brief Parse clogen arguments. 
 * Optional arguments (clogen or not) are moved to the beginning of argv.  
 *
 * @param argc 
 * @param argv 
 * @return The index of the first mandatory argument in argv.
 */
int parse_clogen_arguments(int *argc, char **argv); 

#endif	    /* _CLOGEN_HPP_ */
