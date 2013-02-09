/*
** paraminer.cpp
** 
** Made by (Benjamin Negrevergne)
** Email   [firstname].[name]@imag.fr
** 
** Started on  Thu Sep  2 10:21:29 2010 Benjamin Negrevergne
*/
#include <iostream>
#include <algorithm>
#include <map>
#include <cstdio>
#include <cctype>
#include <cmath>
#include <pthread.h> 
#include <unistd.h>
#include <limits>
#include "paraminer_local.hpp"
#include "utils.hpp"
#include "element.hpp"
#include "pattern.hpp"
#include "database.hpp"


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

float max_reduction_threshold = 10; 	/**< \brief Threshold to
	 control activation of el-reduction on level 0 calls to
	 expand. (-y switch.)

	 On level 0 expand calls, el-reduction is activated by
	 default. The reductions are performed unless the maximum
	 reduction factor is garanteed to be below
	 max_reduction_factor_threshold. */
float min_reduction_threshold = 2; /**< \brief Threshold to control
	 activationi of el-reduction on level 1 and above. (-x switch.)
	 
	 On level 1 and above, el-reduction is deactivated by
	 default. The el-reductions are activated if the minimum
	 reduction factor is garanteed to be above
	 min_reduction_factor_threshold.
 */

#endif //PARALLEL_PROCESS

using std::cout; 
using std::endl;
using std::cerr; 
TransactionTable &tt = *new TransactionTable;
TransactionTable &ot = *new TransactionTable; 

int threshold; 

bool show_tids = false ;

static std::map<TransactionTable *, int> nb_refs_map; 
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; 
void set_nb_refs(TransactionTable *p, int nb_refs){
  pthread_mutex_lock(&mutex);
  nb_refs_map[p] = nb_refs; 
  pthread_mutex_unlock(&mutex); 
}

void increase_nb_refs(TransactionTable *p, int nb_refs){
  pthread_mutex_lock(&mutex);
  nb_refs_map[p] += nb_refs;
  pthread_mutex_unlock(&mutex); 
}

int decrease_nb_refs(TransactionTable *p){
  int tmp; 
  pthread_mutex_lock(&mutex);
  assert(nb_refs_map[p] > 0);
  tmp = (nb_refs_map[p]-=1);
  assert(nb_refs_map[p] >= 0); 
  pthread_mutex_unlock(&mutex);
  return tmp; 
}

void release_shared_memory(TransactionTable *tt, TransactionTable *ot){
  if(decrease_nb_refs(tt) == 0){
    delete tt;
    delete ot;
  }
}

struct support_sort_cmp_t{
  const TransactionTable &ot;
  bool operator()(int a, int b) {
    return ot[a].size() < ot[b].size(); 
  }
};
  
/** 
 * Returns true if parent pattern is the first parrent of \pattern
 * according to exclusion list.  Note that given the exclusion list,
 * parent_pattern is not needed to compute this result.
 * 
 */
bool parent_pattern_is_first_parent(const set_t &pattern, const set_t &exclusion_list){
  assert(is_sorted(pattern));
  assert(is_sorted(exclusion_list)); 
  /* Check if one element from closed set belong to the exclusion list */ 
  set_t::const_iterator xlit = exclusion_list.begin();
  const set_t::const_iterator xlend = exclusion_list.end();
  set_t::const_iterator cit = pattern.begin();  
  const set_t::const_iterator cend = pattern.end();  
  while(xlit != xlend && cit != cend){
    if(*xlit < *cit)
      ++xlit;
    else if (*xlit > *cit)
      ++cit; 
    else{
      return false; 
    }
  }
  return true; 
}


void expand_async(TransactionTable &tt, TransactionTable &ot, const Transaction &occs, 
		  const set_t &parent_pattern, element_t pattern_augmentation, 
		  int depth, const set_t &exclusion_list, const set_t &exclusion_list_tail, 
		  int membership_retval, bool shared_tt){
  tuple_t tuple;
  tuple.tt = &tt; 
  tuple.ot = &ot; 
  tuple.occs = new Transaction(occs); 
  tuple.s = new set_t(parent_pattern); 
  tuple.e = pattern_augmentation; 
  tuple.depth = depth; 
  tuple.exclusion_list = new set_t(exclusion_list); 
  tuple.exclusion_list_tail = new set_t(exclusion_list_tail); 
  tuple.u_data = membership_retval; 
  tuple.shared_tt = shared_tt; 
  m_tuplespace_put(&ts, (opaque_tuple_t*)&tuple, 1);
}


