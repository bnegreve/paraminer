//
// melinda_local.hpp
// 
// Made by Benjamin Negrevergne
// Email   <@[firstname].[name]@@@imag.fr@>
// 
// Started on  Thu Sep  9 16:42:23 2010 Benjamin Negrevergne
//

#ifndef   	_MELINDA_LOCAL_HPP_
#define   	_MELINDA_LOCAL_HPP_


extern "C" {
#include <melinda.h>
#include <thread.h>
}

typedef struct{
  TransactionTable *tt; 
  TransactionTable *ot; 
  set_t *s; 
  element_t e; 
  int depth; 
  set_t *exclusion_list;   
}tuple_t; 

int m_distribute(opaque_tuple_t *tuple){
  return m_thread_id(); 
}

int m_retrieve(){
  return m_thread_id(); 
}


#endif	    /* _MELINDA_LOCAL_HPP_ */
