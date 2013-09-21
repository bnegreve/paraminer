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
crg_compare_nbr.sh $PM_DATASET_PATH/crg/Hughes10 $PM_DATASET_PATH/crg/Hughes10-100.gspan 0.3 100
