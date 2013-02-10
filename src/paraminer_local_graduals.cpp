/**
 * @file   paraminer_local_cgraphs.cpp
 * @author Benjamin Negrevergne <benjamin@neb.dyn.cs.kuleuven.be>
 * @date   Tue Oct 19 18:44:38 2010
 * 
 * @brief ParaMiner instance for mining closed frequent gradual itemsets (gri).
 * 
 * /!\ This is the gradual definition as it is defined in
 * GLCM. (Handle no variations as positive variations.)  For a more
 * advanced definition of graduals, check 
 * paraminer_local_graduals_gen.cpp
 * 
 */
#include <cstdlib>
#include <algorithm>
#include <vector>
#include <map>
#include <iostream>
#include <sstream>
#include <fstream>
#include <set>
#include <stdint.h>
#include <cmath>
#include <limits>

#define TRACK_TIDS

#include "paraminer_local.hpp"

#include "pattern.hpp"
#include "database.hpp"
#include "paraminer.hpp" 
#include "utils.hpp"
#include "bool_matrix.hpp"

//x//#include "BinaryMatrix.h"


//#define DETECT_NULL_VARIATION 1


using std::cout; 
using std::cerr; 
using std::endl; 
using namespace std; 
//x//using namespace nsPattern; 

int ELEMENT_RANGE_END; 


extern int threshold; 

/* Hack to efficiently compute the original pairs of tids */
typedef union{  
  uint16_t i16s[2]; 
  uint32_t i32; 
}two_short_t; 
#define first_ i16s[0] 
#define second_ i16s[1] 

typedef two_short_t trans_t;	/**< Virtual transaction descriptor
 type (virtual transactions are created for each pair of original
 transaction. This types holds the tids of the original transactions. */

typedef vector<trans_t> id_trans_t; 

typedef std::map<trans_t, id_trans_t> trans_id_t; 


int nb_attributes = 0;
int nb_initial_trans= 0; 
id_trans_t id_trans; 
trans_id_t trans_id; 
vector<int> vtrans_t;
int nb_vtrans = 0; 

std::vector<trans_t> tid_code_to_original_array; /**< Foreach virtual
 transaction id, holds the trans_t that contains the tid of the
 original transactions. */

std::vector<BoolMatrix> all_bms; /**< BoolMatrix are used to represent
 the variations between original transactions of each individual
 attribute.  Larger patterns can be built by performing AND operations
 between attribute matrix.
 */

trans_t tid_code_to_original(int code); 

/** 
 * \brief Fill the boolean matrix \bm from a set of \nb_trans
 * transaction pairs.
 *
 * Boolean variable bm[i, j] is set to one, when the variation
 *   between i and j supports the current pattern. 
 * @param bm The boolean matrix to fill. 
 * @param transaction pairs of original transactions (records). 
 * @param nb_trans number of original transactions (records). 
 */
void fill_bool_matrix(BoolMatrix *bm, const id_trans_t &transaction, int nb_trans)
{
  assert(nb_trans == bm->get_size()); 
  int col, row;
  for(int i = 0; i < transaction.size(); i++){
    /* if transaction in in thransction means t1 < t2 according to the item */
    col = transaction[i].first_; 
    row = transaction[i].second_;
    bm->set_value(col, row, 1); /* therefore we set BM[t2][t1] to 1*/
  }
}


void print_grad_transaction(const Transaction &t){
  for(int i = 0; i < t.size(); i++){
    element_print(t[i]);cout<<" " ;
  }
}

 void print_grad_transaction_table(const TransactionTable &tt){
  for(int i = 0; i < tt.size(); i++){
    trans_t t = tid_code_to_original(i); 
    cout<<i<<" "<<t.first_<<"x"<<t.second_<<" ("<<tt[i].weight<<") : "; print_grad_transaction(tt[i]); 
    cout<<endl;
  }
}

void read_transaction_table_vtrans(TransactionTable *tt, const char *filename){
  ifstream ifs (filename , ifstream::in);
  int nb_items = 0; 
  int t1 = 0, t2 = 1;
  ifs.ignore(65536, '\n');  /* skip first line */
  while (ifs.good()){
    
    string line; 
    stringstream ss; 
    Transaction t;
    double item; 
    ss<<skipws; 
    getline(ifs, line); 
    ss<<line; 
    ss>>item;
    t.reserve(nb_attributes);
    while(ss.good()){
      if(item >= (numeric_limits<int>::max())/1000){
	cerr<<"No value larger than "<<numeric_limits<int>::max()/1000<<" allowed in input file (See. "
	    <<__FILE__<<":"<<__LINE__<<")."<<endl;
	abort();
      }
      /* The item*1000 is a simple hack to it also works with float input values, remove if needed. (here and bellow) */
      t.push_back((int)(item*1000)); 
      ++nb_items;
      ss>>item;
    }
    if(!ss.fail()){
      t.push_back((int)(item*1000)); 
      ++nb_items;
      //   cout<<"READ "<<item<<endl; 
    }  
    if(nb_attributes==0)
      nb_attributes = nb_items;

    if(t.size() != 0){
      tt->push_back(t);
      nb_initial_trans++; 
    }

  }
  ifs.close();
}


