// itemset_specific.cpp
// Made by Benjamin Negrevergne
// Started on  Fri Sep  3 14:09:56 2010
#include <sstream>
#include <fstream>
#include <set>
#include "database.hpp"
#include "utils.hpp"


using namespace std; 

void read_transaction_table(TransactionTable *tt, const char *filename){

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
      t.push_back(item); 
      ++nb_items; 
      ss>>item;
    }
    if(t.size() != 0){
      tt->push_back(t); 
      ++nb_trans; 
    }
  }

  cout<<"Data loaded, "<<nb_items<<" items within "<<nb_trans<<" transactions."<<endl;
  ifs.close();
}

void print_transaction(const Transaction &t){
  for(int i = 0; i < t.size(); i++){
    cout<<t[i]<<" "; 
  }
  cout<<endl;
}

void print_transaction_table(const TransactionTable &tt){
  for(int i = 0; i < tt.size(); i++){
    cout<<i<<" : "; print_transaction(tt[i]); 
  }
  cout<<endl; 
}


void transpose(const TransactionTable &tt, TransactionTable *ot){
  assert(!ot->size()); 
  for(int i = 0; i < tt.size(); i++)
    for(int j = 0; j < tt[i].size(); j++){
      int v = tt[i][j]; 
      if(v >= ot->size())
	ot->resize(v+1);
      (*ot)[v].push_back(i); 
    }
}

void database_build_reduced(TransactionTable *new_tt, const TransactionTable &tt,
			    const Transaction &occurence, const SupportTable &support){
  //TODO remove pattenr from db
  new_tt->reserve(occurence.size()); 
  new_tt->push_back(Transaction()); 
  Transaction *current_trans = &new_tt->back(); 
  for(Transaction::const_iterator occ_it = occurence.begin(); 
      occ_it != occurence.end(); ++occ_it){
    Transaction::const_iterator trans_it_end = tt[*occ_it].end(); 
    for(Transaction::const_iterator trans_it = tt[*occ_it].begin(); 
	trans_it != trans_it_end; ++trans_it){
      if(support[*trans_it] > 0){
	current_trans->push_back(*trans_it);
      }
    }
    if(current_trans->size() > 0){
      new_tt->push_back(Transaction()); 
      current_trans = &new_tt->back(); 
    }
  }

  if(current_trans->size() == 0){
    new_tt->resize(new_tt->size()-1);
  }
}


void database_build_reduced(TransactionTable *new_tt, const TransactionTable &tt,
			    const Transaction &occurence){
  //TODO remove pattenr from db
  new_tt->reserve(occurence.size()); 
  new_tt->push_back(Transaction()); 
  Transaction *current_trans = &new_tt->back(); 
  for(Transaction::const_iterator occ_it = occurence.begin(); 
      occ_it != occurence.end(); ++occ_it){
    new_tt->push_back(tt[*occ_it]);
  }
}


void database_occuring_elements(set_t *elements, 
			       const TransactionTable &tt, const Transaction &occurences){
  
  std::set<element_t> uniq_set; 
  for(Transaction::const_iterator occ_it = occurences.begin(); 
      occ_it != occurences.end(); ++occ_it){
    Transaction::const_iterator trans_end = tt[*occ_it].end(); 
    for(Transaction::const_iterator trans_it = tt[*occ_it].begin(); 
	trans_it != trans_end; ++trans_it)
      uniq_set.insert(*trans_it);
  }
  
  elements->reserve(uniq_set.size()); 
  for(std::set<element_t>::const_iterator set_it = uniq_set.begin(); set_it != uniq_set.end(); ++set_it){
    elements->push_back(*set_it);
  }
}

int get_set_presence_1d(const Transaction &t, const set_t &set){
  return is_included_1d(t, set); 
}

int is_included_1d(const Transaction &t, const set_t &set){
  assert(is_sorted(t)); 
  if(set.size()==0)
    return 1; 
  if(t.size() == 0)
    return 0; 
    
  //  assert(is_sorted(t));
  // if(!is_sorted(set))
  //   set_print(set); 
  assert(is_sorted(set)); 
  Transaction::const_iterator it = t.begin(); 
  set_t::const_iterator set_it = set.begin(); 

  do{
    if(*set_it == *it)
      if(++set_it == set.end())
	return 1;
    ++it; 
  }while(it != t.end());
 
  return 0; 
}

int count_inclusion_1d();

int count_inclusion_2d(const TransactionTable &tt, const set_t &set){
  int count  = 0; 
  for(TransactionTable::const_iterator it = tt.begin(); it != tt.end(); ++it){
    if(is_included_1d(*it, set))
      count++; 
  }
  return count; 
}

int count_inclusion_2d(const TransactionTable &tt, const Transaction &occs, const set_t &set){
  int count  = 0; 
  for(int i = 0; i < occs.size(); i++)
    if(is_included_1d(tt[occs[i]], set))
      count++; 

  return count; 
}


void get_occurences_2d(const TransactionTable &tt, const set_t &set, Occurence *oc){
  int i = 0;
  for(TransactionTable::const_iterator it = tt.begin(); it != tt.end(); ++it, ++i){
    if(get_set_presence_1d(*it, set))
      oc->push_back(i); 
  }
}


set_t canonical_form(set_t set){
  return set; 
}

element_t canonical_transform_element(const set_t &set, element_t &element){
  return element; 
}

set_t canonical_form(set_t set, element_t *element){
  return set; 
}


void compute_element_support(SupportTable *support, const TransactionTable &tt, const Transaction &occs){
  Transaction::const_iterator o_it_end = occs.end(); 
  for(Transaction::const_iterator o_it = occs.begin(); o_it != o_it_end; ++o_it){
    Transaction::const_iterator t_it_end = tt[*o_it].end(); 
    for(Transaction::const_iterator t_it = tt[*o_it].begin(); t_it != t_it_end; ++t_it){
      if(support->size() <= *t_it)
	support->resize(*t_it+1, 0); 
      (*support)[*t_it]++; 
    }
  }
} 



void all_occurences(Transaction *occs, const TransactionTable &tt){
  occs->resize(tt.size()); 
  for(int i = 0; i < tt.size(); i++){
    (*occs)[i] = i; 
  }
}
