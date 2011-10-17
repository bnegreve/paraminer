// is_included_1d.cpp
// Made by Benjamin Negrevergne
// Started on  Mon Sep  6 12:03:21 2010

#include <cstdlib>
#include <pattern.hpp>
#include <utils.hpp>

using namespace std; 

int main(int argc, char **argv){
  int nb_item = 0; 
  TransactionTable tt, ot, tt2; 

  for(int i  = 0; i < 100; i++){
    Transaction t; 
    for(int j = 0; j < 100; j++){
      if(rand()%2){
	nb_item++; 
	t.push_back(j);
      }
    }
    tt.push_back(t); 
  }

  transpose(tt, &ot); 
  
  transpose(ot, &tt2); 

    if(tt.size() != tt2.size()){
	cout<<"tt and tt2 don't have the same number of transactions. "<<endl;
	cout<<"tt : "<<tt.size()<<" tt2 "<<tt2.size()<<endl;
	exit(EXIT_FAILURE); 
    }

  for(int i = 0; i < tt.size(); i++){
    if(tt[i].size() != tt2[i].size()){
	cout<<"Transaction "<<i<<" don't have the same size."<<endl;
	print_transaction(tt[i]);
	cout<<endl;
	print_transaction(tt2[i]);
	exit(EXIT_FAILURE); 
    }
    for(int j = 0; j < tt[i].size(); j++){
      if(tt[i][j] != tt2[i][j]){
	cout<<"Transaction "<<i<<" are differents."<<endl;
	print_transaction(tt[i]);
	cout<<endl;
	print_transaction(tt2[i]);
	exit(EXIT_FAILURE); 
      }
    }
  }	

  exit(EXIT_SUCCESS); 
}

