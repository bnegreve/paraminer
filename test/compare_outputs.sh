#!/bin/sh
##
## compare_outputs.sh
## 
## Made by Benjamin Negrevergne
## Login   <bengreve@neb.imag.fr>
## 
## Started on  Thu Oct 21 15:13:53 2010 Benjamin Negrevergne
## Last update Thu Oct 21 15:13:53 2010 Benjamin Negrevergne
##

usage(){
    echo "compare_outputs.sh expected-output-file"
    echo "expected-output-file being a file starting with a command line and the expected output for the first line"
    exit 1
}

if [ $# -lt 1 ] ; then usage ; fi
if [ $# -eq 1 ] ; then PAT="\'\'"; else PAT=$2; fi

head -n 1 $1 | sh |grep $PAT > /tmp/output_co
if [ $? -ne 0 ] ; then exit 1; fi
tail -n +2 $1 | grep $PAT > /tmp/expected_co
diff /tmp/expected_co /tmp/output_co