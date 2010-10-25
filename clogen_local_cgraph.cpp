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

using std::cout; 
using std::cerr; 
using std::endl; 

const int ELEMENT_RANGE_START = 1; 
const int ELEMENT_RANGE_END = 5; 

std::multimap<int, int > graph; 

extern int threshold; 

void read_graph(const std::string &graph_filename){  
  std::ifstream ifs (graph_filename.c_str(), std::ifstream::in);
  int nb_edges=0; 
  //ifs<<std::skipws; 
  while(ifs.good()){
    std::pair<int, int> edge;
    ifs>>edge.first>>edge.second; 
    if(ifs.good()){
	graph.insert(edge); 
	int tmp = edge.first; 
	edge.first = edge.second; 
	edge.second = tmp; 	
	graph.insert(edge); 
	nb_edges++;
    }
  }

  // while (ifs.good()){
  //   std::string line; 
  //   std::stringstream ss; 
  //   std::pair<int, int> edge;
  //   ss<<std::skipws; 
  //   getline(ifs, line); 
  //   ss<<line; 
  //   if(ss.good()){
  //     ss>>edge.first>>edge.second; 
  //     if(ss.good()){
  // 	graph.insert(edge); 
  // 	int tmp = edge.first; 
  // 	edge.first = edge.second; 
  // 	edge.second = tmp; 	
  // 	graph.insert(edge); 
  // 	nb_edges++;
  //     }
  //   }
  // }

  cout<<"graph loaded, "<<nb_edges<<" edges."<<std::endl;
  ifs.close();
}

bool is_connected_rec(const set_t &set, int current,  std::vector<bool>touched_edges, int nb_touched_edges) {
  /* find all eges pair starting with current edge */
  touched_edges[current] = true; 
  nb_touched_edges++; 

  if(nb_touched_edges == set.size())
    return true; 

  std::pair<std::map<int, int>::const_iterator, std::map<int, int>::const_iterator> range = 
    graph.equal_range(set[current]);

  for(std::map<int, int>::const_iterator it = range.first; it != range.second; ++it){
    int idx; 
    if( (idx = set_member_index(set, (*it).second)) != -1 ){
      if(touched_edges[idx] == false)
	if(is_connected_rec(set, idx, touched_edges, nb_touched_edges))
	  return true; 
    }
  }
  return false;
}

bool is_connected(const set_t &set){
  if(set.size() < 2)
    return 1; 
  std::vector<bool> touched_edges(set.size(), false); 
  return is_connected_rec(set, 0, touched_edges, 0); 

}

bool edge_is_connected(const set_t &set, element_t e){

  for(int i = 0; i < set.size(); i++){
    /* Find all successors of current edge */
    std::pair<std::map<int, int>::const_iterator, std::map<int, int>::const_iterator> range = 
      graph.equal_range(set[i]);

      for(std::map<int, int>::const_iterator it = range.first; it != range.second; ++it){
	if(it->second == e)
	  return true; 
    }
  }
  return false; 
}

int membership_oracle(const set_t &set){
  if(!is_connected(set))
    return 0; 
  int freq = count_inclusion_2d(tt, set);
  
  if(freq >= threshold)
    return freq; 
  return 0; 
}


int membership_oracle(const set_t &set, const TransactionTable &tt,
		      const Transaction &occurences){
  if(!is_connected(set))
    return 0; 

  return count_inclusion_2d(tt, occurences, set) >= threshold;
}

set_t clo(const set_t &set){
  /* We keep in the closure, only the part of the extension connected to the base */ 
  /* See Boley's paper example 8 */ 

  Occurence oc;
  set_t clo(set); 
  get_occurences_2d(tt, set, &oc); 

  set_t dummy_set; 
  for(int i = 0 ; i < oc.size(); i++)
    dummy_set.push_back(oc[i]); 

  /* Deal with the empty set case */ 
  if(set.size() == 0){
    for(int i = 0; i < ot.size(); i++){
      if(get_set_presence_1d((ot)[i], dummy_set) == 1){
	clo.push_back(i); //
      }
    }
    if(is_connected(clo))
       return clo; 
    return set; 
  }  

  std::vector<bool> visited(ot.size(), false);
  for(int i = 0; i < ot.size(); i++){
    if(visited[i])
      continue; 
    if(set_member(clo, i)){
      visited[i] = true; 
      continue; 
    }
    if(edge_is_connected(clo, i)){
      if(get_set_presence_1d((ot)[i], dummy_set) == 1){
	clo.push_back(i); 
	std::sort(clo.begin(), clo.end()); 
	visited = std::vector<bool>(ot.size(), false); 
	i = 0; 
      }
    }
  }
  return clo; 
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

  read_graph(argv[idx]); 
  read_transaction_table(&tt, argv[idx+1]); 
  threshold = std::atoi(argv[optind+2]); 
  transpose(tt, &ot);
  

  set_t empty_set; 
  int num_pattern = clogen(empty_set);
  cout<<num_pattern<<" patterns mined"<<endl;

}
