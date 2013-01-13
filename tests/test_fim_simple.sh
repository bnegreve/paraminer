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
# $PM_DATASET_PATH/ should be define as the root dataset dir
fim_compare_nbr.sh $PM_DATASET_PATH/fim/mushroom.dat 4000 1 && fim_compare_nbr.sh $PM_DATASET_PATH/fim/mushroom.dat 1500 1
