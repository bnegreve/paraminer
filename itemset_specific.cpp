// itemset_specific.cpp
// Made by Benjamin Negrevergne
// Started on  Fri Sep  3 14:09:56 2010
#include <sstream>
#include <fstream>
#include "itemset_specific.hpp"

using namespace std; 

void read_transaction_table(TransactionTable *tt, const char *filename){

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
      ++nb_items; 
      ss>>item;
    }
    if(t.size() != 0){
      tt->push_back(t); 
      ++nb_trans; 
    }
  }

  cout<<"Data loaded, "<<nb_items<<" items within "<<nb_trans<<" transactions."<<endl;
  ifs.close();
}

void print_transaction(const Transaction &t){
  for(int i = 0; i < t.size(); i++){
    cout<<t[i]<<" "; 
  }
}

void print_transaction_table(const TransactionTable &tt){
  for(int i = 0; i < tt.size(); i++){
    cout<<i<<" : "; print_transaction(tt[i]); 
    cout<<endl; 
  }
}
#if 0 
int main(int argc, char **argv){
  TransactionTable tt; 
  read_transaction_table(&tt, "test.dat"); 
  print_transaction_table(tt); 
}
#endif 
