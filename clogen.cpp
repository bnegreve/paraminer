/*
** clogen.cpp
** 
** Made by (Benjamin Negrevergne)
** Email   [firstname].[name]@imag.fr
** 
** Started on  Thu Sep  2 10:21:29 2010 Benjamin Negrevergne
*/
#include <iostream>
#include <algorithm>
#include <cstdio>

#include "utils.hpp"
#include "element.hpp"
#include "pattern.hpp"
#include "database.hpp"
#include "clogen_local.hpp"

int num_threads = 1; 

#define PARALLEL_PROCESS
#ifdef PARALLEL_PROCESS
extern "C" {
#include <tuplespace.h>
#include <thread.h>
}

#include "melinda_local.hpp"
tuplespace_t ts;

int depth_tuple_cutoff = 2;
#endif //PARALLEL_PROCESS

using std::cout; 
using std::endl;
using std::cerr; 
TransactionTable tt; 
TransactionTable ot; 

int threshold; 

/* Return the maximum element e so set \ {e} is in F */
/* assumes set is ordered */
element_t get_tail(const set_t &set, const TransactionTable &tt,
		   const Transaction &occurences){
  assert(false);
  #if 0
  element_t max = element_null; 
  
  set_const_rit it_end = set.rend(); 
  int i = set.size() - 1; 
  for(set_const_rit s_it = set.rbegin(); s_it != it_end; ++s_it, i--) {
    /* Generate a set without element set[i] */
    set_t s; 
    for(int j = 0; j < set.size(); j++){
      if(i != j)
	s.push_back(set[j]);
    }
    if(membership_oracle(s))
      return set[i];
      //      if(max == element_null || element_compare_ge(set[i], max))
      //	max=set[i];
    
  }
  assert(false); 
  return max; 
#endif 
}

 struct element_cmp {
   /* return true if e1 < e2 */
   bool operator()(const element_t &e1, const element_t &e2) const{
     return !element_compare_ge(e1, e2);
   }
 };

std::pair<set_t, element_t> get_first_parent(const set_t &set, const TransactionTable &tt,
					     const Transaction &occurences){
  assert(false); 
  #if 0
  assert(set.size() >= 0);
  assert(is_sorted(set));
  set_t z(set); 
  while(z.size() != 0){
    element_t e = get_tail(z, tt, occurences); 
    set_t x;
    for(int i = 0 ; i < z.size(); i++)
      if(z[i] != e)
	x.push_back(z[i]); 
    /* x = z  \ {e} */
    //    assert(clo(z) == set);
    /*If the closure of z \ {e} is not z itself it's his parent.*/
    set_t xx(clo(x)); 
    if( xx != set ){ //BUG DANS LE PAPIER DE UNO ?
      std::sort(xx.begin(), xx.end(), element_cmp()); 
      return make_pair(canonical_form(xx, &e), e);
    }
    z = x; 
  }
  /* Bottom !*/
  return make_pair(set_t(0), get_tail(set, tt, occurences));
  #endif
}

