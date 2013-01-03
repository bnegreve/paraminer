#!/bin/sh
##
## test_fim_simple.sh
## 
## Made by Benjamin Negrevergne
## Login   <benjamin@neb.dyn.cs.kuleuven.be>
## 
## Started on  Thu Jan  3 11:39:55 2013 Benjamin Negrevergne
## Last update Thu Jan  3 11:39:55 2013 Benjamin Negrevergne
##


# - run two simple instance of fim (mushroom@4000 and mushroom@1500)
# - compare # of pattern generated with lcm. 
frequent_itemset_basic.sh 4000 1 && frequent_itemset_basic.sh 1500 1
