// test.cpp
// Made by Benjamin Negrevergne
// Started on  Fri Mar 23 10:28:24 2012

#include "utils.hpp"
#include "database.hpp"

using namespace std; 
element_t ELEMENT_RANGE_START = 0; 
void element_print(element_t a){
  cout<<a<<" ";
}

int main(){
  element_t v1[] = {23, 36, 39, 14, 26}; 
  element_t v2[] = {23, 36, 1, 13, 25, 38};
  element_t v3[] = {39, 2, 16, 24,28, 37}; 

  element_t vp[]=  {3,9,23,34,36,40}; 
  element_t vel[]=  {1,2,10,13,14,15,16,24,25,26,28,37,38};

  Transaction occs;
 
  occs.push_back(0); 
  occs.push_back(1); 

  
  Transaction t1,t2, t3; 
  t1.weight = t2.weight = t3.weight = 0; 
  for(int i = 0; i < sizeof(v1)/4; i++){
    t1.push_back(v1[i]); 
  }
  for(int i = 0; i < sizeof(v2)/4; i++){
    t2.push_back(v2[i]); 
  }

  for(int i = 0; i < sizeof(v3)/4; i++){
    t3.push_back(v3[i]); 
  }

  set_t el (vel, vel+sizeof(vel)/4);
  set_t pattern (vp, vp+sizeof(vp)/4);

  TransactionTable tt; 
  tt.push_back(t1); 
  tt.push_back(t2); 
  tt.push_back(t3); 
  tt.max_element = 40; 

  cout<<"pattern : "; set_print(pattern); 
  cout<<"el : "; set_print(el); 

  cout<<"BEFORE"<<endl; 
  print_transaction_table(tt);
  cout<<"END BEFORE"<<endl; 
  TransactionTable tt2;

  database_build_reduced2(&tt2, tt , occs, pattern, el, 0, true); 

  cout<<"AFTER"<<endl; 
  print_transaction_table(tt2);
  cout<<"END AFTER"<<endl; 

}
