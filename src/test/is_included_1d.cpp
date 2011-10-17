// is_included_1d.cpp
// Made by Benjamin Negrevergne
// Started on  Mon Sep  6 12:03:21 2010

#include <cstdlib>
#include <pattern.hpp>
#include <utils.hpp>

using namespace std; 

int main(int argc, char **argv){

  assert(argc == 2); 
  for(int k = 0; k < atoi(argv[1]); k++){
    Transaction t; 
    set_t set; 

    srand(time(NULL)); 

    /* present test */
    for(int i = 0; i < 100; i++){
      if((rand()%2) == 1)
	if((rand()%2) == 1){
	  t.push_back(i); 
	  set.push_back(i); 
	}
	else
	  t.push_back(i); 
    }

    cout<<"Transaction :"<<endl;
    print_transaction(t); 
    cout<<"Pattern :"<<endl;
    set_print(set); 

    cout<<"Result :"<<endl;
    assert(is_included_1d(t, set)); 


    t.clear(); 
    set.clear(); 

    /* not present test */
    int set_size = rand()%12+1; 
    cout << set_size<<endl;
    int set_nb_present = rand()%(set_size); 
    int flag = 0; 
    cout << set_nb_present<<endl;
    int i = 0; 
    do{
      if((rand()%2) == 1){
	t.push_back(i);
      }else
	if((rand()%10) <= 8){
	  set.push_back(i);
	  t.push_back(i); 
	}else{
	  set.push_back(i);
	  flag++;
	}
      i++; 
    }while(!flag); 

    cout<<"Transaction :"<<endl;
    print_transaction(t); 
    cout<<"Pattern :"<<endl;
    set_print(set); 

    cout<<"Result :"<<endl;
    assert(is_included_1d(t, set)== 0); 
  }
  exit(EXIT_SUCCESS); 
}
