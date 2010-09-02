//
// pattern.hpp
// 
// Made by Benjamin Negrevergne
// Email   <@[firstname].[name]@@@imag.fr@>
// 
// Started on  Thu Sep  2 11:41:02 2010 Benjamin Negrevergne
//

#ifndef   	_PATTERN_HPP_
#define   	_PATTERN_HPP_

#include <vector>
#include "element.hpp" 


typedef std::vector<element_t> set_t; 

int membership_oracle(const set_t &set); 

set_t clo(const set_t &set); 

/* DEPRECATED */
set_t canonical_form(set_t set); 

/* DEPRECATED */
element_t canonical_transform_element(const set_t &set, element_t &element); 

/* return the canonical form of a pattern */
set_t canonical_form(set_t set, element_t *element = NULL);

#endif	    /* _PATTERN_HPP_ */