size_t expand(const TransactionTable &tt,const TransactionTable &ot, set_t s, element_t e, int depth, set_t *exclusion_list, int sup){
  
  set_t set(s); 

  /* occurences of e is occs of s u {e} since tt is restricted to occs(s) */
  Transaction occs = ot[e]; 
  
  
  SupportTable support; 
  compute_element_support(&support, tt, ot[e]); 
  set.push_back(e);
  int set_support=support[e];
  for(int i = 0; i < set.size(); i++){
    if(set[i]<support.size()) //TODO REINTRODUCE MAXITEM
      support[set[i]] = 0; 
  }

  std::sort(set.begin(), set.end());
  membership_data_t c_data = {tt, occs, ot[e], support};
  set_t c = clo(set, set_support, support, c_data); 
  //TODO ugly .. nevermind
  for(int i = 0; i < c.size(); i++){
    if(c[i]<support.size()) //TODO REINTRODUCE MAXITEM
      support[c[i]] = 0; //By doing this we remove from db the elements that already blong to the set.
  }
  //  std::sort(c.begin(), c.end()); //optional ?

  /* First parent test */ 
  /* We check here if the closed set \c has set \s as first parent. If
     not the call terminates because this same closed set will be
     generated from another branch */

  /* There are various way to to this, depending of the set system
     properties */
  //#define GET_FIRST_PARENT
#ifdef GET_FIRST_PARENT
  std::pair<set_t, element_t> first_parent = get_first_parent(c, tt, occs);       
  if(!(first_parent.first == s && first_parent.second == e))
    /* No need to explore this branch it will be explored from another recursive call */
    return 0;
#else
  for(int i = 0; i < c.size(); i++){
    if(set_member(*exclusion_list, c[i]))
      return 0;
  }
#endif
  pattern_print(c,sup); 
  
  size_t num_pattern = 1; 
 
  set_t cooccuring_elements; 
  for(int i = 0; i < support.size(); i++){
    if(support[i] != 0)
      cooccuring_elements.push_back(i); 
  }
  //  database_occuring_elements(&cooccuring_elements, tt, occs);  

  int nb_candidates = 0; 
  set_t extensions; 

  /* store the value returned by membership foreach element */
  set_t u_data(support.size()); 

  set_t::const_iterator it_co_end = cooccuring_elements.end(); 
  for(set_t::const_iterator it_co = cooccuring_elements.begin(); it_co != it_co_end; ++it_co){
    element_t current = *it_co; 

    membership_data_t m_data = {tt, occs, ot[current], support};    

    if( (u_data[current] = membership_oracle(c,current, m_data))){
      //if(membership_oracle(candidate_set)){
      if(!set_member(*exclusion_list, current))
	extensions.push_back(current);      
    }
  }
  
  if(extensions.size() > 0){
    TransactionTable *new_tt = new TransactionTable;  // TODO free this memory !     
    database_build_reduced(new_tt, tt, occs, support); //TODO occurences c, pas de s !
    TransactionTable *new_ot = new TransactionTable; 
    transpose(*new_tt, new_ot); /* occurence deliver .. sort of */


    if(depth < depth_tuple_cutoff){
      set_t::const_iterator c_it_end = extensions.end(); 
      for(set_t::const_iterator c_it = extensions.begin(); c_it != c_it_end; ++c_it){
	if(set_member(*exclusion_list, *c_it))
	  continue; 
	//	cout<<"thread "<<m_thread_id()<<" is putting tuple: "<<endl;
	//	set_print(c); 
	tuple_t tuple;
	tuple.tt = new_tt; 
	tuple.ot = new_ot; 
	tuple.s = new set_t(c); 
	tuple.e = *c_it; 
	tuple.depth = depth+1; 
	tuple.exclusion_list = new set_t(*exclusion_list); 
	tuple.u_data = u_data[*c_it]; 
	m_tuplespace_put(&ts, (opaque_tuple_t*)&tuple, 1);

	/* insert the current extension into the exclusion list for the next calls.*/
	exclusion_list->push_back(*c_it); 
      }
    }
    else{

      
      set_t::const_iterator c_it_end = extensions.end(); 
      for(set_t::const_iterator c_it = extensions.begin(); c_it != c_it_end; ++c_it){     
	set_t new_exclusion_list(*exclusion_list);
	num_pattern += expand(*new_tt, *new_ot, c, *c_it, depth+1, &new_exclusion_list, u_data[*c_it]);
	/* insert the current extension into the exclusion list for the next calls.*/
	exclusion_list->push_back(*c_it); 
      }
      delete new_tt;
      delete new_ot;
    }
  }
  return num_pattern; 
}

#ifdef PARALLEL_PROCESS