int tt_to_grad_items(TransactionTable *output, const TransactionTable &input){
  output->reserve(input.size()*input.size()-input.size()); 
  int nb_trans = 0; 
  for(int i=0; i < input.size(); i++){
    for(int j = 0; j < input.size(); j++){
      if(i!=j){
	Transaction t;
	t.weight = 1; 
#ifdef TRACK_TIDS
	t.tids.push_back(nb_trans); 
#endif //TRACK_TIDS
	trans_t p; 
	p.first_=i; 
	p.second_=j; 
	tid_code_to_original_array.push_back(p);
	nb_trans++;



	t.reserve(nb_attributes); 
	for(int k = 0; k < input[i].size(); k++){
#ifdef DETECT_NULL_VARIATION
	  if(input[i][k] < input[j][k]){
	    t.push_back(2*k); 
	  }
	  else if(input[i][k] > input[j][k]){
	    t.push_back(2*k+1); 
	  }
	  else{
	    t.push_back(2*k); 
	    t.push_back(2*k+1); 
	  }
#else
	  if(input[i][k] < input[j][k]){
	    t.push_back(2*k); 
	  }
	  else if(input[i][k] > input[j][k]){
	    t.push_back(2*k+1); 
	  }
	  else {	    
	    t.push_back(2*k+(i>j)); 
	  }
#endif //DETECT_NULL_VARIATION
	}
	t.limit=t.size(); 
	output->push_back(t); 
      }
    }
  }
  return 0; 
}


/** 
 * \brief Convert the tid of a vtrans into a pair of original transaction tids. 
 * @param code the tid of the vtrans. 
 * @return The pair of original transaction tids. 
 */
trans_t tid_code_to_original(int code){
  return tid_code_to_original_array[code]; 
  trans_t t; 
  t.first_ = code / (nb_initial_trans-1); 
  t.second_ = code % (nb_initial_trans-1); 
  if(t.second_ >= t.first_)
    t.second_++; 
  return t; 
}

void element_print(const element_t element){
  cout<<" "<<(element/2)+1<<(element%2?"-":"+"); 
}

int support_from_path_lengths(const vector<int> &path_lengths){
  int sup = 0; 
  for(int i = 0; i < path_lengths.size(); i++){
    sup = std::max(sup, static_cast<int>(path_lengths[i])); 
  }
  return sup; 
}

void rec_compute_path_length(int trans, const BoolMatrix &BM, 
			     vector<int> *path_length){

  if((*path_length)[trans] != 0)
    return; 
  int BMSize = BM.get_size();
  if (!BM.null_row_p(trans)) {

    for(int j = 0; j < BMSize; j++)
      {
  	if (BM.get_value(trans,j))
  	  {
	   
	    //if (BM.checkZero(j)) freMap[trans] = 1;
	    if ((*path_length)[j] == 0) 
	      rec_compute_path_length(j,BM, path_length);

	    int current_path_length = (*path_length)[j] + 1;

	    if( current_path_length > (*path_length)[trans]){
	      (*path_length)[trans] = current_path_length; 	      
	    }
	  }	
      }
  }
  else{
    (*path_length)[trans] = 1; 
  }
}

/* Same as loop_find_longuest_paths() but only computes support value (do not store the paths)*/
int compute_gradual_support(const BoolMatrix &BM, vector<int> *path_length){
  int psize = path_length->size();
  for(int i = 0; i < psize; i++)
    {
      rec_compute_path_length(i,BM, path_length);
    }

  return support_from_path_lengths(*path_length); 
}


pair<int,int> retreive_transaction_pairs_count(const TransactionTable &tt, const Occurence &occurences, id_trans_t *transaction_pairs){

  
  vector<bool> present(nb_vtrans, false); 
  int nb_tids = 0;
  int max_tid = 0; 
  int nb_pairs = 0; 
  for(Occurence::const_iterator it = occurences.begin(); it != occurences.end(); ++it){    
    //    original_occurences[i++] = data.tt[*it].original_tid;
    const Transaction &cur = tt[*it]; 
    for(set_t::const_iterator it2 = cur.tids.begin(); it2 != cur.tids.end(); ++it2){
      trans_t t = tid_code_to_original(*it2); 
      if(!present[t.first_]){
	present[t.first_] = true; 
	nb_tids++; 
	if(t.first_>max_tid)
	  max_tid = t.first_; 
      }
      if(!present[t.second_]){
	present[t.second_] = true; 
	nb_tids++;
	if(t.second_>max_tid)
	  max_tid = t.second_; 
      }
      (*transaction_pairs)[nb_pairs++] = t; 
    }
    //cout<<i-1<<" : "<<transaction_pairs[i-1].first<<"x"<<transaction_pairs[i-1].second<<endl; 
  }
  return pair<int,int>(nb_tids, max_tid); 
}

