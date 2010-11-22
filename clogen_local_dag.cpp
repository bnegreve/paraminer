// clogen_local_itemsets.cpp
// Made by Benjamin Negrevergne
// Started on  Tue Oct 19 18:44:38 2010
#include <cstdlib>
#include <map>
#include <fstream>
#include <algorithm>
#include <sstream>
#include "clogen_local.hpp"
#include "utils.hpp"
#include "pattern.hpp"
#include "database.hpp"
#include "clogen.hpp" 

using namespace std; 
 int ELEMENT_RANGE_START = 1; 
 int ELEMENT_RANGE_END = 8*9; 



extern int threshold; 
int g; 

typedef  std::multimap <int, int> dag_t; 

void dag_print(const dag_t &dag)  {
  for(dag_t::const_iterator it = dag.begin(); it != dag.end(); ++it){
    cout<<"( "<<it->first<<", "<<it->second<<" )"<<endl;
  }
}


void dag_to_transaction(Transaction *t, const dag_t &dag){
  for(dag_t::const_iterator it = dag.begin(); it != dag.end(); ++it){
    t->push_back((it->first) * 8 + (it->second)); 
  }
  sort(t->begin(), t->end()); 

}

/* Complexity too high ! doesnt scale up to one single dag in Huges*/
void compute_transitive_closure(dag_t *dag, int current_node, std::vector<int> predecesors){

  bool flag = false; 
  for(int i = 0; i < predecesors.size(); i++){
    std::pair<dag_t::const_iterator, dag_t::const_iterator> range = dag->equal_range(predecesors[i]);
    dag_t::const_iterator succ_it; //:)
    for(succ_it = range.first; (!flag) && succ_it != range.second; ++succ_it){
      if(succ_it->second == current_node)
	flag = true; 
    }
    if(!flag){
      dag->insert(make_pair(predecesors[i], current_node)); 
    }
    else
      flag = false; 
  }

  predecesors.push_back(current_node); 

  std::pair<dag_t::const_iterator, dag_t::const_iterator> range = dag->equal_range(current_node);
  dag_t::const_iterator succ_it; //:)
  for(succ_it = range.first; succ_it != range.second; ++succ_it){
    compute_transitive_closure(dag, succ_it->second, predecesors);     
  }
}

void read_input_dag(dag_t *dag, const std::string &dag_filename){
  std::map<int, bool> possible_root; 

  std::ifstream ifs (dag_filename.c_str(), std::ifstream::in);
  int nb_edges=0; 
  //ifs<<std::skipws;   
  ifs.ignore(256, '\n'); 
  while(ifs.good()){
    double edge_threshold; 
    std::pair<int, int> edge;
    ifs>>edge.first>>edge.second>>edge_threshold; 
    if(ifs.good()){
      dag->insert(edge); 

      std::map<int, bool>::iterator p_it; 
      if( possible_root.find(edge.first) == possible_root.end()){
	/* if there is no record about the node 1, store it as a possible root */
	possible_root.insert(make_pair(edge.first, true)); 
      }

      if( (p_it = possible_root.find(edge.second)) == possible_root.end()){
	possible_root.insert(make_pair(edge.second, false)); 
      }else{
	p_it->second = false; 
      }
      nb_edges++;

    }
  }

  
  /* complete */ 

  map<int, bool>::const_iterator it; 
  map<int, bool>::const_iterator it_end = possible_root.end(); 
  for(it = possible_root.begin(); it != it_end; ++it){
    if(it->second)
      compute_transitive_closure(dag, it->first, std::vector<int>()); 
  }
}

// bool is_connected_rec(const set_t &set, int current,  std::vector<bool>touched_edges, int nb_touched_edges) {
//   /* find all eges pair starting with current edge */
//   touched_edges[current] = true; 
//   nb_touched_edges++; 

//   if(nb_touched_edges == set.size())
//     return true; 

//   std::pair<std::map<int, int>::const_iterator, std::map<int, int>::const_iterator> range = 
//     graph.equal_range(set[current]);

//   for(std::map<int, int>::const_iterator it = range.first; it != range.second; ++it){
//     int idx; 
//     if( (idx = set_member_index(set, (*it).second)) != -1 ){
//       if(touched_edges[idx] == false)
// 	if(is_connected_rec(set, idx, touched_edges, nb_touched_edges))
// 	  return true; 
//     }
//   }
//   return false;
// }

// bool is_connected(const set_t &set){
//   if(set.size() < 2)
//     return 1; 
//   std::vector<bool> touched_edges(set.size(), false); 
//   return is_connected_rec(set, 0, touched_edges, 0); 

// }

// bool edge_is_connected(const set_t &set, element_t e){

//   for(int i = 0; i < set.size(); i++){
//     /* Find all successors of current edge */
//     std::pair<std::map<int, int>::const_iterator, std::map<int, int>::const_iterator> range = 
//       graph.equal_range(set[i]);

//       for(std::map<int, int>::const_iterator it = range.first; it != range.second; ++it){
// 	if(it->second == e)
// 	  return true; 
//     }
//   }
//   return false; 
// }

int membership_oracle(const set_t &set){
  return 1; 
  // if(!is_connected(set))
  //   return 0; 
  // int freq = count_inclusion_2d(tt, set);
  
  // if(freq >= threshold)
  //   return freq; 
  // return 0; 
}


int membership_oracle(const set_t &set, const TransactionTable &tt,
		      const Transaction &occurences){
  return 1; 
  // if(!is_connected(set))
  //   return 0; 

  // return count_inclusion_2d(tt, occurences, set) >= threshold;
}

set_t clo(const set_t &s){

  Occurence oc;
  set_t clo; 
  set_t set(s); 
  std::sort(set.begin(), set.end()); 
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

set_t clo(const set_t &set, int set_support, const SupportTable &support){
  return clo(set); 
}


void usage(char *bin_name){
  cout<<bin_name<<" graphdescription dataset minsup [-t numthreads=1] [-c tuplecutoff=2]"<<endl;
  exit(EXIT_FAILURE); 
}


int main(int argc, char **argv){

  int idx = parse_clogen_arguments(&argc, argv); 
  if(argc-idx != 3){
    usage(argv[0]); 
    exit(EXIT_FAILURE); 
  }

  dag_t dag; 
  read_input_dag(&dag, argv[idx]); 


  dag_print(dag); 

  Transaction t; 
  dag_to_transaction(&t, dag); 
  tt.push_back(t); 
  threshold = std::atoi(argv[optind+2]); 
  transpose(tt, &ot);

  /* TODO do not include in element range elements that don't appear initialy in db */
  ot.resize(ELEMENT_RANGE_END); 

  set_t empty_set; 
  int num_pattern = clogen(empty_set);
  cout<<num_pattern<<" patterns mined"<<endl;

}
