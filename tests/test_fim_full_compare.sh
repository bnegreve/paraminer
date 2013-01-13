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


# - run a simple instance of fim (mushroom@2500) 
# - compare each itemset generated 
# $PM_DATASET_PATH/ should be defined as the root dataset dir
fim_full_compare.sh $PM_DATASET_PATH/fim/mushroom.dat 2500 1