void *process_tuple(void *){
  size_t num_patterns = 0; 
  m_thread_register();   
  for(;;){
    tuple_t tuple; 
    
    int r = m_tuplespace_get(&ts, 1, (tuple_t*)&tuple);
    if(r == TUPLESPACE_CLOSED)
      break; 

    //    cout<<"thread "<<m_thread_id()<<" is processing tuple: "<<endl;
    //    set_print(*(set_t*)tuple); 
    //    size_t x = expand(*tuple.set, tuple.depth); 
    //    cout<<"tuple made "<< x <<"tuples"<<endl;
    num_patterns += expand(*tuple.tt, *tuple.ot, *tuple.s, tuple.e, 
			   tuple.depth, tuple.exclusion_list, tuple.u_data);
    //    num_patterns += x; 
    delete tuple.s; 
  }
  return reinterpret_cast<void*>(num_patterns); 
}


#endif //PARALLEL_PROCESS

int parse_clogen_arguments(int *argc, char **argv){
  char opt_char=0;
  while ((opt_char = getopt(*argc, argv, "c:t:")) != -1)
    {
      switch(opt_char)
	{
	case 'c':
	  depth_tuple_cutoff = atoi(optarg); 
	  cout<<"depth cutoff set to "<<depth_tuple_cutoff<<endl;
	  *optarg = '\0'; 
	  break ;
	case 't':
	  num_threads = atoi(optarg);
	  *optarg = '\0'; 
	  break; 
	default:
	  
	  // usage(argv[0]); 
	  // exit(-1);
	  break;	  
	}
    }
  cout<<"running with "<<num_threads<<" thread(s)."<<endl;  

  return optind; 
}

int clogen(set_t initial_pattern){
  int num_pattern = 0; 
  cerr<<"DATABASE REDUCTION ENABLED"<<endl;
#ifndef NDEBUG
  cout<<"CLOGEN DEBUG"<<endl;
#endif

#ifdef PARALLEL_PROCESS

 
  m_tuplespace_init(&ts, sizeof(tuple_t), 0, TUPLESPACE_OPTIONAUTOCLOSE); 
  m_thread_register(); 


  set_t empty_set;
  set_t exclusion_list;

  Occurence all_tids(tt.size()); 
  for(int i = 0; i < tt.size(); i++)
    all_tids[i]=i; 
  SupportTable support;
  compute_element_support(&support, tt, all_tids);

  
  
  for(element_t  current = element_first(); 
      current != element_null; current = element_next(current)){
    membership_data_t m_data = {tt,all_tids,ot[current],support};
    int sup; 
    if( (sup = membership_oracle(empty_set,current, m_data)) ){
      	tuple_t tuple;
	tuple.tt = &tt; 
	tuple.ot = &ot; 
	tuple.s = new set_t(empty_set); 
	tuple.e = current;
	tuple.depth = 0;
	tuple.exclusion_list = new set_t(exclusion_list);
	tuple.u_data = sup; 
	m_tuplespace_put(&ts, (opaque_tuple_t*)&tuple, 1);
	exclusion_list.push_back(current);
	}
    }
  

  //  Run the threads
  pthread_t *tids = new pthread_t[num_threads - 1];
  for(int i = 0; i < num_threads - 1; i++){
    cout<<"Creating thread"<<endl;
    if(pthread_create(&tids[i], NULL, 
  		      (void*(*)(void*))process_tuple, (void*)i)){
      perror("pthread_create ");
      exit(EXIT_FAILURE); 
    }
  }


  
  m_tuplespace_close_at(&ts, num_threads);   
  num_pattern += reinterpret_cast<size_t>(process_tuple(0)); 

  for(int i = 0; i < num_threads -1; i++){
    void *num_thread_pattern; 
    pthread_join(tids[i], &(num_thread_pattern));
    num_pattern += reinterpret_cast<size_t>(num_thread_pattern); 
  }
  delete[] tids;

#else
  num_pattern = expand(clo(empty_set), 0); 
#endif//PARALLEL_PROCESS

  return num_pattern; 
}
#ifndef TEST


#endif //TEST
