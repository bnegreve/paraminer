// clogen_local_itemsets.cpp
// Made by Benjamin Negrevergne
// Started on  Tue Oct 19 18:44:38 2010
#include <cstdlib>
#include <algorithm>
#include <vector>
#include "clogen_local.hpp"

#include "pattern.hpp"
#include "database.hpp"
#include "clogen.hpp" 
#include "utils.hpp"

using std::cout; 
using std::cerr; 
using std::endl; 
using namespace std; 

const int ELEMENT_RANGE_START = 0; 
const int ELEMENT_RANGE_END = 120; 

extern int threshold; 

using namespace std;

int membership_oracle(const set_t &set){
  int freq = count_inclusion_2d(tt, set);
  if(freq >= threshold)
    return freq; 
  return 0; 
}

int membership_oracle(const set_t &set, const TransactionTable &tt,
		      const Transaction &occurences){
  return set_is_frequent_in_occurences(set, tt, occurences, threshold); 
}

set_t clo(const set_t &s){
  Occurence oc;
  set_t clo; 
  set_t set(s); 
  sort(set.begin(), set.end()); 
  get_occurences_2d(tt, set, &oc); 

  set_t dummy_set; 
  for(int i = 0 ; i < oc.size(); i++)
    dummy_set.push_back(oc[i]); 

  for(int i = 0; i < ot.size(); i++){ 
    if(get_set_presence_1d((ot)[i], dummy_set) == 1)
      clo.push_back(i); //
  }
  return clo; 
}

set_t clo(const set_t &set, const Transaction &occurences){

  set_t clo; 
  vector<int> freq; 
  Transaction::const_iterator t_it_end = occurences.end(); 
  for(Transaction::const_iterator t_it = occurences.begin(); t_it != t_it_end; ++t_it){
    if(*t_it >= freq.size())
      freq.resize(*t_it); 
    freq[*t_it]++; 
  }
  
  for(int i = 0; i < freq.size(); i++){
    if(freq[i] == occurences.size()){
      clo.push_back(i); 
    }
  }
  return clo; 
}

set_t clo(const set_t &set, int set_support, const SupportTable &support){
  return support_based_closure(set, set_support, support); 
}


void usage(char *bin_name){
  cout<<bin_name<<" inputfile minsup [-t numthreads=1] [-c tuplecutoff=2]"<<endl;
  exit(EXIT_FAILURE); 
}


int main(int argc, char **argv){

  int idx = parse_clogen_arguments(&argc, argv); 
  if(argc-idx != 2){
    usage(argv[0]); 
  }

  read_transaction_table(&tt, argv[idx]); 
  transpose(tt, &ot);
  threshold = std::atoi(argv[idx+1]); 

  set_t empty_set; 
  int num_pattern = clogen(empty_set);
  cout<<num_pattern<<" patterns mined"<<endl;

}
