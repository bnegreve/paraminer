#!/bin/sh
##
## cgraph_gspan_test.sh
## 
## Made by Benjamin Negrevergne
## Login   <bengreve@neb.imag.fr>
## 
## Started on  Wed Feb  2 19:27:41 2011 Benjamin Negrevergne
## Last update Wed Feb  2 19:27:41 2011 Benjamin Negrevergne
##

#run gspan and compare number of generated patterns with clogen_cgraph.
#edge threshold is 120 and min sup is arg 1. 

echo "Support = $1"; 

NB_PAT_GSPAN=$(external/gSpan6/gSpan -f ../data/cgraph/Hughes-120.gspan -o -s $1 > /dev/null;
../data/cgraph/gspan_remove_1graphs.pl ../data/cgraph/Hughes-120.gspan.fp /dev/null  | cut -d " " -f 1); 

echo "GSPAN: $NB_PAT_GSPAN"; 

NB_PAT_CLOGEN=$(../src/clogen_cgraph    ../data/cgraph/Hughes/ $1 120   -a | tail -n 1 |cut -d " " -f 1)

echo "CLOGEN: $NB_PAT_CLOGEN"; 

test $NB_PAT_CLOGEN -eq $NB_PAT_GSPAN