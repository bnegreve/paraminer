#!/bin/sh
##
## timing.sh
## 
## Made by Benjamin Negrevergne
## Login   <bengreve@neb.imag.fr>
## 
## Started on  Fri Apr 20 09:54:59 2012 Benjamin Negrevergne
## Last update Fri Apr 20 09:54:59 2012 Benjamin Negrevergne
##

RUN=run.pl


$(RUN) -p PROBLEM itemsets itemsets itemsets itemsets graduals graduals -p PMVERSION elred pdbr -p DATASET accidents accidents kosarak BMS-WebView-2 I500 I4408 -p SUPPORT 68036 34018 1500 15 0.6 0.7 -p NUM_THREADS 1 16 32 -m 90 -t 7200 -u (PROBLEM=DATASET=SUPPORT)fxPMVERSIONlxNUM_THREADSc -- bins/clogen_PROBLEM_PMVERSION ../data/itemsets/DATASET.dat SUPPORT -t NUM_THREADS
