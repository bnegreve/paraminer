/**
 * @file   paraminer.hpp
 * @author Benjamin Negrevergne <benjamin@neb.dyn.cs.kuleuven.be>
 * @date   Fri Sep  3 14:44:21 2010
 * 
 * @brief  ParaMiner main file. 
 * 
 */
#ifndef   	_PARAMINER_HPP_
#define   	_PARAMINER_HPP_

#include "database.hpp"
#include "pattern.hpp"

extern TransactionTable &tt; 
extern TransactionTable &ot; 

int paraminer(set_t initial_pattern);

/** 
 * \brief Print help for paraminer standard parameters. 
 *
 * @param bin_name Executable file name.
 * @param exit If set, paraminer_usage will call exit(EXIT_FAILURE). 
 */
void paraminer_usage(char *bin_name, bool exit=false); 

/** 
 * \brief Parse paraminer arguments. 
 * Optional arguments (paraminer or not) are moved to the beginning of argv.  
 *
 * @param argc 
 * @param argv 
 * @return The index of the first mandatory argument in argv.
 */
int parse_paraminer_arguments(int *argc, char **argv); 

#endif	    /* _PARAMINER_HPP_ */
