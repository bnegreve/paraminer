/**
 * @file   paraminer_local_cgraphs.cpp
 * @author Benjamin Negrevergne <benjamin@neb.dyn.cs.kuleuven.be>
 * @date   Tue Oct 19 18:44:38 2010
 * 
 * @brief ParaMiner instance for mining closed frequent itemsets (fim)
 * 
 */
#include <cstdlib>
#include <algorithm>
#include <vector>
#include <sstream>
#include <fstream>

#include "paraminer_local.hpp"

#include "pattern.hpp"
#include "database.hpp"
#include "paraminer.hpp" 
#include "utils.hpp"

using std::cout; 
using std::cerr; 
using std::endl; 
using namespace std; 


#define PERMUTE_ITEMS

int ELEMENT_RANGE_END; 

extern int threshold;

using namespace std;

set_t permutations; 

void element_print(const element_t element){
  cout<<permutations[element]; 
}


typedef struct{
  bool operator()(const pair<int,int> &a, const pair<int,int> &b){
    return a.second < b.second; 
  }
}pair_gt_second_t;

int compute_permutation_by_frequency(const char *filename, set_t *permutations, int minsup){
  
  vector<pair<int, int> >freq_table; 
  element_t max_element = -1; 
  ifstream ifs (filename , ifstream::in);
  int nb_items = 0; 
  int nb_trans = 0;
  while (ifs.good()){
    string line; 
    stringstream ss; 
    int item; 
    ss<<skipws; 
    getline(ifs, line); 
    ss<<line; 
    do{
      ss>>item;
      if(!ss.fail()){
	if(item > max_element) {
	  max_element=item;
	  freq_table.resize(max_element+1, pair<int,int>(-1, 0));
	}
	++nb_items; 
	freq_table[item].second++; 
      }
    }while(ss.good()); 
  }

  for(int i = 0; i < freq_table.size(); i++){
    freq_table[i].first = i; 
  }
  /* Compute permutations */ 
  pair_gt_second_t gt; 
  sort(freq_table.begin(), freq_table.end(),gt); 

  permutations->resize(max_element+1); 
  
  int item_index = 0; 
  for(int i = 0; i < freq_table.size(); i++){
    if(freq_table[i].second >= minsup)
      (*permutations)[freq_table[i].first] = item_index++;
    else
      (*permutations)[freq_table[i].first] = -1;
  }

  // for(int i = 0 ; i < permutations->size(); i++){
  //   cout<<i<<" -> "<<(*permutations)[i]<<endl; 
  // }
  
#ifdef PERMUTE_ITEMS

#else
  /* reset pertutations (dirty) */ 
  for(int i = 0; i < permutations->size(); i++){
    (*permutations)[i] = i; 
  }
#endif 
  return item_index; 
}


element_t read_transaction_table(TransactionTable *tt, const char *filename, const set_t &permutations){
  element_t max_element = 0; 
  ifstream ifs (filename , ifstream::in);
  int nb_items = 0; 
  int nb_trans = 0;
  while (ifs.good()){
    string line; 
    stringstream ss; 
    Transaction t;
    int item; 
    ss<<skipws; 
    getline(ifs, line); 
    ss<<line; 

    do{
      ss>>item;
      if(!ss.fail()){
	element_t p; 
	if( (p = permutations[item]) != -1)
	  t.push_back(p);
	if(p > max_element) max_element=p; 
	++nb_items; 
      }
    }while(ss.good()); 

    if(t.size() != 0){
      t.limit=t.size();
#ifdef TRACK_TIDS
      if(show_tids)
	t.tids.push_back(nb_trans);
#endif //TRACK_TIDS
      nb_trans++;
      t.weight = 1;
#ifdef SORT_DATABASE
      sort(t.begin(), t.end());
#else
#ifndef NDEBUG
      assert(is_sorted(t)); 
#endif //
#endif //SORT_DATABASE

      tt->push_back(t); 
    }   
  }

  cout<<"Data loaded, "<<nb_items<<" items within "<<nb_trans<<" transactions."<<endl;
  ifs.close();

  tt->max_element = max_element;
  merge_identical_transactions(tt); 
  return max_element; 
}



int membership_oracle(const set_t &base_set, const element_t extension, 
		      const membership_data_t &data){
  return data.support[extension] >= threshold ? data.support[extension] : 0; 
}

set_t clo(const set_t &set, const closure_data_t &data){
  return support_based_closure(set, data.set_support, data.support); 
}


void usage(char *bin_name){
  paraminer_usage(bin_name);
  cerr<<"Problem specific command line arguments:"<<endl; 
  cerr<<bin_name<<" [<paraminer_options> (See above.)] <dataset> <minsup>"<<endl;
  exit(EXIT_FAILURE); 
}


int main(int argc, char **argv){

  parse_paraminer_arguments(&argc, argv); 
  if(argc != 3){
    usage(argv[0]); 
  }
  threshold = std::atoi(argv[2]); 
  

  compute_permutation_by_frequency(argv[1], &permutations, threshold); 
  element_t max = read_transaction_table(&tt, argv[1], permutations);
  

  set_t permutations2(permutations);

  reverse_permutations(&permutations); 


  ELEMENT_RANGE_END = max+1;

  // print_transaction_table(tt);

  // Occurence tids; 
  // for(int i = 0; i < tt.size(); i++){
  //   tids.push_back(i); 
  // }
  // quick_sort_tids(tt, &tids, 0, tids.size());
  // set_print(tids);
  // merge_identical_transactions(&tt); 
  // print_transaction_table(tt); 
  // exit(1); 


  transpose(tt, &ot);

  trace_init(num_threads);
  
  set_t empty_set; 
  int num_pattern = paraminer(empty_set);
  cout<<num_pattern<<" patterns mined"<<endl;

}
