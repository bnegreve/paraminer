// clogen_local_itemsets.cpp
// Made by Benjamin Negrevergne
// Started on  Tue Oct 19 18:44:38 2010
#include <cstdlib>
#include <algorithm>
#include <vector>
#include <sstream>
#include <fstream>
#include "clogen_local.hpp"
#define TRACK_TIDS
#include "pattern.hpp"
#include "database.hpp"
#include "clogen.hpp" 
#include "utils.hpp"

using std::cout; 
using std::cerr; 
using std::endl; 
using namespace std; 

int ELEMENT_RANGE_START = 0; 
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
  element_t max_element = 0; 
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
    ss>>item;
    while(ss.good()){
      if(item > max_element) {
	max_element=item;
	freq_table.resize(max_element+1, pair<int,int>(-1, 0));
      }
      ++nb_items; 
      freq_table[item].second++; 
      ss>>item;
    }
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
    ss>>item;
    while(ss.good()){
      element_t p; 
      if( (p = item) != -1)
	t.push_back(p);
      if(p > max_element) max_element=p; 
      ++nb_items; 
      ss>>item;
    }
    if(t.size() != 0){
      t.limit=t.size();
      t.tids.push_back(nb_trans); 
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
  //  merge_identical_transactions(tt); 
  return max_element; 
}



int membership_oracle(const set_t &base_set, const element_t extension, 
		      const membership_data_t &data){

  // Get tidlist of the pattern in occurences (with "new" tids)
  Occurence occurences;
  set_intersect(&occurences, data.base_set_occurences, data.extension_occurences);

  // Get original tids
  set_t orig_tids(tt.size());
  for(int i = 0; i < occurences.size(); i++){
    std::copy(data.tt[occurences[i]].tids.begin(),  data.tt[occurences[i]].tids.end(), orig_tids.begin());
  }
  
  if(orig_tids.size() < 2)
    return 0; 

  std::sort(orig_tids.begin(), orig_tids.end());
  int max_gap = orig_tids.back() - orig_tids.front(); 
  vector<int> gaps(max_gap, 0); 

  
  for(int i = 1; i < orig_tids.size(); i++){
    int current_gap = orig_tids[i] - orig_tids[i-1];
    gaps[current_gap]++; 
  }
  
  int big_gap = 0; 
  int big_gap_support = 0; 
  for(int i = 0; i < max_gap; i++){
    if(gaps[i] > big_gap_support){
      big_gap_support = gaps[i]; 
      big_gap = i ; 
    }
  }

  int return_value = big_gap * 1000 + big_gap_support;
  
  return big_gap_support>=threshold?big_gap:0; 
}

set_t clo(const set_t &set, int set_support, const SupportTable &support, const membership_data_t &data){  
  return support_based_closure(set, set_support, support); 
}


void usage(char *bin_name){
  cout<<bin_name<<" inputfile minsup [-t numthreads=1] [-c tuplecutoff=2]"<<endl;
  exit(EXIT_FAILURE); 
}


int main(int argc, char **argv){

  parse_clogen_arguments(&argc, argv); 
  if(argc != 3){
    usage(argv[0]); 
  }
  threshold = std::atoi(argv[2]); 
  
  //  compute_permutation_by_frequency(argv[1], &permutations, threshold); 
  element_t max = read_transaction_table(&tt, argv[1], permutations);




  ELEMENT_RANGE_END = max+1;

  print_transaction_table(tt);

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


  set_t empty_set; 
  int num_pattern = clogen(empty_set);
  cout<<num_pattern<<" patterns mined"<<endl;

}
