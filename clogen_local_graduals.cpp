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
#include "clogen_local.hpp"

#include "pattern.hpp"
#include "database.hpp"
#include "clogen.hpp" 
#include "utils.hpp"
#include "BinaryMatrix.h"

using std::cout; 
using std::cerr; 
using std::endl; 
using namespace std; 
using namespace nsPattern; 
int ELEMENT_RANGE_START = 0; 
int ELEMENT_RANGE_END; 


extern int threshold; 



typedef pair<int, int> trans_t;
typedef vector<trans_t> id_trans_t; 
typedef std::map<trans_t, id_trans_t> trans_id_t; 


int nb_attributes = 0;
int nb_initial_trans= 0; 
id_trans_t id_trans; 
trans_id_t trans_id; 
vector<int> vtrans_t;
int nb_vtrans = 0; 

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
    cout<<t.first<<"x"<<t.second<<" : "; print_grad_transaction(tt[i]); 
    cout<<endl;
  }
}

void read_transaction_table_vtrans(TransactionTable *tt, const char *filename){
  ifstream ifs (filename , ifstream::in);
  int nb_items = 0; 
  int t1 = 0, t2 = 1;
  ifs.ignore(256, '\n');  /* skipe first line */
  while (ifs.good()){
    
    string line; 
    stringstream ss; 
    Transaction t;
    int item; 
    ss<<skipws; 
    getline(ifs, line); 
    ss<<line; 
    ss>>item;
    t.reserve(nb_attributes);
    while(ss.good()){
      t.push_back(item); 
      ++nb_items;
      ss>>item;
    }
    if(!ss.fail()){
      t.push_back(item); 
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
	t.original_tid=nb_trans++; 
	t.reserve(nb_attributes); 
	for(int k = 0; k < input[i].size(); k++){
	  if(input[i][k] < input[j][k]){
	    t.push_back(2*k+1); 
	  }
	  else if(input[i][k] > input[j][k]){
	    t.push_back(2*k); 
	  }
	  else{
	    	    t.push_back(2*k);
		    t.push_back(2*k+1); 
	  }
	}
	output->push_back(t); 
      }
    }
  }
  return 0; 
}

trans_t tid_code_to_original(int code){
  trans_t t; 
  t.first = code / (nb_initial_trans-1); 
  t.second = code % (nb_initial_trans-1); 
  if(t.second >= t.first)
    t.second++; 
  return t; 
}

void element_print(const element_t element){
  cout<<" "<<(element/2)+1<<(element%2?"+":"-"); 
}

