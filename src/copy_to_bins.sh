#!/bin/sh

make -j 32


cp paraminer_itemsets bins/paraminer_itemsets_`git branch | grep '\*' | cut -d ' ' -f 2`
cp paraminer_cgraph bins/paraminer_cgraph_`git branch | grep '\*' | cut -d ' ' -f 2`
cp paraminer_graduals bins/paraminer_graduals_`git branch | grep '\*' | cut -d ' ' -f 2`