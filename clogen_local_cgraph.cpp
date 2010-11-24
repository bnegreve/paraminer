// clogen_local_itemsets.cpp
// Made by Benjamin Negrevergne
// Started on  Tue Oct 19 18:44:38 2010
#include <cstdlib>
#include <map>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <dirent.h>
#include "clogen_local.hpp"
#include "utils.hpp"
#include "pattern.hpp"
#include "database.hpp"
#include "clogen.hpp" 

using std::cout; 
using std::cerr; 
using std::endl; 

 int ELEMENT_RANGE_START = 0; 
 int ELEMENT_RANGE_END ; 

typedef std::multimap<int, int > graph_t; 
std::vector<graph_t> all_graphs;


typedef std::pair<int, int> edge_t; 
typedef int edge_id_t;

typedef std::map<edge_t, edge_id_t> edge_node_t; 
edge_node_t  edge_node;
typedef std::vector<edge_t> node_edge_t;
typedef std::vector<edge_t> edge_set_t; 
node_edge_t node_edge;


int last_edge_id = 0;

extern int threshold;
int edge_threshold; 


void element_print(const element_t element){
  cout<<"( "<<node_edge[element].first<<", "<<node_edge[element].second<<" )";
}

void graph_print(const graph_t &graph)  {
  for(graph_t::const_iterator it = graph.begin(); it != graph.end(); ++it){
    cout<<"( "<<it->first<<", "<<it->second<<" )"<<endl;
  }
}



void graph_to_transaction(Transaction *t, const graph_t &graph){
  for(graph_t::const_iterator it = graph.begin(); it != graph.end(); ++it){
    int node_id; 
    edge_node_t::iterator ent = edge_node.find(*it); 
    if(ent != edge_node.end()){
      node_id = ent->second;
    }
    else{
      node_id = node_edge.size(); 
      edge_node.insert(ent, make_pair(*it, node_id));
      node_edge.push_back(*it); 
    }
    t->push_back(node_id); 
  }
  sort(t->begin(), t->end()); 
}

void read_input_graph(graph_t *graph, const std::string &graph_filename){
  //  std::map<int, bool> possible_root; 

  std::ifstream ifs (graph_filename.c_str(), std::ifstream::in);
  int nb_edges=0; 
  //ifs<<std::skipws;   
  ifs.ignore(256, '\n'); 
  while(ifs.good()){
    double this_edge_threshold; 
    std::pair<int, int> edge;
    ifs>>edge.first>>edge.second>>this_edge_threshold; 
    if(ifs.good()){
      if( this_edge_threshold >= edge_threshold){
	if(edge.first > edge.second){
	  int tmp = edge.first; 
	  edge.first = edge.second; 
	  edge.second = tmp; 
	}
	graph->insert(edge);	
	nb_edges++;
      }
      // std::map<int, bool>::iterator p_it;
      
      // if( possible_root.find(edge.first) == possible_root.end()){
      // 	/* if there is no record about the node 1, store it as a possible root */
      // 	possible_root.insert(make_pair(edge.first, true)); 
      // }

      // if( (p_it = possible_root.find(edge.second)) == possible_root.end()){
      // 	possible_root.insert(make_pair(edge.second, false)); 
      // }else{
      // 	p_it->second = false; 
      // }


    }
  }

  
  // /* complete */ 

  // map<int, bool>::const_iterator it; 
  // map<int, bool>::const_iterator it_end = possible_root.end(); 
  // for(it = possible_root.begin(); it != it_end; ++it){
  //   if(it->second)
  //     compute_transitive_closure(dag, it->first, std::vector<int>()); 
  // }
}

void read_graph(const std::string &graph_filename){  
#if 0
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
  #endif
}

bool is_connected_rec(const edge_set_t &edge_set, edge_t current_edge,
		      std::vector<bool>touched_edges, int nb_touched_edges) {
  int i = 0; 
  for(edge_set_t::const_iterator edge_it = edge_set.begin(); edge_it != edge_set.end(); ++edge_it, i++){
    if(touched_edges[i] == false){
      if(edge_it->first == current_edge.first 
	 || edge_it->first == current_edge.second
	 || edge_it->second == current_edge.first 
	 || edge_it->second == current_edge.second){

	if(++nb_touched_edges == edge_set.size())
	  return true; 
	touched_edges[i] = true;
	if(is_connected_rec(edge_set, *edge_it, touched_edges, nb_touched_edges))
	  return true; 
      }
    }
  }
  return false;
}

bool is_connected(const set_t &set){
  if(set.size() < 2)
    return 1; 
  std::vector<bool> touched_edges(set.size(), false);
  edge_set_t edge_set; 
  edge_set.reserve(set.size()); 
  for(set_t::const_iterator s_it = set.begin(); s_it != set.end(); ++s_it){
    edge_set.push_back(node_edge[*s_it]); 
  }
  touched_edges[0] = true; 
  return is_connected_rec(edge_set, edge_set[0], touched_edges, 1); 
}

bool edge_is_connected(const set_t &set, const graph_t &graph, element_t e){

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
  assert(false); 
#if 0
  if(!is_connected(set))
    return 0; 

  int freq = count_inclusion_2d(tt, set);
  
  if(freq >= threshold)
    return freq; 
  return 0;
  #endif 
}


int membership_oracle(const set_t &set, const TransactionTable &tt,
		      const Transaction &occurences){
  if(is_connected(set) == false)
     return 0; 
  return set_is_frequent_in_occurences(set, tt, occurences, threshold); 

}

set_t clo(const set_t &set){
#if 0 
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
#endif //ATTENTION VIRER LE RETURN EN DESSOUS
  return set; 
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
  threshold = std::atoi(argv[idx+1]);
  edge_threshold = std::atoi(argv[idx+2]); 

  DIR *dir; 
  if(! (dir = opendir(argv[idx]))){
    perror(""); 
  }

  struct dirent *dirent; 
  while((dirent = readdir(dir))){
    if(dirent->d_name[0] != '.'){
      char buf[128];
      snprintf(buf, 128, "%s/%s", argv[idx], dirent->d_name);
      all_graphs.resize(all_graphs.size()+1); 
      read_input_graph(&all_graphs.back(), buf);

      Transaction t; 
      graph_to_transaction(&t, all_graphs.back()); 
      tt.push_back(t);
    }
  }

  ELEMENT_RANGE_END = node_edge.size();
  
  //  graph_print(all_graphs[0]); 


  //  print_transaction_table(tt);

  // for(int i = 0; i < tt[0].size(); i++){
  //   cout<<"( "<<node_edge[i].first<<", "<<node_edge[i].second<<" )"<<endl;
  // }

  transpose(tt, &ot);

  cout<<"ELEMENT_RANGE_END"<<ELEMENT_RANGE_END<<endl;
  // read_transaction_table(&tt, argv[idx+1]); 
  // threshold = std::atoi(argv[optind+2]); 
  // transpose(tt, &ot);
  
  set_t empty_set; 
  int num_pattern = clogen(empty_set);
   cout<<num_pattern<<" patterns mined"<<endl;

}
