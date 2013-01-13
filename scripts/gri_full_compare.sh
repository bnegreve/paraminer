#!/bin/bash
##
## gri_full_compare.sh
## 
## Made by Benjamin Negrevergne
## Login   <benjamin@neb.dyn.cs.kuleuven.be>
## 
## Started on  Sun Jan 13 17:07:28 2013 Benjamin Negrevergne
## Last update Sun Jan 13 17:07:28 2013 Benjamin Negrevergne
##

usage(){
echo "Usage: ./gri_full_compare dataset minsup [num_thread]" 1>&2
echo "Compare results produced by ParaMiner (gri) with results produced by Glcm." 1>&2
echo "Succeed if results are identical" 1>&2
exit 1
}

PARAMINER_GRI="paraminer_graduals"
GLCM="glcm"

if [ $# -lt 2 -o $# -gt 3 ]; then usage; fi
if [ -f $1 ]; then DATASET=$1; else echo "Dataset file: '$1' not found." 1>&2; usage; fi
if [[ $2 =~ ^[0-9]+([.][0-9]+)?$ ]]; then THRES=$2;\
 else echo "Theshold: '$2' is not a numeric value." 1>&2; usage; fi
if [ $# -eq 2 ]; then \
    NUM_THREAD="1"; else \
    if [[ $3 =~ ^[0-9]+$ ]]; then NUM_THREAD=$3;\
 else echo "Number of threads: '$3' is not a integer value." 1>&2; usage; fi; fi

CMD_LINE="$GLCM nor $DATASET $THRES "
echo "command line: >$CMD_LINE<"

$CMD_LINE | sort_line_grads.pl > /tmp/glcm_tmp1
if [ ! -f /tmp/glcm_tmp1 ]; then \
    echo "Error: Could not produce GLCM output file." 1>&2; exit 1; fi

CMD_LINE="$PARAMINER_GRI $DATASET $THRES -t $NUM_THREAD"
echo "command line: >$CMD_LINE<"
$CMD_LINE  | sort_line_grads.pl > /tmp/pm_gri_tmp1

if [ ! -f /tmp/pm_gri_tmp1 ]; then \
    echo "Error: Could not produce ParaMiner (gri) output file." 1>&2; exit 1; fi

diff /tmp/glcm_tmp1 /tmp/pm_gri_tmp1



