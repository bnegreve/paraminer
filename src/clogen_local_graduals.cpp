// clogen_local_itemsets.cpp
// Made by Benjamin Negrevergne
// Started on  Tue Oct 19 18:44:38 2010
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

#define TRACK_TIDS

#include "clogen_local.hpp"

#include "pattern.hpp"
#include "database.hpp"
#include "clogen.hpp" 
#include "utils.hpp"

#include "BinaryMatrix.h"

//#define DETECT_NULL_VARIATION 1


using std::cout; 
using std::cerr; 
using std::endl; 
using namespace std; 
using namespace nsPattern; 

int ELEMENT_RANGE_END; 


extern int threshold; 


typedef two_short_t trans_t;
typedef vector<trans_t> id_trans_t; 
typedef std::map<trans_t, id_trans_t> trans_id_t; 


int nb_attributes = 0;
int nb_initial_trans= 0; 
id_trans_t id_trans; 
trans_id_t trans_id; 
vector<int> vtrans_t;
int nb_vtrans = 0; 

std::vector<trans_t> tid_code_to_original_array;

std::vector<BinaryMatrix> all_bms; 


trans_t tid_code_to_original(int code); 


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
    float item; 
    ss<<skipws; 
    getline(ifs, line); 
    ss<<line; 
    ss>>item;
    t.reserve(nb_attributes);
    while(ss.good()){
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

/* convert a tid code and return the original pair of transactions */ 
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

int get_path_length(int current, vector< pair < trans_t, int> > &t){
  /* TODO force return when reaching a path length greater than the current minimum */
  //  cout<<"CALL ON "<< t[current].first.first<<" "<<t[current].first.second<<endl; 
  /* ((t1, t2), path) */
  if(t[current].second != 0){
    //    cout<<"return "<<t[current].second<<endl;
    return t[current].second; /* return pre-computed value of the longest path from current */ 
  }


  /* recursively computes the longest path */
  t[current].second = 1; 
  for(int i = 0; i < t.size(); i++){
    if(t[i].first.first_ == t[current].first.second_){
      //      cout <<"YE "<< t[i].first.first<<" "<<t[i].first.second<<endl; 
      int path_len = 1 + get_path_length(i, t); 
      if(path_len > t[current].second)
	t[current].second = path_len; 	
    }
    else{
      //      cout <<"NO "<< t[i].first.first<<" "<<t[i].first.second<<endl; 
    }
  }

  //  cout<<"return "<<t[current].second<<endl;
  return t[current].second ; 
}

int get_longest_path(const Occurence &occs){
  
  if(occs.size() == 0) 
    return 0; 

  vector< pair< trans_t, int> > t; 

  for(int i = 0; i < occs.size(); i++){
    t.push_back(pair<trans_t, int>(tid_code_to_original(occs[i]), 0)); 
  }

  int longest_path = 0; 
  for(int i = 0; i < occs.size(); i++){
    int path = get_path_length(i, t) + 1; 
    if(path > longest_path)
      longest_path = path;
  }

  return longest_path;
}




void binary_matrix_remove_short_cycles(BinaryMatrix *bm, vector<vector<int> > *sibling, int nb_trans){
  /* Here we build a matrix without cycles by merging transactions
     that are equal according to pattern (theses transactions are the
     one responsable of the cycles.  We keep trac of the merged
     transactions inside siblings. It must be concidered we computing
     the frequency since when two transactions are equal it is always
     possible to loop once and include in the supporting path all the
     equal transactions instead of just one*/
  sibling->resize(nb_trans);
  for(int i = 0; i < nb_trans; i++)
    (*sibling)[i].push_back(i); 
  for(int i = 0; i < nb_trans; i++){
    for(int j = 0; j < nb_trans; j++){
      if(i != j)
	if(bm->getValue(i, j) == 1){
	  if(bm->getValue(j, i) == 1){
	    /* sibling transactions */
	    /* remove j from bm and use report all conections into i */
	    for(int jj = 0; jj < nb_trans; jj++)
	      //TODO remove from here 
	      // if(bm->getValue(j, jj)){
	      // 	if( i != jj){
	      // 	  assert(bm->getValue(i, jj)); /* TODO remove */
	      // 	  /* repporting connections is useless unless this assert fails */
	      // 	  bm->setValue(i, jj, 1); 
	      // 	}
	      //TO HERE unless the assert fails 
	      bm->setValue(j, jj, 0);
		  //	      }
	  
	    for(int ii = 0; ii < nb_trans; ii++){
	      bm->setValue(ii, j, 0);
	    }
	    (*sibling)[i].push_back(j);
	    (*sibling)[j].clear();
	  }
	}
    }
  }
}


/* function from GLCM */
void  recursion_Chk_Freq(int trans, BinaryMatrix BM, vector<int> & freMap, 
			 const set_t &t_weights)
{
  	// int BMSize = BM.getSize();
	// if (BM.checkZero(trans)) freMap[trans] = 1;
	// 	for(int j = 0; j < BMSize; j++)
	// 	{
	// 		if (BM.getValue(j,trans))
	// 		{
	// 			//if (BM.checkZero(j)) freMap[trans] = 1;
	// 			if (freMap[j] == -1) recursion_Chk_Freq(j,BM,freMap);
	// 			freMap[trans] = (freMap[trans]>freMap[j] + 1)?freMap[trans]:(freMap[j] + 1);
	// 		}
	// 	}
  int BMSize = BM.getSize();
  if (BM.checkZero(trans)) freMap[trans] = 1;
  else{
    for(int j = 0; j < BMSize; j++)
      {
  	if (BM.getValue(j,trans))
  	  {
	   
	    //if (BM.checkZero(j)) freMap[trans] = 1;
	    if (freMap[j] == -1) 
	      recursion_Chk_Freq(j,BM,freMap, t_weights);
	  }	
      }
    for(int j = 0; j < BMSize; j++){
      int nb_siblings = t_weights[j]; 
      if (BM.getValue(j,trans))
	  freMap[trans] = 
	    (freMap[trans]>freMap[j] + nb_siblings)?freMap[trans]:(freMap[j] + nb_siblings);
    }
  }
}

void  recursion_find_path(int trans, const BinaryMatrix &BM, 
			  const vector<vector<int> > &siblings,
			  vector<int> *path_length,
			  vector<vector <int> > *path)
{

  /* TODO use a set instead of a vector, there is no node to keep the correct order */
  if((*path_length)[trans] != 0)
    return; 
  int nb_siblings = siblings[trans].size(); 
  int BMSize = BM.getSize();
  if (!BM.checkZero(trans)) {

    for(int j = 0; j < BMSize; j++)
      {
  	if (BM.getValue(j,trans))
  	  {
	   
	    //if (BM.checkZero(j)) freMap[trans] = 1;
	    if ((*path_length)[j] == 0) 
	      recursion_find_path(j,BM,siblings, path_length, path);

	    int current_path_length = (*path_length)[j] + nb_siblings; 

	    if( current_path_length > (*path_length)[trans]){
	      (*path_length)[trans] = current_path_length; 
		/* new path is greater */
		//	      copy((*path)[j].begin(), (*path)[j].end(), (*path)[trans].end());
		vector<int> &current_path = (*path)[trans]; 
		current_path.clear();
		current_path.insert(current_path.begin(), siblings[trans].begin(), siblings[trans].end()); 
		current_path.insert(current_path.end(), 
				    (*path)[j].begin(), (*path)[j].end()); 
	      
	    }
	    else if (current_path_length == (*path_length)[trans]){
	      /* The to path are the same length. Keep track of all the
		 node belonging to any potential longuest path*/
	      vector<int> &current_path = (*path)[trans];
	      current_path.insert(current_path.begin(), siblings[trans].begin(), siblings[trans].end()); 
	      current_path.insert(current_path.end(), //TODO pas besoin d'ajouter les fere ils y sont deja
				  (*path)[j].begin(), (*path)[j].end()); 
	      
	    }
	  }	
      }

  }
  
  else{
    (*path_length)[trans] = nb_siblings; 
    (*path)[trans].insert((*path)[trans].begin(), siblings[trans].begin(), siblings[trans].end());
  
  }
  // for(int i = 0; i < nb_siblings; i++){
  //   (*path)[trans].push_back(siblings[trans][i]); 
  // }
 
}

/* Computes longuest path in a binary matrix */
void loop_find_longest_paths(const BinaryMatrix &BM,
			     const vector<vector<int> > &siblings,
			     vector<int> *path_length,
			     vector<vector <int> > *path)
{
  int psize = path->size();
  for(int i = 0; i < psize; i++)
    {
      recursion_find_path(i,BM, siblings, path_length, path);
    }
}

int support_from_path_lengths(const vector<int> &path_lengths){
  int sup = 0; 
  for(int i = 0; i < path_lengths.size(); i++){
    sup = std::max(sup, static_cast<int>(path_lengths[i])); 
  }
  return sup; 
}

void rec_compute_path_length_siblings(int trans, const BinaryMatrix &BM, 
			     const vector<vector<int> > &siblings,
			     vector<int> *path_length){

  if((*path_length)[trans] != 0)
    return; 
  int nb_siblings = siblings[trans].size(); 
  int BMSize = BM.getSize();
  if (!BM.checkZero(trans)) {

    for(int j = 0; j < BMSize; j++)
      {
  	if (BM.getValue(j,trans))
  	  {
	   
	    //if (BM.checkZero(j)) freMap[trans] = 1;
	    if ((*path_length)[j] == 0) 
	      rec_compute_path_length_siblings(j,BM,siblings, path_length);

	    int current_path_length = (*path_length)[j] + nb_siblings; 

	    if( current_path_length > (*path_length)[trans]){
	      (*path_length)[trans] = current_path_length; 	      
	    }
	  }	
      }
  }
  else{
    (*path_length)[trans] = nb_siblings; 
  }
}

void rec_compute_path_length(int trans, const BinaryMatrix &BM, 
			     vector<int> *path_length){

  if((*path_length)[trans] != 0)
    return; 
  int BMSize = BM.getSize();
  if (!BM.checkZero(trans)) {

    for(int j = 0; j < BMSize; j++)
      {
  	if (BM.getValue(j,trans))
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
int compute_gradual_support_siblings(const BinaryMatrix &BM, const vector<vector<int> > &siblings,
			     vector<int> *path_length){
  int psize = path_length->size();
  for(int i = 0; i < psize; i++)
    {
      rec_compute_path_length_siblings(i,BM, siblings, path_length);
    }

  return support_from_path_lengths(*path_length); 
}

/* Same as loop_find_longuest_paths() but only computes support value (do not store the paths)*/
int compute_gradual_support(const BinaryMatrix &BM, vector<int> *path_length){
  int psize = path_length->size();
  for(int i = 0; i < psize; i++)
    {
      rec_compute_path_length(i,BM, path_length);
    }

  return support_from_path_lengths(*path_length); 
}



int frequentCount(vector<int> & freMap)
{
  int maxFreq = 0;
  int freMapSize = freMap.size();
  for(int i = 0; i < freMapSize; i++)
    {
      if (freMap[i] > maxFreq) maxFreq = freMap[i]; 
    }
  return maxFreq;
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


void retreive_transaction_pairs(const TransactionTable &tt, const Occurence &occurences, id_trans_t *transaction_pairs){
  
  for(Occurence::const_iterator it = occurences.begin(); it != occurences.end(); ++it){    
    //    original_occurences[i++] = data.tt[*it].original_tid;
    const Transaction &cur = tt[*it]; 
    for(set_t::const_iterator it2 = cur.tids.begin(); it2 != cur.tids.end(); ++it2){      
      transaction_pairs->push_back(tid_code_to_original(*it2)); 
    }
    //cout<<i-1<<" : "<<transaction_pairs[i-1].first<<"x"<<transaction_pairs[i-1].second<<endl; 
  }
}


void detect_short_cycles(const BinaryMatrix &bm){
  for (int i = 0; i < bm.getSize(); i++)
    for (int j = 0; j < bm.getSize(); j++)
      if(i != j && bm.getValue(i,j) && bm.getValue(j,i)){
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
  
  BinaryMatrix bm(nb_max_tids.first);

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
    bm.setValue(t1,t2, true); 
  }


  //sort(transaction_pairs.begin(), transaction_pairs.end()); 

  
  //print transaction pairs supporting pattern






    //bm.constructBinaryMatrixClogen(transaction_pairs, nb_vtrans);
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


void reduceBM(BinaryMatrix & BM, int threshold, bool & change)
{
  int maxTrans = 0;
  int maxCount = 0;
  //if (change) return;
  maxTrans = BM.getMaxTrans(maxCount);
  if (maxCount < (threshold-1)) 
    {
      BM.setBMtoZero();	
      return;
    }
  BM.reduce_nonSupport_TS(threshold,change);
  while (change) 
    {		
      BM.reduce_nonSupport_TS(threshold,change);
      //cout << "....." << change << endl;
    }
  //reduceBM(BM, threshold,change);
}


set_t calF(BinaryMatrix * BM, const vector<BinaryMatrix> & vBM, int & resFre, vector<vector <int> > &siblings)
{

  set_t is;
  is.clear();
  //TransactionSequence ts;
  int BMSize = BM->getSize();
  BinaryMatrix calFBM(BMSize);
  calFBM = (*BM);
  vector<int> freMap(BMSize, -1);

  set_t t_weights(nb_vtrans); 
  for(int i = 0; i < siblings.size(); i++){
    t_weights[i] = siblings[i].size(); 
  }
  //loop_Chk_Freq(calFBM,freMap, t_weights);
  resFre = frequentCount(freMap);
  if (resFre < threshold) return is;

  bool change = false;
  reduceBM(calFBM, resFre, change);
  //bool bcf = CheckFrequent(BM, _threshold, freType, ts);
  //if (!bcf) return is;


  int G1ItemSize = ELEMENT_RANGE_END; 
  for (int i = 0; i < G1ItemSize; i++)
    {
      //if (vBM[i]->isInclude(BM)) is.addItem(i);
      BinaryMatrix tBM = vBM[i];
      tBM &= (*BM);
      if (tBM == (*BM)) {
	is.push_back(i); 
      }
    }
  return is;
}


///////////////////////////////////////////////////////////////////
///////////////// CAL G  //////////////////////////////////////////
///////////////////////////////////////////////////////////////////
/* Computes the transactions quequences supporting itemset \is */
// void calG(set_t is, const vector<BinaryMatrix *> & vBM, BinaryMatrix& res)
// {
//   int isSize = is.size();
//   if (isSize == 0)
//     return;

//   res = *vBM[is[0]];

//   for (int i = 1; i < isSize; i++)
//     res &= *vBM[is[i]];
// }

void extract_longuest_path_nodes(vector<int> *nodes, const vector<int> &path_lengths, 
				 const vector<vector <int> > &paths){
  /* find longuest path */
  int max = support_from_path_lengths(path_lengths);

  std::set<int> node_set; 
  
  for(int i = 0; i < paths.size(); i++){
    if(path_lengths[i] == max){
      node_set.insert(paths[i].begin(), paths[i].end()); 
    }
  }

  nodes->reserve(node_set.size());
  nodes->insert(nodes->end(), node_set.begin(), node_set.end()); 
  
  //  copy(nodes->begin(), node_set.begin(), node_set.end()); 
}

void restrict_binary_matrix(BinaryMatrix *bm, const vector<int> &nodes){
  vector<int> keep(bm->getSize(), 0); 
  for(int i = 0; i < nodes.size(); i++){
    keep[nodes[i]] = 1; 
  }

  for(int i = 0; i < keep.size(); i++)
    if(!keep[i])
      bm->resetIndex(i); 
}

set_t clo(const set_t &set, const closure_data_t &data){
  set_t c(set);
  //return set; 
  const Occurence &occs = data.occurences; 
  id_trans_t transaction_pairs(occs.size());
  
  int i=0; 
  for(Occurence::const_iterator it = occs.begin(); it != occs.end(); ++it){    
    //    original_occs[i++] = data.tt[*it].original_tid; 
    transaction_pairs[i++] = tid_code_to_original(data.tt[*it].tids[0]); 
  }
  //  sort(transaction_pairs.begin(), transaction_pairs.end()); 

  BinaryMatrix bm(nb_vtrans);

  
  bm.constructBinaryMatrixClogen(transaction_pairs, nb_vtrans);
  
  bool change = true; 

  /* TODO REDUCTION FROM DOT, REMOVE.*/
  //  reduceBM(bm, threshold, change); 

  /* TODO Retreive paths from membership computation */
  //  binary_matrix_remove_short_cycles(&bm_no_cycles, &siblings, nb_vtrans); 

  //  cout<<"BINARY MATRIX FOR : "<<endl;
  //  set_print(s); 
  //  bm.PrintInfo(); 


  //  vector<vector< int > > paths(nb_vtrans, vector<int>());
  //  vector<int> path_lengths(nb_vtrans, 0); 
  //  loop_find_longest_paths(bm_no_cycles, siblings, &path_lengths, &paths);

  //  vector<int> longuest_path_nodes; 
  //  extract_longuest_path_nodes(&longuest_path_nodes, path_lengths, paths); 
  //restrict_binary_matrix(&bm, longuest_path_nodes);


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
      
      BinaryMatrix bme(all_bms[i]); 
      //    restrict_binary_matrix(&bme, longuest_path_nodes); 
      bme &= bm; 
      if(bme == bm){
	c.push_back(i); 
	if(i%2==0)
	  first_positive_flag = true; 
      }
    }
  }

  // if(set != c){

  //   cout<<"FERMETURE DE "<<endl; 
  //   set_print(set); 
  //   cout<<" EST : "<<endl; 
  //   set_print(c); 
  //   cout<<"FIN"<<endl; 
  // }
  return c; 
}


void usage(char *bin_name){
  cout<<bin_name<<" inputfile minsup [-t numthreads=1] [-c tuplecutoff=2]"<<endl;
  exit(EXIT_FAILURE); 
}


int main(int argc, char **argv){

  int idx = parse_clogen_arguments(&argc, argv); 
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
    BinaryMatrix bm(nb_vtrans);
    id_trans_t trans; 
    for(int j = 0; j < ot[i].size(); j++){
      trans.push_back(tid_code_to_original(ot[i][j]));
    }


   vector<vector<int> > siblings; 
  bm.constructBinaryMatrixClogen(trans,nb_vtrans); 
  
  // vector<int> freMap(nb_vtrans, -1); 
  // set_t t_weights(nb_vtrans); 
  // for(int i = 0; i < siblings.size(); i++){
  //   t_weights[i] = siblings[i].size(); 
  // }

  //   bm.constructBinaryMatrixClogen(trans);
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
  int num_pattern = clogen(empty_set);
  cout<<num_pattern<<" patterns mined"<<endl;

}
