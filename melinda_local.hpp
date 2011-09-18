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
  int u_data; 
}tuple_t; 

#define STRAT_STANDARD

#ifdef STRAT_USE_LESS_MEM
int m_distribute(opaque_tuple_t *tuple){
  tuple_t *t = (tuple_t*)tuple; 
  return  ((long int)t->tt)%64; 
  //  return 0; 
}


int m_retrieve(){
  return 1;
}
#endif


#ifdef STRAT_BIG_FIRST
int m_distribute(opaque_tuple_t *tuple){
  tuple_t *t = (tuple_t*)tuple;
  
  //  printf("DBSIZE %d\n", ((long int)t->tt->size())); 
  if(((long int)t->tt->size()) > 500){    
    return 0; 
  } 
  if(((long int)t->tt->size()) > 100){    
    return 1; 
  } 
  return 2; 
}


int m_retrieve(){
  return 0;
}
#endif


#ifdef STRAT_STANDARD
int m_distribute(opaque_tuple_t *tuple){
    return m_thread_id(); 
}

int m_retrieve(){
    return m_thread_id(); 
}
#endif

#endif	    /* _MELINDA_LOCAL_HPP_ */
