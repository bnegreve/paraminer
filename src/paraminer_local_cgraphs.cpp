/**
 * @file   paraminer_local_cgraphs.cpp
 * @author Benjamin Negrevergne <benjamin@neb.dyn.cs.kuleuven.be>
 * @date   Tue Oct 19 18:44:38 2010
 * 
 * @brief ParaMiner instance for mining closed connected relational
 * graphs (crg)
 * 
 */
#include <cstdlib>
#include <map>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <cmath>
#include <unistd.h>
#include <dirent.h>
#include "paraminer_local.hpp"
#include "utils.hpp"
#include "pattern.hpp"
#include "database.hpp"
#include "paraminer.hpp" 

using std::cout; 
using std::cerr; 
using std::endl; 

int ELEMENT_RANGE_END ; 

int global_nb_edges = 0; 
int global_nb_nodes = 0;
typedef std::multimap<int, int> graph_t; 
std::vector<graph_t> all_graphs;


typedef std::pair<int, int> edge_t; 
typedef int edge_id_t;

typedef std::map<edge_t, edge_id_t> edge_node_t; 
edge_node_t  edge_node;
typedef std::vector<edge_t> node_edge_t;
typedef std::vector<edge_t> edge_set_t; 
node_edge_t node_edge;

bool mine_only_closed = true; 
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
  t->weight = 1; 
  t->limit = t->size(); 
}

