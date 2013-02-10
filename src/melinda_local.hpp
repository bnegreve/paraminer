/**
 * @file   melinda_local.hpp
 * @author Benjamin Negrevergne <benjamin@neb.dyn.cs.kuleuven.be>
 * @date   Thu Sep  9 16:42:23 2010 
 * 
 * @brief  Melinda config file for ParaMiner. 
 * 
 */

#ifndef   	_MELINDA_LOCAL_HPP_
#define   	_MELINDA_LOCAL_HPP_


extern "C" {
#include <melinda.h>
#include <thread.h>
}

typedef struct{
  TransactionTable *tt; 
  TransactionTable *ot;
  Transaction *occs; 
  set_t *s; 
  element_t e; 
  int depth; 
  set_t *exclusion_list;
  set_t *exclusion_list_tail;
  int u_data; 
  bool shared_tt; 
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