size_t expand(TransactionTable &tt, TransactionTable &ot, const Transaction &occs,  
	      const set_t &parent_pattern, element_t pattern_augmentation, 
	      int depth, const set_t &parent_el, const set_t &el_tail,
	      int membership_retval, bool shared_tt){


  set_t exclusion_list(parent_el.size()+el_tail.size()); 
  copy(parent_el.begin(), parent_el.end(), exclusion_list.begin()); 
  copy(el_tail.begin(), el_tail.end(), exclusion_list.begin()+parent_el.size()); 
  assert(is_sorted(parent_el)); 
  sort(exclusion_list.begin(), exclusion_list.end()); //TODO Very inefficient
  
  set_t pattern(parent_pattern); 
  element_t max_element = tt.max_element;
  
  SupportTable support(max_element+1, 0);  
  compute_element_support(&support, tt, occs); 
  pattern.push_back(pattern_augmentation);
  int set_support=support[pattern_augmentation];
  for(set_t::const_iterator it = pattern.begin(); it != pattern.end(); ++it){
    if(*it <= max_element)
      support[*it] = 0; 
  }

  /***************************/
  /* Compute pattern closure */
  /***************************/
  closure_data_t c_data = {tt, occs, support, set_support};
  set_t closed_pattern = clo(pattern, c_data);
  sort(closed_pattern.begin(), closed_pattern.end()); 
  //TODO could be improved, 
  for(set_t::const_iterator it = closed_pattern.begin(); it != closed_pattern.end(); ++it){
    if(*it <= tt.max_element) 
      support[*it] = 0; //By doing this we remove from db the elements that already blong to the set.
  }

  /******************************************************************/
  /* Check if parent pattern is the first parent of closed_pattern. */
  /******************************************************************/
  if(!parent_pattern_is_first_parent(closed_pattern, exclusion_list)){
    /* If it isn't release the memory and abort the current branch exploration. */
    if(shared_tt) release_shared_memory(&tt, &ot); 
    return 0; 
  }
      

  /*******************/
  /* Output pattern. */
  /*******************/
#ifdef TRACK_TIDS
  // Get original tids
  set_t orig_tids;
  if (show_tids) {
    orig_tids.reserve(occs.size()); 
    for(int i = 0; i < occs.size(); i++){
      orig_tids.insert(orig_tids.end(), tt[occs[i]].tids.begin(),  tt[occs[i]].tids.end());
    }
    std::sort(orig_tids.begin(), orig_tids.end());
  }
  pattern_print(closed_pattern,membership_retval, orig_tids); 
#else
  set_t *orig_tids; //dummy set, won't be read
  pattern_print(closed_pattern,membership_retval, *orig_tids); 
#endif //TRACK_TIDS
  size_t num_pattern = 1; 
 


  /*************************************************/
  /* Computes current closed pattern augmentations */
  /*************************************************/
  set_t augmentations;

  /* to store the value returned by membership foreach element. (Will
     be passed to the rec. call.) */
  set_t augmentations_membership_retval(max_element+1); 

  for(int i = 0; i <= max_element; i++){
    if(support[i] != 0){ /* i occurs in the dataset */
      membership_data_t m_data = {tt, occs, ot[i], support};    
      // TODO Release OT here

      if(!set_member(exclusion_list, i))
	if( (augmentations_membership_retval[i] = 
	     membership_oracle(closed_pattern, i, m_data))){
	  /* Closed_pattern U i is a pattern hence i is an augmentation. */
	  augmentations.push_back(i);
	}
    }
  }
  
  if(augmentations.size() > 0){
    
    if(depth == 0)
      trace_timestamp_print("DBR", EVENT_START ); 
			  
    set_t new_exclusion_list;
    set_t new_el_tail;
    TransactionTable *new_tt;
    TransactionTable *new_ot;
    bool new_shared_tt;

    /* Perform an async call ? */ 
    bool async_call = depth < depth_tuple_cutoff;

    /* Perform el-reduction ? */ 
    bool el_reduce; 
    if(depth <= 0){
      /* On level 0 expand calls, el-reduction is activated by
	 default. The reductions are performed unless the maximum
	 reduction factor is garanteed to be below
	 max_reduction_factor_threshold. */

      int max_reduction_factor = 
	el_tail.size()>31?(1<<(el_tail.size())):std::numeric_limits<int>::max();
      el_reduce = max_reduction_factor >= max_reduction_threshold;
    }
    else{
      /* On level 1 and above, el-reduction is deactivated by
	 default. The el-reductions are activated if the minimum
	 reduction factor is garanteed to be above
	 min_reduction_factor_threshold. */

      int not_el_size = (tt.max_element+1-exclusion_list.size());
      int min_reduction_factor = not_el_size<31?(occs.size()/(1<<not_el_size)):1;
      el_reduce = min_reduction_factor >= min_reduction_threshold; 
    }
							       
    new_el_tail.reserve(el_tail.size() +  augmentations.size());
    if(el_reduce){
      /********************************/
      /* Creates new reduced dataset. */
      /********************************/
      new_tt = new TransactionTable;
      new_ot = new TransactionTable;

      database_build_reduced2(new_tt, tt, occs, 
			      closed_pattern, exclusion_list, depth, el_reduce);
      set_truncate_above(&exclusion_list, new_tt->max_element); 
      
      /* Deal with memory management (parent tt). */
      if(shared_tt)	/* If the parent tt was shared, release it if necessary */ 
	release_shared_memory(&tt, &ot); 

      transpose(*new_tt, new_ot);

      /* Deal with memory management (new tt). */
      if(async_call){
	/* If we are going to proceed to an async call to expand, the
	   dataset will be thread-shared. */
	new_shared_tt = true; 
	set_nb_refs(new_tt, augmentations.size()); 
      }
      else{
	/* Otherwise (recursive call to expand) the dataset won't be shared. */
	new_shared_tt = false;
      }

      new_exclusion_list = exclusion_list;
    }
    else{
      /**************************/
      /* Reuses parent dataset. */
      /**************************/
      new_tt = &tt;
      new_ot = &ot;

      /* Deal with memory management (new tt = parent tt). */
      new_shared_tt = shared_tt; /* shared iff parent is shared. */
      if(new_shared_tt)	increase_nb_refs(&tt, augmentations.size()-1);

      /* Re-compute the new tids (this can be improved) */
      new_exclusion_list = parent_el; 
      new_el_tail = el_tail; 
    }

    support_sort_cmp_t support_sort = {*new_ot};      
    std::sort(augmentations.begin(), augmentations.end(), support_sort); 

    if(async_call){
      /********************************/
      /* Asynchronous call to expand. */
      /********************************/
      set_t::const_iterator c_it_end = augmentations.end(); 
      for(set_t::const_iterator c_it = augmentations.begin(); c_it != c_it_end; ++c_it){
	assert(!(set_member(exclusion_list, *c_it)));
	Transaction new_occs;
	if(!el_reduce)
	  set_intersect(&new_occs, occs, (*new_ot)[*c_it]);
	else
	  new_occs = (*new_ot)[*c_it];
	expand_async(*new_tt, *new_ot, new_occs, closed_pattern,
		     *c_it, depth+1, new_exclusion_list, new_el_tail,
		     augmentations_membership_retval[*c_it], new_shared_tt);
	new_el_tail.push_back(*c_it); 
      }
    }
    else{
      /*****************************/
      /* Recursive call to expand. */
      /*****************************/
      set_t::const_iterator c_it_end = augmentations.end(); 
      for(set_t::const_iterator c_it = augmentations.begin(); c_it != c_it_end; ++c_it){
	Transaction new_occs; 
	if(!el_reduce)
	  set_intersect(&new_occs, occs, (*new_ot)[*c_it]);
	else
	  new_occs = (*new_ot)[*c_it];
	num_pattern += expand(*new_tt, *new_ot, new_occs, closed_pattern, 
			      *c_it, depth+1, new_exclusion_list, new_el_tail, 
			      augmentations_membership_retval[*c_it], new_shared_tt);
	/* insert the current augmentation into the exclusion list for the next calls.*/
	new_el_tail.push_back(*c_it); 
      }

      if(el_reduce && !new_shared_tt){
	/* if this recursive call created a dataset that is not thread-shared.*/
	delete new_tt;
	delete new_ot;
      }
    }
  }
  
  else{
    /* this is a tail call, free thread-shared memory */
    if(shared_tt) release_shared_memory(&tt, &ot); 
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

    num_patterns += expand(*tuple.tt, *tuple.ot, *tuple.occs, *tuple.s, tuple.e, 
			   tuple.depth, *tuple.exclusion_list, 
			   *tuple.exclusion_list_tail, tuple.u_data, tuple.shared_tt);
    //    num_patterns += x; 
    delete tuple.s;
    delete tuple.occs; 
    delete tuple.exclusion_list; 
    delete tuple.exclusion_list_tail;
  }
  return reinterpret_cast<void*>(num_patterns); 
}


