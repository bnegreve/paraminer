#!/bin/sh

make -j 32


cp clogen_itemsets bins/clogen_itemsets_`git branch | grep '\*' | cut -d ' ' -f 2`
cp clogen_cgraph bins/clogen_cgraph_`git branch | grep '\*' | cut -d ' ' -f 2`
cp clogen_graduals bins/clogen_graduals_`git branch | grep '\*' | cut -d ' ' -f 2`