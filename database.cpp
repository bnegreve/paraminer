// itemset_specific.cpp
// Made by Benjamin Negrevergne
// Started on  Fri Sep  3 14:09:56 2010
#include <sstream>
#include <fstream>
#include <set>
#include <algorithm>
#include "database.hpp"
#include "utils.hpp"


using namespace std; 

element_t read_transaction_table(TransactionTable *tt, const char *filename){
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
      t.push_back(item);
      if(item > max_element) max_element=item; 
      ++nb_items; 
      ss>>item;
    }
    if(t.size() != 0){
      t.original_tid = nb_trans++;
      t.weight = 1;
      tt->push_back(t); 
    }
  }

  cout<<"Data loaded, "<<nb_items<<" items within "<<nb_trans<<" transactions."<<endl;
  ifs.close();

  tt->max_element = max_element; 
  return max_element; 
}

void print_transaction(const Transaction &t){
  cout<<"("<<t.weight<<") "; 
  for(int i = 0; i < t.size(); i++){
    cout<<t[i]<<" "; 
  }
  cout<<endl;
}

void print_transaction_table(const TransactionTable &tt){
  for(int i = 0; i < tt.size(); i++){
    cout<<i<<" : "; print_transaction(tt[i]); 
  }

}


void transpose(const TransactionTable &tt, TransactionTable *ot){
  assert(!ot->size());
  ot->resize(tt.max_element+1);
  for(int i = 0; i < tt.size(); i++)
    for(int j = 0; j < tt[i].size(); j++){
      int v = tt[i][j]; 
      (*ot)[v].push_back(i); 
    }
}

void data_base_shrink(TransactionTable *tt){
  
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

struct tid_cmp{
  const TransactionTable *tt; 
  bool operator()(const int t1, const int t2){
    return (set_lexical_compare((*tt)[t1], (*tt)[t2]) ==1 ); 
  }
}; 

/* code from plcm */ 
void quick_sort_tids( const TransactionTable &tt, Occurence *tids, 
		      int begin, int end){
  tid_cmp cmp; 
  cmp.tt=&tt; 
  std::sort(tids->begin(), tids->end(), cmp); 
  return ; 
  //TODO remplace with a call to std::sort
  //  cout<<"ITER "<<begin<<" "<<end<<endl;
  //  getchar();
  if( (end - begin) <= 1)
    return; 

  int siIdx=-1; 
  //  Array<item_t> siTids; 

  /* Select the middle as a pivot */
  //  int pivotIdx = (end+begin)/2; 
  //  const Transaction &pivot = transactions[(*tids)[pivotIdx]];
  int pivotIdx = end-1; 
  const Transaction &pivot = tt[(*tids)[pivotIdx]];
  //  cout<<pivotIdx<<" "<<pivot<<endl;
  int storeIdx = begin; 
  for (int i = begin; i < end-1; ++i) {
    const tid_t currentTid = (*tids)[i];
    //    cout<<"COMPARE "<<endl<<pivot<<endl<<"AND"<<endl<<transactions[currentTid]<<endl<<"RETURNED"<<pivot.lexicalGt(transactions[currentTid])<<endl;
    int cmp; 
    if((cmp = set_lexical_compare(pivot, tt[currentTid])) == 1){
      /* swap the tids */
      tid_t tmp = (*tids)[storeIdx];
      (*tids)[storeIdx++] = currentTid; 
      (*tids)[i] = tmp; 
    }
  }
  tid_t tmp = (*tids)[storeIdx];
  (*tids)[storeIdx] = (*tids)[pivotIdx];
  (*tids)[pivotIdx] = tmp; 

  quick_sort_tids(tt, tids, begin, storeIdx);   
  quick_sort_tids(tt, tids, storeIdx+1, end);   
}


/* code from plcm */ 
void merge_identical_transactions(TransactionTable *tt){
  //  cout<<"SIZE "<<tt->trans->size()<<" item "<<tt->item<<endl;
  int x = 0; 
  TransactionTable::iterator pTransactions = tt->begin(); 
  
  // cout<<"MERGIG "<<tt->max_element<<endl;
  //  print_transaction_table(*tt); 
  // getchar();
  
  Occurence sorted(tt->size()); 
  for(int i = 0; i < tt->size(); i++)
    sorted[i] = i ;
  
  quick_sort_tids(*tt, &sorted, 0, sorted.size()); 

  Occurence::const_iterator refTid = sorted.begin(); //sort transactions, lexical order
  Occurence::const_iterator end = sorted.end(); 
  for(Occurence::const_iterator currentTid = refTid+1; currentTid < end; ++currentTid){
    if(set_equal((*tt)[*currentTid], (*tt)[*refTid])){
      (*tt)[*refTid].weight += (*tt)[*currentTid].weight; 
      (*tt)[*currentTid].clear(); 
      x++; 
    }
    else{
      refTid=currentTid;
    }
  }
  //  removeEmptyTransactions(tt);

}
/*** END mergeIdenticalTransactions ***/  


void database_build_reduced(TransactionTable *new_tt, const TransactionTable &tt,
			    const Transaction &occurence, const SupportTable &support, 
			    const set_t &exclusion_list){
  new_tt->max_element=0; 
  new_tt->reserve(occurence.size()); 
  new_tt->push_back(Transaction()); 
  Transaction *current_trans = &new_tt->back(); 
  for(Transaction::const_iterator occ_it = occurence.begin(); 
      occ_it != occurence.end(); ++occ_it){
    current_trans->original_tid = tt[*occ_it].original_tid;
    current_trans->weight =  tt[*occ_it].weight;
    Transaction::const_iterator trans_it_end = tt[*occ_it].end(); 
    for(Transaction::const_iterator trans_it = tt[*occ_it].begin(); 
	trans_it != trans_it_end; ++trans_it){
      if(support[*trans_it] > 0 /*&& !set_member(exclusion_list, *trans_it)*/){
	current_trans->push_back(*trans_it);
	new_tt->max_element = std::max(new_tt->max_element, *trans_it); 
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

#ifdef DATABASE_MERGE_TRANS
  merge_identical_transactions(new_tt);
#endif //DATABASE_MERGE_TRANS  

}


void database_build_reduced(TransactionTable *new_tt, const TransactionTable &tt,
			    const Transaction &occurence){
  assert(false);  
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


void compute_element_support(SupportTable *support, const TransactionTable &tt,
			     const Occurence &occs){

  support->resize(tt.max_element+1); 
  Transaction::const_iterator o_it_end = occs.end(); 
  for(Transaction::const_iterator o_it = occs.begin(); o_it != o_it_end; ++o_it){
    int t_weight = tt[*o_it].weight; 
    Transaction::const_iterator t_it_end = tt[*o_it].end();
    for(Transaction::const_iterator t_it = tt[*o_it].begin(); t_it != t_it_end; ++t_it){
      //      if(support->size() <= *t_it)
      //	support->resize(*t_it+1, 0); 
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
