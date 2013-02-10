/*
** utils.c
** 
** Made by (Benjamin Negrevergne)
** Email   [firstname].[name]@imag.fr
** 
** Started on  Thu Sep  2 13:15:21 2010 Benjamin Negrevergne
*/

#include <iostream>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <pthread.h>
#include <sys/time.h>
extern "C" {
#include <thread.h>
}
#include "utils.hpp"

using namespace std; 

void set_print(const set_t &set){
  if(set.size() == 0){
    cout<<"{emptyset}"<<endl; 
    return ; 
  }
  for(int i = 0 ; i < set.size(); i++){
    element_print(set[i]); 
    cout<<" : "; 
  }
  cout<<endl; 
}

void pattern_print(const set_t &set, int u_data, const set_t &tids){
  static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
  pthread_mutex_lock(&mutex); 
  if(set.size() == 0){
    cout<<"{emptyset}"<<endl; 
  }
  else
    for(int i = 0 ; i < set.size(); i++){
      element_print(set[i]); 
      cout<<" "; 
    }  
  cout<<"("<<u_data<<")" ;

#ifdef TRACK_TIDS
  if (tids.size() > 0) {
    cout << " [ " ;
    for (int i=0 ; i<tids.size() ; ++i)
      cout << tids[i] << " " ;
    cout << "]" ;
  }
#endif //TRACK_TIDS
  cout << endl ;
  
  pthread_mutex_unlock(&mutex); 
}

void set_print_raw(const set_t &set){
  if(set.size() == 0){
    cout<<"{emptyset}"<<endl; 
    return ; 
  }
  for(int i = 0 ; i < set.size(); i++){
    cout<<set[i]; 
    cout<<" : "; 
  }
  cout<<endl; 
}


bool set_equal(const set_t &s1, const set_t &s2){
  if(s1.size() != s2.size())
    return false; 
  set_t::const_iterator s1_end = s1.end(); 
  for(set_t::const_iterator it1 = s1.begin(), it2 = s2.begin(); it1 != s1_end; ++it1, ++it2){
    if(*it1 != *it2)
      return false; 
  }
  return true; 
}

bool set_equal_limited(const set_t &s1, int l1, const set_t &s2, int l2){
  if(l1 != l2)
    return false; 

  set_t::const_iterator end1 = s1.begin()+l1;
  set_t::const_iterator end2 = s2.begin()+l2; 

  for(set_t::const_iterator it1 = s1.begin(), it2 = s2.begin(); it1 != end1; ++it1, ++it2){
    if(*it1 != *it2)
      return false; 
  }
  return true; 
}



/** \brief returns true if the two sets are equals ignoring element in excluded*/
bool set_equal_with_excluded_elements(const set_t &s1, const set_t &s2,
				      const std::vector<bool> &excluded){
    
  set_t::const_iterator end1 = s1.end();
  set_t::const_iterator end2 = s2.end(); 

  set_t::const_iterator it1 = s1.begin(), it2 = s2.begin();
  while(it1 != end1 && it2 != end2){
    if(excluded[*it1]){
      ++it1; continue; 
    }
    if(excluded[*it2]){
      ++it2; continue; 
    }
    if(*it1 != *it2)
      return false; 
    ++it1; ++it2;
  }
  
  for(;it1 != end1;++it1)
    if(!excluded[*it1])
      return false; 
  
  for(;it2 != end2;++it2)
    if(!excluded[*it2])
      return false; 

  return true; 
}

/** \brief returns wether the first set is smaller than the other
    prioritizing elements that are not exlcuded. 
    
    \warning Elements in transactions must be sorted in each category
    (el and non-el). 
*/

bool set_compare_with_excluded_elements(const set_t &s1, const set_t &s2,
					const std::vector<bool> &excluded){

  assert(false); 
  /* First compare non-el elements */
  set_t::const_iterator end1 = s1.end();
  set_t::const_iterator end2 = s2.end(); 

  set_t::const_iterator it1 = s1.begin(), it2 = s2.begin();

  
  while(it1 != end1 && it2 != end2){
    if(excluded[*it1]){
      ++it1; continue; 
    }
    if(excluded[*it2]){
      ++it2; continue; 
    }
    if(*it1 < *it2)
      return true; 

    else if(*it1 > *it2)
      return false; 

    ++it1; ++it2;
  }
  
  for(;it1 != end1;++it1)
    if(!excluded[*it1])
      return false; 
  
  for(;it2 != end2;++it2)
    if(!excluded[*it2])
      return true;
  
  /* now compare el-elements. */
  it1 = s1.begin(), it2 = s2.begin();

    while(it1 != end1 && it2 != end2){
    if(!excluded[*it1]){
      ++it1; continue; 
    }
    if(!excluded[*it2]){
      ++it2; continue; 
    }
    if(*it1 < *it2)
      return true; 

    else if(*it1 > *it2)
      return false; 

    ++it1; ++it2;
  }
  
  for(;it1 != s1.end();++it1)
    if(excluded[*it1])
      return false; 
  
  for(;it2 != s2.end();++it2)
    if(excluded[*it2])
      return true;

  /* set are equals */
  return false; 
}