void detect_short_cycles(const BoolMatrix &bm){
  for (int i = 0; i < bm.get_size(); i++)
    for (int j = 0; j < bm.get_size(); j++)
      if(i != j && bm.get_value(i,j) && bm.get_value(j,i)){
	cerr<<"SHORT CYCLE DETECTED "<<i<<"x"<<j<<endl; 
	abort(); 
      }
}

int membership_oracle(const set_t &base_set, const element_t extension, 
		      const membership_data_t &data){

  //  if(data.support[extension]+1 < threshold)
    //return 0; 

  set_t s(base_set); 
  s.push_back(extension);
  sort(s.begin(), s.end());

  if(s[0]%2==1) /* remove from F sets whose first element is a X- */
      return 0; 
  if(base_set.size() == 0)
    nb_vtrans;

  #if 0 
for(int i = 0; i < s.size()-1; i++){
    if(s[i]/2 == (s[i+1])/2){ 
      /* removes sets including X+ X- simultaneously */
      //cout<<"REMOVED : "<<endl; 
      //      set_print(s); 
      return 0; 
    }
  }
#endif 
    // if(s.size() == 1)
    //   return 1; 
  

  Occurence occurences;
  set_intersect(&occurences, data.base_set_occurences, data.extension_occurences);
  //  Occurence original_occurences(occurences.size()); 



  id_trans_t transaction_pairs(data.support[extension]);  

  int i=0;

  pair<int,int> nb_max_tids = retreive_transaction_pairs_count(data.tt, occurences, &transaction_pairs);


  // for(int i = 0; i < transaction_pairs.size(); i++){
  //   cout<<transaction_pairs[i].first_<<"x"<<transaction_pairs[i].second_<<endl; 
  // }
  
  BoolMatrix bm(nb_max_tids.first);

  /* Rename tids in the range [0, max_tids] */
  vector<int16_t> back_perms(nb_max_tids.second+1,-1);
  int perm_idx = 0;
  for(id_trans_t::const_iterator it = transaction_pairs.begin(); it != transaction_pairs.end(); ++it){
    int t1, t2;
    if( (t1 = back_perms[it->first_]) == -1){
      t1 = (back_perms[it->first_] = perm_idx++); 
    }
    if( (t2 = back_perms[it->second_]) == -1){
      t2 = (back_perms[it->second_] = perm_idx++); 
    }

    //    cout<<back_perms[it->first_]<<"->"<<t1<<endl; 
    //    cout<<back_perms[it->second_]<<"->"<<t2<<endl; 
    bm.set_value(t2,t1, true); 
  }


  //sort(transaction_pairs.begin(), transaction_pairs.end()); 

  
  //print transaction pairs supporting pattern






    //bm.fill_bool_matrixParaMiner(transaction_pairs, nb_vtrans);
  //  vector<int> path_length(nb_vtrans, 0);

  vector<int> path_length(nb_max_tids.first, 0);
#ifdef DETECT_NULL_VARIATION
  vector<vector<int> > siblings; 
  binary_matrix_remove_short_cycles(&bm, &siblings, nb_vtrans);
  int sup = compute_gradual_support_siblings(bm, siblings, &path_length); 
#else

#ifndef NDEBUG
  detect_short_cycles(bm);
#endif 
  int sup = compute_gradual_support(bm, &path_length); 
#endif
  
  /* Find longuest path (useless)*/
#if 0 
  vector<vector< int > > paths(nb_vtrans, vector<int>());
  loop_find_longest_paths(bm, siblings, &path_length, &paths);
  int sup = support_from_path_lengths(path_length);   
  // vector<int> path_length2(nb_vtrans, 0);   
  // assert( sup == sup2); 
#endif 


  
  // cout<<"PATHS"<<endl; 
  // for(int i = 0; i < paths.size(); i++){
  //   for(int j = 0; j < paths[i].size(); j++){
  //     cout<<paths[i][j]<<" "; 
  //   }
  //   cout<<endl; 
  // }
  // getchar(); 
  // vector<int> freMap(nb_vtrans, -1);   
  // //  vector<int> freMap(nb_vtrans, -1); 
  // set_t t_weights(nb_vtrans); 
  // for(int i = 0; i < siblings.size(); i++){
  //   t_weights[i] = siblings[i].size(); 
  // }

  // loop_Chk_Freq(bm, freMap, t_weights); 
  // int sup = frequentCount(freMap); 
  // cout<<"SUP"<<sup<<endl;


  //  int sup = get_longest_path(original_occurences);
  // cout<<"STARTTT"<<endl; 
  // set_print(base_set); 
  // element_print(extension); 
  // cout<<" SUP "<<sup<<endl;
  // set_print_raw(occurences);
  // cout<<"ENDDDDD"<<endl;


 
 // cout<<"TESTING ("<<sup<<endl; 
 // set_print(s); 
 // cout<<"DB"<<endl; 
 // print_grad_transaction_table(data.tt);   
 // cout<<"END"<<endl; 

  return sup>=threshold?sup:0;
}