void read_input_graph(graph_t *graph, const std::string &graph_filename){
  //  std::map<int, bool> possible_root; 

  std::ifstream ifs (graph_filename.c_str(), std::ifstream::in);
  int nb_edges=0;
  int nb_nodes= 0; 
  int last_node = -1; 
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
	
	if(edge.first != last_node){
	  nb_nodes++;
	  last_node= edge.first; 
	}
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

   global_nb_edges += nb_edges; 
   global_nb_nodes += nb_nodes; 
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

inline bool edge_is_connected(const edge_t e1, const edge_t e2){
  return (e1.first == e2.first 
	  || e1.first == e2.second
	  || e1.second == e2.first
	  || e1.second == e2.second); 
}

inline bool edge_id_is_connected(const edge_id_t e1, const edge_id_t e2){
  return edge_is_connected(node_edge[e1], node_edge[e2]); 
}

edge_set_t set_to_edge_set(const set_t &set){
  edge_set_t edge_set; 
  edge_set.reserve(set.size()); 
  for(set_t::const_iterator s_it = set.begin(); s_it != set.end(); ++s_it){
    edge_set.push_back(node_edge[*s_it]); 
  }
  return edge_set; 
}

inline bool edge_is_connected_to_graph(const edge_set_t &edge_set, const edge_id_t e){
  if(edge_set.size() == 0)
    return true; 
  for(edge_set_t::const_iterator e_it = edge_set.begin(); e_it != edge_set.end(); ++e_it){
    if(edge_is_connected(*e_it, node_edge[e])){
      return true; 
    }
  }
  return false; 
}

bool is_connected_rec(const edge_set_t &edge_set, edge_t current_edge,
		      std::vector<bool>touched_edges, int nb_touched_edges) {
  //BUGGY FUNCTION DO NOT USE
  assert(false); 
  int i = 0; 
  for(edge_set_t::const_iterator edge_it = edge_set.begin(); edge_it != edge_set.end(); ++edge_it, i++){
    if(touched_edges[i] == false){
      if(edge_is_connected(current_edge, *edge_it)){
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

  edge_set_t edge_set = set_to_edge_set(set); 
  touched_edges[0] = true; 
  return is_connected_rec(edge_set, edge_set[0], touched_edges, 1); 
}

int membership_oracle(const set_t &base_set, const element_t extension, 
		      const membership_data_t &data){
  if(data.support[extension] >= threshold){  
    edge_set_t edge_set = set_to_edge_set(base_set);
    if(edge_is_connected_to_graph(edge_set, extension))
      return data.support[extension];
  }
  return false; 

}

set_t clo(const set_t &set, const closure_data_t &data){
  if(!mine_only_closed)
    return set; 
  set_t c(set);
  
  edge_set_t graph = set_to_edge_set(set); 

  bool change = false; 
  do{
    //TODO must be improved. 
    change = false; 
    for(int e = 0; e <= data.tt.max_element; e++){
      if(!set_member(c, e)){
	if(data.support[e] == data.set_support){
	  if(edge_is_connected_to_graph(graph, e)){
	    c.push_back(e);
	    graph.push_back(node_edge[e]); 
	    change = true; 
	  }
	}
      }
    }
  }while (change);
  return c; 
}


void usage(char *bin_name){
  paraminer_usage(bin_name);
  cerr<<"Problem specific command line arguments:"<<endl; 
  cerr<<bin_name<<" [<paraminer options> (See above.)] <graph dataset directory> <minsup> <edge threshold> [-a mine all graphs (not only closed ones)]"<<endl;
  
  exit(EXIT_FAILURE); 
}


int main(int argc, char **argv){

  parse_paraminer_arguments(&argc, argv); 

  char opt_char = 0; 

  while ((opt_char = getopt(argc, argv, "a")) != -1)
    {
      switch(opt_char){
      case 'a':
	mine_only_closed = false; 
	cout<<"Mining all connected graphs"<<endl;
  	break;
      default: 
  	usage(argv[0]);
  	break;
      }
    }
  int idx = optind;

  if(argc-idx != 3){
    usage(argv[0]); 
    exit(EXIT_FAILURE); 
  }

  
  edge_threshold = std::atoi(argv[idx+2]); 


  DIR *dir; 
  if(! (dir = opendir(argv[idx]))){
    perror(""); 
  }

  int nb_graphs = 0; 
  struct dirent *dirent; 
  while((dirent = readdir(dir))){
    if(dirent->d_name[0] != '.'){
      //TODO file order depends, therfore element associated with edges can change therefore edge order can depends on final output, solve this issues it messes up with testing. 
      char buf[128];
      snprintf(buf, 128, "%s/%s", argv[idx], dirent->d_name);
      all_graphs.resize(all_graphs.size()+1); 
      read_input_graph(&all_graphs.back(), buf);

      Transaction t;
      if(show_tids)	
	t.tids.push_back(nb_graphs); 
      graph_to_transaction(&t, all_graphs.back());




      t.weight = 1; 
      tt.push_back(t);
      nb_graphs++; 
    }
  }

float f_threshold = atof(argv[idx+1]); 
  if(f_threshold < 1){ //TODO ambiguity when 1 !
    threshold = std::ceil(f_threshold*(tt.size()));
  }
  else{
    threshold = f_threshold;
    f_threshold = (tt.size())/threshold;
  }
  cerr<<"THRESHOLD = "<<f_threshold<<" ["<<threshold<<" / "<<tt.size()<<"]"<<endl;



  ELEMENT_RANGE_END = node_edge.size();
  tt.max_element = ELEMENT_RANGE_END; 
  //  graph_print(all_graphs[0]); 


  //  print_transaction_table(tt);

  // for(int i = 0; i < tt[0].size(); i++){
  //   cout<<"( "<<node_edge[i].first<<", "<<node_edge[i].second<<" )"<<endl;
  // }

  transpose(tt, &ot);

  cout<<"ELEMENT_RANGE_END"<<ELEMENT_RANGE_END<<endl;
  cout<<"AVARAGE NB NODE : EDGES" <<global_nb_nodes/nb_graphs<<" : "<<global_nb_edges/nb_graphs<<endl; 
  // read_transaction_table(&tt, argv[idx+1]); 
  // threshold = std::atoi(argv[optind+2]); 
  // transpose(tt, &ot);
  
  set_t empty_set; 
  int num_pattern = paraminer(empty_set);
   cout<<num_pattern<<" patterns mined"<<endl;

}