int get_path_length(int current, vector< pair < pair <int, int>, int> > &t){
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
    if(t[i].first.first == t[current].first.second){
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

  vector< pair< pair<int, int>, int> > t; 

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


void set_intersect(set_t *out, const set_t &t1, const set_t &t2){
  assert(is_sorted(t1) && is_sorted(t2));
  out->resize(std::min(t1.size(), t2.size()));
  Transaction::iterator it;
  it=set_intersection (t1.begin(), t1.end(), t2.begin(), t2.end(), out->begin());
  
  out->resize(it-out->begin());

}
/* function from GLCM */
void recursion_Chk_Freq(int trans, BinaryMatrix BM, vector<int> & freMap)
{
  int BMSize = BM.getSize();
  if (BM.checkZero(trans)) freMap[trans] = 1;
  for(int j = 0; j < BMSize; j++)
    {
      if (BM.getValue(j,trans))
	{
	  if(BM.getValue(trans, j) == 0) /* make sure that we are not on an equal item */
	    //if (BM.checkZero(j)) freMap[trans] = 1;
	    if (freMap[j] == -1) recursion_Chk_Freq(j,BM,freMap);
	  freMap[trans] = (freMap[trans]>freMap[j] + 1)?freMap[trans]:(freMap[j] + 1);
	}
    }
}
/* free map is a vector, containing the longest path from each trans ? */
void loop_Chk_Freq(BinaryMatrix BM, vector<int> & freMap)
{
  int freMapSize = freMap.size();
  for(int i = 0; i < freMapSize; i++)
    {
      recursion_Chk_Freq(i,BM,freMap);
    }
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

int membership_oracle(const set_t &base_set, const element_t extension, 
		      const membership_data_t &data){

  if(data.support[extension] < threshold)
    return 0; 


  set_t s(base_set); 
  s.push_back(extension);
  sort(s.begin(), s.end());   
  for(int i = 0; i < s.size()-1; i++){
    if(s[i]/2 == (s[i+1])/2){
      /* removes sets including X+ X- simultaneously */
      //cout<<"REMOVED : "<<endl; 
      //      set_print(s); 
      return 0; 
    }
  }
  
  // if(base_set.size()){
  //   if(base_set[0] < extension)
  //     if(base_set[0] %2==0){
  // 	return 0;
  //     }
  // }
  // else{
  //   if(extension%2==0)
  //     return 0; 
  // }
  
  Occurence occurences;
  set_intersect(&occurences, data.base_set_occurences, data.extension_occurences);
  //  Occurence original_occurences(occurences.size()); 

  id_trans_t transaction_pairs(occurences.size());
  
  
  int i=0; 
  for(Occurence::const_iterator it = occurences.begin(); it != occurences.end(); ++it){    
    //    original_occurences[i++] = data.tt[*it].original_tid; 
    transaction_pairs[i++] = tid_code_to_original(data.tt[*it].original_tid); 
  }

  BinaryMatrix bm(nb_vtrans);
  bm.constructBinaryMatrixClogen(transaction_pairs); 
  
  vector<int> freMap(nb_vtrans); 
  loop_Chk_Freq(bm, freMap); 
  int sup = frequentCount(freMap); 
  
  
  //  int sup = get_longest_path(original_occurences);
  // cout<<"STARTTT"<<endl; 
  //   set_print(base_set); 
  // element_print(extension); 
  // cout<<" SUP "<<sup<<endl;
  // set_print_raw(occurences);
  // cout<<"ENDDDDD"<<endl; 
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


set_t calF(BinaryMatrix * BM, const vector<BinaryMatrix> & vBM, int & resFre)
{

	set_t is;
	is.clear();
	//TransactionSequence ts;
	int BMSize = BM->getSize();
	BinaryMatrix calFBM(BMSize);
	calFBM = (*BM);
	vector<int> freMap;
	freMap.clear();
	for (int i = 0; i < BMSize; i++)
	{
		freMap.push_back(-1);
	}
	loop_Chk_Freq(calFBM,freMap);
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
		if (tBM == (*BM)) is.push_back(i);
	}
	return is;
}


///////////////////////////////////////////////////////////////////
///////////////// CAL G  //////////////////////////////////////////
///////////////////////////////////////////////////////////////////
/* Computes the transactions quequences supporting itemset \is */
void calG(set_t is, const vector<BinaryMatrix *> & vBM, BinaryMatrix& res)
{
	int isSize = is.size();
	if (isSize == 0)
        return;

	res = *vBM[is[0]];

	for (int i = 1; i < isSize; i++)
        res &= *vBM[is[i]];
}


set_t clo(const set_t &set, int set_support, const SupportTable &support, const membership_data_t &data){

  const Occurence &occs = data.base_set_occurences; 
  id_trans_t transaction_pairs(occs.size());
  
  
  int i=0; 
  for(Occurence::const_iterator it = occs.begin(); it != occs.end(); ++it){    
    //    original_occs[i++] = data.tt[*it].original_tid; 
    transaction_pairs[i++] = tid_code_to_original(data.tt[*it].original_tid); 
  }

  BinaryMatrix bm(nb_vtrans);
  bm.constructBinaryMatrixClogen(transaction_pairs); 
  int a; 
  set_t closed_set = calF(&bm, all_bms, a); 
  
  return closed_set;
  //  return support_based_closure(set, set_support, support); 
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

  
  TransactionTable tmp; 
  read_transaction_table_vtrans(&tmp, argv[idx]);
  nb_vtrans = tmp.size(); 
  ELEMENT_RANGE_END = nb_attributes*2;
  
  cout<<"nb_attributes "<<nb_attributes<<endl;
  tt_to_grad_items(&tt, tmp); 
  cout<<nb_initial_trans<<endl;
  //  print_transaction_table(tt); 
  //  print_grad_transaction_table(tt);   
  tt.max_element = ELEMENT_RANGE_END; 
  transpose(tt, &ot);


  all_bms.reserve(ELEMENT_RANGE_END); 
  for(int i = 0; i < ELEMENT_RANGE_END; i++){
    BinaryMatrix bm(nb_vtrans);
    id_trans_t trans; 
    for(int j = 0; j < ot[i].size(); j++){
      trans.push_back(tid_code_to_original(ot[i][j]));
    }
 cout<<"BM FOR ";element_print(i);
    bm.constructBinaryMatrixClogen(trans);
    
    cout<<endl; 
    //     bm.PrintInfo();
    all_bms.push_back(bm); 
  }

  

  threshold = std::atoi(argv[idx+1]) ; 

  set_t empty_set; 
  int num_pattern = clogen(empty_set);
  cout<<num_pattern<<" patterns mined"<<endl;

}
