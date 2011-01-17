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
      t.limit=t.size(); 
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
}

/* remove elements in transaction that can never be the closure of any pattern in the given db
   This is element that belong to the exclusion list (ie. they are not possible extensions) 
   and don't belong to all the transactions with the same prefix
   Also sort the tids in tids (code partially from plcm)
*/
void remove_non_closed(TransactionTable &tt, Occurence *tids, 
		       int begin, int end){
#if 0 
  if(sort_only){
    tid_cmp cmp; 
    cmp.tt=&tt; 
    std::sort(tids->begin(), tids->end(), cmp); 
    return ;
  }
#endif 

  //TODO remplace with a call to std::sort
  //  cout<<"ITER "<<begin<<" "<<end<<endl;
  //  getchar();
  if( (end - begin) <= 1)
    return; 
  
  int intersect = 0; 
  //  Array<item_t> siTids; 
  
  /* Select the middle as a pivot */
  //  int pivotIdx = (end+begin)/2; 
  //  const Transaction &pivot = transactions[(*tids)[pivotIdx]];
  int pivotIdx = end-1; 
  const Transaction &pivot = tt[(*tids)[pivotIdx]];
  int pivot_limit = pivot.limit; 
  //  cout<<pivotIdx<<" "<<pivot<<endl;
  int storeIdx = begin; 
  for (int i = begin; i < end-1; ++i) {
    const tid_t currentTid = (*tids)[i];
    const Transaction &current_trans = tt[currentTid]; 
    //    cout<<"COMPARE "<<endl<<pivot<<endl<<"AND"<<endl<<transactions[currentTid]<<endl<<"RETURNED"<<pivot.lexicalGt(transactions[currentTid])<<endl;
    int cmp; 
    if((cmp = set_lexical_compare_limited(pivot.begin(), pivot_limit,
					  current_trans.begin(), 
					  current_trans.limit)) == 1){
      /* transaction is smaller than pivot, move its tid at the beginning of tids array */
      /* swap the tids */
      swap((*tids)[storeIdx], (*tids)[i]); 
      storeIdx++; 
    }
    else if (cmp == 0){
      /* Transactions are equal to the pivot, suffix intersection will be needed */ 
      intersect++; 
    }

  }
  swap((*tids)[storeIdx],   (*tids)[pivotIdx]);

  /* Sort first part */
  remove_non_closed(tt, tids, begin, storeIdx);
  
  /* Sort the second part and process to a SI on transaction equal to the pivot */
  if(intersect){
    /* Partition of second window */
    int storeIdx2 = storeIdx+1; 
    for(int i = storeIdx+1; i < end; i++){
      tid_t currentTid = (*tids)[i];
      const Transaction &current_trans = tt[currentTid]; 
      if(set_equal_limited(pivot, pivot_limit,
			   current_trans, 
			   current_trans.limit)){
	/* swap the tids */
	swap((*tids)[storeIdx2], (*tids)[i]); 
	storeIdx2++;       
      }
    }
    suffix_intersection(&tt, tids, storeIdx, storeIdx2);
    storeIdx = storeIdx2-1; 
  }
  
  /* sort third part */
  remove_non_closed(tt, tids, storeIdx+1, end);
}

/* Code from plcm */
void suffix_intersection(TransactionTable *tt, 
			vector<tid_t> *input,
			int begin, int end){
  
  if((end - begin)  < 1)
    return; 

  TransactionTable &transactions = *tt;
 
  //TODO : do not proceed to anything if one transaction is <= index (shold to a first pass) */

  tid_t refTransIndex = (*input)[begin];
  Transaction *refTrans = &(transactions)[refTransIndex];
  int limit = refTrans->limit; 
  vector<int> buckets(1); 
  bool flag = false; 
  buckets.resize(tt->max_element+1, 0); 
  for(int i = begin; i < end; ++i){
    tid_t currentTid = (*input)[i];
    Transaction &current_trans = (transactions)[currentTid]; 
    if(current_trans.size() <= limit){ //intersection is empty !
      flag = true;
      break;
    }    
    else{
      //TODO if one item is not present we can remove it (decrease MAXITEM ?)

      Transaction::const_iterator x = current_trans.begin()+current_trans.limit; 
      Transaction::const_iterator tEnd = current_trans.end(); 
      for(; x < tEnd; ++x)
	buckets[*x]++;
    }
  }

  /* Troncate the current refTrans at item */
  refTrans->resize(limit); 
  
  /* Add the intersection if non-empty */
  if(!flag)
    for(int i = 0 ; i <= tt->max_element; ++i){
      if(buckets[i] == (end-begin)) //if item is present in all transactions, 
	(*refTrans).push_back(i);    //it belongs to the intersection
    }
  
  /* Clears others transactions */ 

  
  int refTransWeight = 0; 
  for(int i = begin+1; i < end; ++i){
    Transaction &cur = transactions[(*input)[i]];
    refTransWeight += cur.weight;
#ifdef TRACK_TIDS
    refTrans->tids.insert(refTrans->tids.end(), cur.tids.begin(), cur.tids.end());
    cur.tids.clear(); 
#endif
      cur.clear();
  }
  refTrans->weight += refTransWeight; 
}