set_t clo(const set_t &set, const closure_data_t &data){
  set_t c(set);
  //return set; 
  const Occurence &occs = data.occurences; 
  id_trans_t transaction_pairs(occs.size());
  

  
  /* Retreive original transaction ids from vtrans ids. */
  int i=0; 
  for(Occurence::const_iterator it = occs.begin(); it != occs.end(); ++it){    
    transaction_pairs[i++] = tid_code_to_original(data.tt[*it].tids[0]); 
  }

  /* Fill a bool matrix: [i, j] is set to one, when the variation
     between i and j supports the current pattern. */
  BoolMatrix bm(nb_vtrans);
  fill_bool_matrix(&bm, transaction_pairs, nb_vtrans);
  
  bool change = true; 

  bool first_positive_flag = false;
  bool discard_next = false; 
  
  for(int i = 0; i <= data.tt.max_element; i++){
    //  for(int i = 0; i < all_bms.size(); i++){    
    if(set_member(set, i)){
      if(i%2==0)
	first_positive_flag = true; 
    }
    else{
      if(data.support[i] < data.set_support)
	continue; 
    /* skip negative items before first positive item */ 
      if(i%2==1) 
	if(!first_positive_flag)
	  continue;    
      
      BoolMatrix bme(all_bms[i]); 
      //    restrict_binary_matrix(&bme, longuest_path_nodes); 
      bme.bitwise_and(bm); 
      if(bme == bm){
	c.push_back(i); 
	if(i%2==0)
	  first_positive_flag = true; 
      }
    }
  }

  return c; 
}


void usage(char *bin_name){
  paraminer_usage(bin_name);
  cerr<<"Problem specific command line arguments:"<<endl; 
  cerr<<bin_name<<" [<paraminer options> (See above.)] <dataset> <minsup>"<<endl;
  exit(EXIT_FAILURE); 
}


int main(int argc, char **argv){

  int idx = parse_paraminer_arguments(&argc, argv); 
  if(argc-idx != 3){
    usage(argv[0]); 
  }

  
  TransactionTable tmp; 
  read_transaction_table_vtrans(&tmp, argv[idx+1]);
  nb_vtrans = tmp.size(); 
  ELEMENT_RANGE_END = nb_attributes*2;
  
  cout<<"nb_attributes "<<nb_attributes<<endl;
  tt_to_grad_items(&tt, tmp); 
  cout<<nb_initial_trans<<endl;
  //  print_transaction_table(tt);
  
  //  print_grad_transaction_table(tt);   
  tt.max_element = ELEMENT_RANGE_END-1; 

#ifdef DATABASE_MERGE_TRANS
  merge_identical_transactions(&tt,true); 
#endif
  transpose(tt, &ot);

  all_bms.reserve(ELEMENT_RANGE_END); 
  for(int i = 0; i < ELEMENT_RANGE_END; i++){
    BoolMatrix bm(nb_vtrans);
    id_trans_t trans; 
    for(int j = 0; j < ot[i].size(); j++){
      trans.push_back(tid_code_to_original(ot[i][j]));
    }


   vector<vector<int> > siblings; 
   fill_bool_matrix(&bm, trans,nb_vtrans); 
  
  // vector<int> freMap(nb_vtrans, -1); 
  // set_t t_weights(nb_vtrans); 
  // for(int i = 0; i < siblings.size(); i++){
  //   t_weights[i] = siblings[i].size(); 
  // }

  //   fill_bool_matrixParaMiner(trans);
  //cout<<endl; 
    //     bm.PrintInfo();
    all_bms.push_back(bm); 
  }

  
  float f_threshold = atof(argv[idx+2]); 
  if(f_threshold < 1){ //TODO ambiguity when 1 !
    threshold = std::ceil(f_threshold*(nb_vtrans));
  }
  else{
    threshold = f_threshold;
    f_threshold = (float)threshold/(nb_vtrans);
  }
  cerr<<"THRESHOLD = "<<f_threshold<<" ["<<threshold<<" / "<<nb_vtrans<<"]"<<endl;

  set_t empty_set; 
  int num_pattern = paraminer(empty_set);
  cout<<num_pattern<<" patterns mined"<<endl;

};
