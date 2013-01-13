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


# - run a simple instance of crg (Hughes-120@0.55)
# - compare # of pattern generated with gSpan.
# $PM_DATASET_PATH/ should be define as the root dataset dir
crg_compare_nbr.sh $PM_DATASET_PATH/crg/Hughes $PM_DATASET_PATH/crg/Hughes-120.gspan 0.55 120
