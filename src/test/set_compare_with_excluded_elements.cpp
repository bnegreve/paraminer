// test.cpp
// Made by Benjamin Negrevergne
// Started on  Mon Mar 19 18:48:46 2012
#include <cstdlib>
#include <iostream>
#include <utils.hpp>

using namespace std; 
element_t ELEMENT_RANGE_START = 0; 
void element_print(element_t a){
  cout<<a<<" ";
}

void print_compare(const set_t &a, const set_t &b, const set_t &el,  bool expected, int line){

  /* Convert the exclusion list to bit representation */
  int max = 0; 
  for(int i = 0; i < el.size(); i++)
    max = el[i] > max ? el[i]:max;
  
  for(int i = 0; i < a.size(); i++)
    max = a[i] > max ? a[i]:max; 

  for(int i = 0; i < b.size(); i++)
        max = b[i] > max ? b[i]:max; 


  vector<bool> el_bit(max+1,  false); 
  for(int i = 0; i < el.size(); i++)
    el_bit[el[i]] = true; 

  cout <<"--- "<<endl;
  cout<<"SET A "; 
  for(int i = 0; i < a.size(); i++){
    cout<<a[i]<<" ";
  }
  cout<<endl; 

  cout<<"SET B "; 
  for(int i = 0; i < b.size(); i++){
    cout<<b[i]<<" ";
  }
  cout<<endl;

  cout<<"EL "; 
  for(int i = 0; i < el.size(); i++){
    cout<<el[i]<<" ";
  }
  cout<<endl;

  if(set_compare_with_excluded_elements(a, b, el_bit) != expected){
    cout<<"ERROR: Result ";
    if(expected)
      cout<<"a >= b returned whereas a<b expected";
    else
      cout<<"a < b returned whereas a >= b expected";

    cout<<" at line "<<line<<endl; 
    exit (EXIT_FAILURE); 
  }
}

int main(){

  set_t a; 
  set_t b; 
  set_t el; 

  
  /* compare equal non-el only sorted set */ 
  for (int i = 0; i < 2; i++){
    a.push_back(i); 
    b.push_back(i); 
  }
  print_compare(a, b, el, false, __LINE__); 
  print_compare(b, a, el, false, __LINE__); 

  /* add some el-elements and make sure it is still equal */
   for (int i = 3; i < 5; i++){
    a.push_back(i); 
    b.push_back(i);
    el.push_back(i); 
   }
  print_compare(a, b, el, false, __LINE__); 
  print_compare(b, a, el, false, __LINE__); 

  a.clear();
  b.clear(); 

  
  /* compare equal el only sorted set */ 
  for (int i = 0; i < 2; i++){
    a.push_back(i); 
    b.push_back(i);
    el.push_back(i);  
  }
  print_compare(a, b, el, false, __LINE__); 
  print_compare(b, a, el, false, __LINE__); 

  /* add some non el-elements and make sure it is still equal */
   for (int i = 3; i < 5; i++){
    a.push_back(i); 
    b.push_back(i);
   }
  print_compare(a, b, el, false, __LINE__); 
  print_compare(b, a, el, false, __LINE__); 

  a.clear(); 
  b.clear(); 
  el.clear(); 
  
  /* a < b, same size, non-el elements only */
  a.push_back(1); 
  a.push_back(2); 
  a.push_back(8); 
  b.push_back(1); 
  b.push_back(2); 
  b.push_back(9); 
  print_compare(a, b, el, true, __LINE__); 
  print_compare(b, a, el, false, __LINE__); 

  /* a < b, different size, non-el elements only */
  a.push_back(9); 
  print_compare(a, b, el, true, __LINE__); 
  print_compare(b, a, el, false, __LINE__); 

  a.clear(); 
  b.clear(); 
  /* a < b, same size, el elements only */
  a.push_back(1); 
  a.push_back(2); 
  a.push_back(8); 
  b.push_back(1); 
  b.push_back(2); 
  b.push_back(9); 
  el.push_back(1); 
  el.push_back(2); 
  el.push_back(8); 
  el.push_back(9); 
  print_compare(a, b, el, true, __LINE__); 
  print_compare(b, a, el, false, __LINE__); 

  /* a < b, different size, el elements only */
  a.push_back(9); 
  print_compare(a, b, el, true, __LINE__); 
  print_compare(b, a, el, false, __LINE__); 

  a.clear(); 
  b.clear(); 
  el.clear(); 
  /* a < b, different size, mixed elements */
  el.push_back(2); 
  a.push_back(2); /* a's non-el part is empty, hence b is larger */
  b.push_back(0); 
  b.push_back(2); 
  print_compare(a, b, el, true, __LINE__); 
  print_compare(b, a, el, false, __LINE__); 

  a.clear(); 
  b.clear(); 
  /* a < b, different size, mixed elements */
  a.push_back(0); 
  a.push_back(1); 
  a.push_back(3); 
  b.push_back(0); 
  b.push_back(1); 
  b.push_back(2); /* 2 is in el, hence b is larger */
  b.push_back(3); 
  print_compare(a, b, el, true, __LINE__); 
  print_compare(b, a, el, false, __LINE__); 

  return EXIT_SUCCESS; 


}
