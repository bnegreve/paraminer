#!/bin/sh
##
## full_compare.sh
## 
## Made by Benjamin Negrevergne
## Login   <bengreve@neb.imag.fr>
## 
## Started on  Mon Nov 22 15:14:00 2010 Benjamin Negrevergne
## Last update Mon Nov 22 15:14:00 2010 Benjamin Negrevergne
##

../fim_closed  $1 $2 /tmp/lcmout > /dev/null
./sort_line.pl x < /tmp/lcmout  > /tmp/lcmout2 && sort < /tmp/lcmout2 > /tmp/lcmout
../clogen_itemsets $1 $2 -t $3 | ./sort_line.pl x | sort > /tmp/clogenout

diff /tmp/lcmout /tmp/clogenout 