/* code from plcm */ 
void merge_identical_transactions(TransactionTable *tt, bool remove_non_closed_flag){
  //  cout<<"SIZE "<<tt->trans->size()<<" item "<<tt->item<<endl;
  int x = 0; 
  TransactionTable::iterator pTransactions = tt->begin(); 
  
  // cout<<"MERGIG "<<tt->max_element<<endl;
  //  print_transaction_table(*tt); 
  // getchar();
  
  Occurence sorted(tt->size()); 
  for(int i = 0; i < tt->size(); i++)
    sorted[i] = i ;

  if(remove_non_closed_flag)
    /* sort tids, remove non-closed elements and merge eq trans */
    remove_non_closed(*tt, &sorted, 0, sorted.size()); 
  else{
    /* sort & merge only */
    quick_sort_tids(*tt, &sorted, 0, sorted.size());

    Occurence::const_iterator refTid = sorted.begin(); //sort transactions, lexical order
    Occurence::const_iterator end = sorted.end(); 
    for(Occurence::const_iterator currentTid = refTid+1; currentTid < end; ++currentTid){
      if(set_equal((*tt)[*currentTid], (*tt)[*refTid])){
	/* Merge the transactions */
	Transaction &ref = (*tt)[*refTid];
	Transaction &cur = (*tt)[*currentTid]; 
	ref.weight += cur.weight; 
	cur.clear();
#ifdef TRACK_TIDS
	ref.tids.insert(ref.tids.end(), cur.tids.begin(), cur.tids.end());
	cur.tids.clear(); 
#endif //TRACK_TIDS
	x++; 
      }
      else{
	refTid=currentTid;
      }
    }
 
  }
  //  removeEmptyTransactions(tt);

}
/*** END mergeIdenticalTransactions ***/  

void database_build_reduced(TransactionTable *new_tt, const TransactionTable &tt,
			    const Transaction &occurence, const SupportTable &support, 
			    const set_t &exclusion_list, bool merge){
  new_tt->max_element=0; 
  new_tt->reserve(occurence.size()); 
  new_tt->push_back(Transaction());
  Transaction *current_trans = &new_tt->back(); 

  Transaction buffer; 
  buffer.reserve(exclusion_list.size());
  
  for(Transaction::const_iterator occ_it = occurence.begin(); 
      occ_it != occurence.end(); ++occ_it){
    
    current_trans->original_tid = tt[*occ_it].original_tid;
    current_trans->weight =  tt[*occ_it].weight;
    Transaction::const_iterator limit = tt[*occ_it].begin() + tt[*occ_it].limit; 
    set_t::const_iterator xlit = exclusion_list.begin(); 
#ifdef TRACK_TIDS
    current_trans->tids = tt[*occ_it].tids;
#endif //TRACK_TIDS
    Transaction::const_iterator trans_it_end = tt[*occ_it].end();
    for(Transaction::const_iterator trans_it = tt[*occ_it].begin();	
	trans_it != trans_it_end; ++trans_it){
      if(support[*trans_it] > 0){
#ifdef REMOVE_NON_CLOSED
	/* keep element in exclusion list at the end of transactions */
	if(trans_it >= limit)
	  buffer.push_back(*trans_it); 
	else{
	  bool found = false; 
	  while(xlit != exclusion_list.end()){
	    if(*xlit < *trans_it)
	      ++xlit;
	    else if (*xlit == *trans_it){
	      ++xlit; 
	      found = true; 
	      break; 
	    }
	    else 
	      break; 
	  }

	  if(found){
	    buffer.push_back(*trans_it); 
	  }
	  else{
	    current_trans->push_back(*trans_it);
	  }
	}
#else
	current_trans->push_back(*trans_it);
#endif //REMOVE_NON_CLOSED
      }
    }
    
    current_trans->limit = current_trans->size(); 
    current_trans->insert(current_trans->end(), buffer.begin(), buffer.end()); 
    buffer.clear();
    
    if(current_trans->size() > 0){
      if(current_trans->limit != 0)
	new_tt->max_element = std::max(new_tt->max_element, (*current_trans)[current_trans->limit-1]);
      new_tt->max_element = std::max(new_tt->max_element, current_trans->back());
      new_tt->push_back(Transaction()); 
      current_trans = &new_tt->back(); 
    }
  }

  if(current_trans->size() == 0){
    new_tt->resize(new_tt->size()-1);
  }

  if(merge){
#ifdef DATABASE_MERGE_TRANS
#ifdef REMOVE_NON_CLOSED
    merge_identical_transactions(new_tt, true);
#else
    merge_identical_transactions(new_tt, false);
#endif //REMOVE NON CLOSED
#endif //DATABASE_MERGE_TRANS  
  }

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

  Transaction::const_iterator o_it_end = occs.end(); 
  for(Transaction::const_iterator o_it = occs.begin(); o_it != o_it_end; ++o_it){
    int t_weight = tt[*o_it].weight; 
    Transaction::const_iterator t_it_end = tt[*o_it].end();
    for(Transaction::const_iterator t_it = tt[*o_it].begin(); t_it != t_it_end; ++t_it){
      //      if(support->size() <= *t_it)
      //	support->resize(*t_it+1, 0); 
      (*support)[*t_it]+=t_weight; 
    }
  }
} 



void all_occurences(Transaction *occs, const TransactionTable &tt){
  occs->resize(tt.size()); 
  for(int i = 0; i < tt.size(); i++){
    (*occs)[i] = i; 
  }
}
