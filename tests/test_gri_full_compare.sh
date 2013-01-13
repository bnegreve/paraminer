#!/bin/sh
##
## grad_glcm_full.sh
## 
## Made by Benjamin Negrevergne
## Login   <bengreve@neb.imag.fr>
## 
## Started on  Thu Feb  3 18:33:35 2011 Benjamin Negrevergne
## Last update Thu Feb  3 18:33:35 2011 Benjamin Negrevergne
##

# - run a simple instance of gri (test1@0.08) 
# - compare each gradual pattern generated 
# $PM_DATASET_PATH/ should be defined as the root dataset dir

gri_full_compare.sh $PM_DATASET_PATH/gri/test.dat 0.08