bool set_member(const set_t &set, const element_t &e){
  for(set_t::const_iterator it = set.begin(); it  != set.end(); ++it){
    if(*it == e)
      return true; 
  }
  return false; 
}

int set_lexical_compare(const set_t &t1, const set_t &t2){
  set_t::const_iterator tp = t1.begin(), tpEnd = t1.end();   
  set_t::const_iterator op = t2.begin(), opEnd = t2.end(); 
  int i = 0; 
  for(; tp < tpEnd && op < opEnd; ++tp, ++op, i++){
    if(*tp == *op){
      //      if(unlikely (*tp == prefixBound))
      //	return i+10; /* if the transactions are equals up to the prefix */
      continue; 
    }
    if(*tp > *op)
      return 1; 
    else
      return -1; 
  }
    
  if(tp == tpEnd && op == opEnd)
    return 0; 
  else
    if(tp != tpEnd) 
      return 1; 
    else
      return -1; 
}

void set_intersect(set_t *out, const set_t &t1, const set_t &t2){
  assert(is_sorted(t1) && is_sorted(t2));
  out->resize(std::min(t1.size(), t2.size()));
  Transaction::iterator it;
  it=set_intersection (t1.begin(), t1.end(), t2.begin(), t2.end(), out->begin());
  
  out->resize(it-out->begin());

}


int set_lexical_compare_limited(set_t::const_iterator s1, int end1, 
				set_t::const_iterator s2, int end2){
  set_t::const_iterator s1end = s1+end1; 
  set_t::const_iterator s2end = s2+end2; 

  int i = 0; 
  for(; s1 < s1end && s2 < s2end; ++s1, ++s2, i++){
    if(*s1 == *s2){
      //      if(unlikely (*s1 == limit_value))
      //      	return i+10; /* if the transactions are equals up to the prefix */
      continue; 
    }
    if(*s1 > *s2)
      return 1; 
    else
      return -1; 
  }
    
  if(s1 == s1end && s2 == s2end)
    return 0; 
  else
    if(s1 != s1end) 
      return 1; 
    else
      return -1; 
}


int set_member_index(const set_t &set, const element_t &e){
  int i = 0; 
  for(set_t::const_iterator it = set.begin(); it  != set.end(); ++it,++i){
    //    if(element_equal(*it, e))
    if(*it == e)
      return i; 
  }
  return -1; 
}

bool set_member_sorted(const set_t &sorted_set, const element_t &e){
  assert(is_sorted(sorted_set)); 
  for(set_t::const_iterator it = sorted_set.begin(); 
      it != sorted_set.end() && *it <= e; ++it){    
    if(*it == e)
      return true; 
  }
  return false; 
}

void set_truncate_above(set_t *s, element_t v){
  assert(is_sorted(*s)); 
  int idx = s->size() - 1; 
  while(idx >= 0 && (*s)[idx] > v)
    idx--; 
  s->resize(idx+1);
}


// set_t support_based_closure(TransactionTable &tt, const set_t &s){

//   Occurence oc;
//   set_t clo; 
//   set_t set(s); 
//   std::sort(set.begin(), set.end()); 
//   get_occurences_2d(tt, set, &oc); 

//   set_t dummy_set; 
//   for(int i = 0 ; i < oc.size(); i++)
//     dummy_set.push_back(oc[i]); 

//   for(int i = 0; i < ot.size(); i++){ 
//     if(get_set_presence_1d((ot)[i], dummy_set) == 1)
//       clo.push_back(i); //
//   }
//   return clo; 
// }

set_t support_based_closure(const set_t &set, int set_support, const SupportTable &support){
  set_t c(set); 
  for(int i = 0; i < support.size(); i++){
    if(support[i] == set_support)
      c.push_back(i);
  }
  return c; 
}

void set_insert_sorted(set_t *sorted_set, const element_t e){
  /* TODO improve efficiency */
  sorted_set->push_back(e); 
  set_t::iterator xlit = sorted_set->begin(); 
  while(xlit != sorted_set->end()){
    if(*xlit > e){
      std::swap(*xlit, (sorted_set->back())); 
    }
    ++xlit;
  }
 
}


