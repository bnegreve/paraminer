#!/bin/bash 
##
## fim_compare_nbr.sh
## 
## Made by Benjamin Negrevergne
## Login   <bengreve@neb.imag.fr>
## 
## Started on  Wed Oct 20 09:53:09 2010 Benjamin Negrevergne
##

usage(){
    echo "Usage: ./frequent_itemset_basic dataset minsup [nbthreads=1]" 1>&2
    echo "Compare #results produced by ParaMiner (fim) with #results produced by LCM." 1>&2
    echo "Succeed if both number # of results are identical" 1>&2
    exit 1
}

if [ $# -lt 2 -o $# -gt 3 ]; then usage; fi
if [ -f $1 ]; then DATASET=$1; else echo "Dataset file: '$1' not found." 1>&2; usage; fi
if [[ $2 =~ ^[0-9]+([.][0-9]+)?$ ]]; then THRES=$2;\
 else echo "Theshold: '$2' is not a numeric value." 1>&2; usage; fi
if [ $# -eq 2 ]; then \
    NUM_THREAD="1"; else \
    if [[ $3 =~ ^[0-9]+$ ]]; then NUM_THREAD=$3;\
 else echo "Number of threads: '$3' is not a integer value." 1>&2; usage; fi; fi


PARAMINER_FIM="paraminer_itemsets"
LCM_FIM="lcm25" 

CMD_LINE="$PARAMINER_FIM $DATASET $THRES -t $NUM_THREAD"
echo "command line: >$CMD_LINE<"
NB_PAT_CLOGEN=$($CMD_LINE |grep "patterns mined"|cut -d " " -f 1)

if [[ ! $NB_PAT_CLOGEN =~ ^[0-9]+$ ]]; then echo "ParaMiner: Cannot parse output." 1>&2; exit 1; fi

CMD_LINE="$LCM_FIM $DATASET $THRES"
echo "command line: >$CMD_LINE<"
NB_PAT_LCM=$($CMD_LINE | head -n 1)

if [[ ! $NB_PAT_LCM =~ ^[0-9]+$ ]]; then echo "LCM: Cannot parse output." 1>&2; exit 1; fi

echo "ParaMiner: $NB_PAT_CLOGEN patterns."
echo "Lcm: $NB_PAT_LCM patterns."
test $NB_PAT_CLOGEN -eq $NB_PAT_LCM
