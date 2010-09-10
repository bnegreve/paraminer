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
int nb_patterns = 0; 

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

TransactionTable tt; 
TransactionTable ot; 

int threshold; 

/* Return the maximum element e so set \ {e} is in F */
/* TODO rewrite this extreamly inefficient function a smarter way */
element_t get_tail(const set_t &set){
  
  element_t max = element_null; 
  
  for(int i = 0; i < set.size(); i++){
    /* Generate a set without element i */
    set_t s; 
    for(int j = 0; j < set.size(); j++){
      if(i != j)
	s.push_back(set[j]);
    }
    if(membership_oracle(s)){
      if(max == element_null || element_compare_ge(set[i], max))
	max=set[i];
    }
  }
  return max; 
}

 struct element_cmp {
   /* return true if e1 < e2 */
   bool operator()(const element_t &e1, const element_t &e2) const{
     return !element_compare_ge(e1, e2);
   }
 };

std::pair<set_t, element_t> get_first_parent(const set_t &set){
  assert(set.size() >= 0);
  set_t z(set); 
  while(z.size() != 0){
    element_t e = get_tail(z); 
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
  assert(false); 
}


void expand(set_t c, int depth){
  set_print(c); nb_patterns++; 
  //  cout<<membership_oracle(c)<<endl;
  for(element_t  current = element_first(); current != element_null; current = element_next(current)){

    if(set_member(c, current))
      continue; 
    set_t d = c; 

    d.push_back(current);
    std::sort(d.begin(), d.end()); 
    if(membership_oracle(d) == 0)
      continue; 

    d = clo(d); 
    
      /* retreive real parent of d and compare with the set of the current branch*/
      std::pair<set_t, element_t> first_parent = get_first_parent(d); 
      
      if(first_parent.first == c && first_parent.second == current){
#ifdef PARALLEL_PROCESS

	if(depth < depth_tuple_cutoff){
	//	cout<<"thread "<<m_thread_id()<<" is putting tuple: "<<endl;
	//	set_print(c); 
	tuple_t tuple;
	tuple.set = new set_t(d); 
	tuple.depth += 1; 
	m_tuplespace_put(&ts, (opaque_tuple_t*)&tuple, 1);
	}
	else{
	  expand(d, depth+1);
	}
#else
	expand(d, depth+1);
#endif //PARALLEL_PROCESS	
      }
  }
}

#ifdef PARALLEL_PROCESS

void *process_tuple(void *){
  m_thread_register(); 
  for(;;){
    tuple_t tuple; 
    
    int r = m_tuplespace_get(&ts, 1, (tuple_t*)&tuple);
    if(r == TUPLESPACE_CLOSED)
      break; 

    //    cout<<"thread "<<m_thread_id()<<" is processing tuple: "<<endl;
    //    set_print(*(set_t*)tuple); 
    expand(*tuple.set, tuple.depth); 
    delete tuple.set; 
  }
}


#endif //PARALLEL_PROCESS

void usage(char *bin_name){
  cout<<bin_name<<" inputfile minsup numthreads [-c freq cutoff]"<<endl;
}

#ifndef TEST

int main(int argc, char **argv){

  if(argc < 4){
    usage(argv[0]); 
    exit(EXIT_FAILURE); 
  }

  char opt_char=0;
  while ((opt_char = getopt(argc, argv, "c:")) != -1)
    {
      switch(opt_char)
	{
	case 'c':
	  depth_tuple_cutoff = atoi(optarg); 
	  cout<<"depth cutoff set to "<<depth_tuple_cutoff<<endl;
	  break ;
	default:
	  usage(argv[0]); 
	  exit(-1);
	  break;
	}
    }

  read_transaction_table(&tt, argv[optind]); 
  transpose(tt, &ot);
  threshold = atoi(argv[optind+1]); 

  set_t x;
  set_t empty_set; 

#ifndef NDEBUG
  cout<<"CLOGEN DEBUG"<<endl;
#endif

#ifdef PARALLEL_PROCESS

  int num_threads = atoi(argv[optind+2]); 
  m_tuplespace_init(&ts, sizeof(tuple_t), 0, TUPLESPACE_OPTIONAUTOCLOSE); 
  m_thread_register(); 

  expand(clo(empty_set), 0); 

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
  process_tuple(0);
  
  for(int i = 0; i < num_threads -1; i++)
    pthread_join(tids[i], NULL);
  delete[] tids;

#else
  expand(clo(empty_set), 0); 
#endif//PARALLEL_PROCESS

}

#endif //TEST