void reverse_permutations(set_t *permutations){
  vector<bool> done(permutations->size(), false); 

  for(int i = 0; i < done.size(); i++){
    if(done[i] == false){
      int current_idx = i, current_value = (*permutations)[i];
      while(current_value != -1 && done[current_value] == false){
	  done[current_value] = true;	
	  int tmp = (*permutations)[current_value];
	  (*permutations)[current_value] = current_idx; 
	  current_idx = current_value;
	  current_value = tmp; 
      }      
    }     
  }  
}

#ifdef TIMING
using std::ofstream; 
static vector<ofstream *> trace_file_fds;
static struct timeval *start_time = NULL;


static void trace_check_file_exists(int tid){
      
  if(trace_file_fds.size() > tid && trace_file_fds[tid] != NULL){
    return; 
  }
  else{
    /* unlikely */
    trace_file_fds.resize(tid + 1, NULL); 
    std::ostringstream oss; 
    oss<<"TRACE_"<<tid<<".dat"; 
    trace_file_fds[tid] = new ofstream(oss.str().c_str()); 
  }
}
#endif //TIMING

void trace_init(int nb_threads){
#ifdef TIMING

  if(start_time == NULL){
    start_time = new struct timeval; 
    gettimeofday(start_time, NULL);
  }

  for(int i = 0; i <= nb_threads; i++)
    trace_check_file_exists(i);

#endif //TIMING 
}



void trace_timestamp_print(const string &info, int start_end){
#ifdef TIMING

  struct timeval tv;
  gettimeofday(&tv, NULL);
  
  int thread_id =  m_thread_id();

  ostream *os = trace_file_fds[thread_id];

  *os<<((tv.tv_sec + (double)tv.tv_usec/1000000.) - (start_time->tv_sec + (double) start_time->tv_usec/1000000.))*1000<<", "; 

  switch(start_end){
  case (EVENT_START):
    *os<<thread_id<<", "<<"START "<<info<<endl; 
    break; 
  case (EVENT_END):
   *os<<thread_id<<", "<<"END "<<info<<endl; 
    break;
  default:
    *os<<thread_id<<", "<<info<<endl; 
    break; 
  }

#endif //TIMING
}

void trace_exit(){
#ifdef TIMING

  for(int i = 0; i < trace_file_fds.size(); i++){
    delete trace_file_fds[i]; 
  }

#endif //TIMING 
}

void set_to_bit_representation(const set_t &set, element_t max_element, 
			       std::vector<bool> *set_bit){
  set_bit->resize(max_element+1,  false);
  for(set_t::const_iterator it = set.begin(); it != set.end(); ++it)
    if(*it <= max_element)
      (*set_bit)[*it] = true; 
}


bool set_permuted_limited_compare(const set_t &s1, const set_t &s2, 
				  const set_t &permutations, element_t limit){

    
  set_t::const_iterator end1 = s1.end();
  set_t::const_iterator end2 = s2.end(); 

  set_t::const_iterator it1 = s1.begin(), it2 = s2.begin();
  while(it1 != end1 && it2 != end2){
    element_t v1 = permutations[*it1]; 
    element_t v2 = permutations[*it2]; 
    if(v1 >= limit)
      {++it1; continue;}

    if(v2 >= limit)
      {++it2; continue;}

    if(v1 > v2)
      return false; 

    if(v1 < v2)
      return true;
    
    ++it1;++it2; 
  }
  
  for(;it1 != end1; ++it1)
    if(permutations[*it1] < limit)
      return false; /* s1 is larger */

  for(;it2 != end2; ++it2)
    if(permutations[*it2] < limit)
      return true; /* s2 is larger */

  return false; /* both are equal */
}


void elsort_transaction(Transaction *t, int max_element, const set_t &el){
  /* only for debug purposes, very inefficient */ 
  vector<bool> el_bit(max_element, false);
  for(set_t::const_iterator it = el.begin(); it != el.end(); ++it)
    el_bit[*it] = true; 

  Transaction::iterator last = t->end(); 
  Transaction::iterator it = t->begin(); 
  if(last == it) 
    return;
  last--; 
  while (it <= last){
    if(el_bit[*it]){
      iter_swap(it, last); 
      --last; 
    }
    else
      ++it; 
  }
  t->limit = t->size() - (t->end() - last)+1; 
  std::sort(t->begin(), t->begin()+t->limit); 
  std::sort(t->begin()+t->limit, t->end()); 
}