#endif //PARALLEL_PROCESS

void paraminer_usage(char *bin_name, bool exit){
  cerr<<"ParaMiner general usage (valid for every instance of paraminer):"<<endl;
  cerr<<bin_name<<" [-t <numthreads> = 1] [-c <tuplecutoff> = 2] "<<endl
      <<"[-l (Display tidlist (default is off).)] "<<endl
      <<"[-y <level 0 reduction factor threshold> = 10 (See code doc for details.)] "<<endl
      <<"[-x <level 1 and above reduction factor threshold> = 2 (See code doc for details.)] "<<endl

      <<"<problem specific command line arg 1> .. <problem specific command line arg n>"<<endl<<endl;
  
  if(exit) std::exit(EXIT_FAILURE); 
}

int parse_paraminer_arguments(int *argc, char **argv){
  char opt_char=0;
  opterr = 0; 
  int local_options = 0; 
  while ((opt_char = getopt(*argc, argv, "c:x:y:t:l")) != -1)
    {
      switch(opt_char)
	{
	case 'c':
	  if(optarg==NULL || !isdigit(*optarg)) paraminer_usage(argv[0], true);
	  depth_tuple_cutoff = atoi(optarg);
	  *optarg = '\0';
	  cout<<"depth cutoff set to "<<depth_tuple_cutoff<<"."<<endl;
	  break ;
	case 'y':
	  if(optarg==NULL || !isdigit(*optarg)) paraminer_usage(argv[0], true);
	  max_reduction_threshold = atof(optarg);
	  cout<<"max reduction factor threshold set to "<<max_reduction_threshold<<"."<<endl;
	  *optarg = '\0';
	  break; 
	case 'x':
	  if(optarg==NULL || !isdigit(*optarg)) paraminer_usage(argv[0], true);
	  min_reduction_threshold = atof(optarg);
	  cout<<"min reduction factor threshold set to "<<min_reduction_threshold<<"."<<endl;
	  *optarg = '\0';
	  break; 
	case 't':
	  if(optarg==NULL || !isdigit(*optarg)) paraminer_usage(argv[0], true);
	  num_threads = atoi(optarg);
	  *optarg = '\0'; 
	  break;
	case 'l':
	  show_tids = true ;
	  break ;
	default:
	  break;	  
	}
    }

  /* remove options from argv .. */
  for(int i = 0; i < *argc; i++){
    if(!strncmp(argv[i], "-t",2)){
      *(argv[i]) = '\0'; 
    }
    else if (!strncmp(argv[i], "-c",2)){
      *(argv[i]) = '\0'; 
    }
    else if (!strncmp(argv[i], "-x",2)){
      *(argv[i]) = '\0'; 
    }
    else if (!strncmp(argv[i], "-y",2)){
      *(argv[i]) = '\0'; 
    }
    else if (!strncmp(argv[i], "-l",2)){
      *(argv[i]) = '\0' ;
    }
  }

  int idx = 0; 
  for(int i = 0; i < *argc; i++){
    if(*(argv[i]) != '\0'){
      argv[idx++] = argv[i]; 
    }
  }

  *argc = idx; 

  int tmp = optind; 
  optind = 0; 
  opterr = 1;   
  cout<<"running with "<<num_threads<<" thread(s)."<<endl;
  return optind;
}

