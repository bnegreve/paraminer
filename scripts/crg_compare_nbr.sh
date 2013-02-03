#!/bin/bash
##
## cgraph_gspan_test.sh
## 
## Made by Benjamin Negrevergne
## Login   <bengreve@neb.imag.fr>
## 
## Started on  Wed Feb  2 19:27:41 2011 Benjamin Negrevergne
## Last update Wed Feb  2 19:27:41 2011 Benjamin Negrevergne
##


# edge threshold is 120 and min sup is arg 1. 
usage(){
    echo "Usage: ./crg_compare_nbr <dataset in paraminer format>\
 <dataset in gspan format> <minsup> <edge threshold>" 1>&2
    echo "Compare #results produced by ParaMiner (crg) with #results produced by gSpan." 1>&2
    echo "Succeed if both number # of results are identical" 1>&2
    echo "WARNING: Edge threshold is only for ParaMiner's dataset, the gSpan dataset must be build with the same edge threshold. (see convert_to_gspan.pl script to build a gSpan dataset)." 1>&2
    exit 1
}

PARAMINER='paraminer_cgraphs';
GSPAN='gSpan'; 

if [ $# -ne 4 ]; then usage; fi
if [ -d $1 ]; then DATASET_PM=$1; else\
 echo "Dataset file for ParaMiner: '$1' not found or not a directory." 1>&2; usage; fi
if [ -f $2 ]; then DATASET_GS=$2; else\
 echo "Dataset file for gSpan: '$2' not found or not a file." 1>&2; usage; fi
if [[ $3 =~ ^[0-9]+([.][0-9]+)?$ ]]; then THRES=$3;\
 else echo "Theshold: '$3' is not a numeric value." 1>&2; usage; fi
if [[ $4 =~ ^[0-9]+([.][0-9]+)?$ ]]; then EDGE_THRES=$4;\
 else echo "Edge theshold: '$4' is not a numeric value." 1>&2; usage; fi

echo "Support = $THRES"; 

GS_DATASET_TEMP=`mktemp --suffix=_gSpan_dataset`

cp $DATASET_GS $GS_DATASET_TEMP
CMD_LINE="$GSPAN -f $GS_DATASET_TEMP -o -s $THRES"
echo "command line: >$CMD_LINE<"

NB_PAT_GSPAN=$($CMD_LINE > /dev/null;
gspan_remove_1graphs.pl $GS_DATASET_TEMP.fp /dev/null  | cut -d " " -f 1); 

if [ ! -f $GS_DATASET_TEMP.fp ] ; 
    then echo "gSpan output file '$GS_DATASET_TEMP.fp' does not exist." >&2; exit 1; fi 


echo "gSpan: $NB_PAT_GSPAN patterns."; 

if [[ ! $NB_PAT_GSPAN =~ ^[0-9]+$ ]]; then echo "gSpan: Cannot parse output." 1>&2; exit 1; fi


NB_PAT_CLOGEN=$($PARAMINER $DATASET_PM $THRES $EDGE_THRES -a | tail -n 1 |cut -d " " -f 1)


if [[ ! $NB_PAT_CLOGEN =~ ^[0-9]+$ ]]; then echo "ParaMiner: Cannot parse output." >&2; exit 1; fi

echo "ParaMiner: $NB_PAT_CLOGEN patterns."; 


test $NB_PAT_CLOGEN -eq $NB_PAT_GSPAN