int paraminer(set_t initial_pattern){

  int num_pattern = 0; 
#ifndef NDEBUG
  cout<<"PARAMINER DEBUG"<<endl;
#endif

#ifdef PARALLEL_PROCESS

 
  m_tuplespace_init(&ts, sizeof(tuple_t), 0, TUPLESPACE_OPTIONAUTOCLOSE); 
  m_thread_register(); 

  set_t empty_set;
  set_t exclusion_list;
  set_t exclusion_list_tail;

  Occurence all_tids(tt.size()); 
  for(int i = 0; i < tt.size(); i++)
    all_tids[i]=i; 
  SupportTable support(tt.max_element+1, 0);  
  compute_element_support(&support, tt, all_tids);

  set_t augmentations; 
  augmentations.reserve(tt.max_element+1); 
  set_t augmentations_membership_retvals(tt.max_element+1); 

  for(element_t  current = element_first(); 
      current != element_null; current = element_next(current)){
    membership_data_t m_data = {tt,all_tids,ot[current],support};
    int new_membership_retval; 
    if( (new_membership_retval = membership_oracle(empty_set,current, m_data)) ){
      augmentations.push_back(current); 
      augmentations_membership_retvals[current] = new_membership_retval; 
    }
  }

  set_nb_refs(&tt, augmentations.size());
  for(set_t::const_iterator augmentation = augmentations.begin();
      augmentation != augmentations.end(); ++augmentation){
    expand_async(tt, ot, ot[*augmentation], empty_set, 
		 *augmentation, 0, exclusion_list,
		 exclusion_list_tail, 
		 augmentations_membership_retvals[*augmentation], true); 
    set_insert_sorted(&exclusion_list_tail, *augmentation); 
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
