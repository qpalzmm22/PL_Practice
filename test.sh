#!/bin/bash

test_prog="./mjyint "

declare -a tclist=("{+ 1 2}"
  "{with {x {with {t 3} {+ t 1}}} {+ 3 4}}"
  "{+ {- {+ 2 2} {+ {- 2 4} 4}}}"
  "hel"
  "{with {x {with {t x} {+ t 1}}} {+ 3 4}}" # free variable
  "{with {x 2} {+ {with {x 3} {+ x 1}} 4}}" # 7
  "{+ {- {+ 2 2} {+ {with {x 3} {+ 1 x}} 2}} 1}"
  "{the 3}"
  "{{fun {x} {+ x 1}} 7}"
  "{with {x 3} {with {f {fun {y} {+ x y}}} {with {x 5} {f 4}}}}"
  "{with {f {dsfun {y} {+ x y}}} {with {x 5} {f 4}}}"
  "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
)

for val in "${tclist[@]}";
do
  echo "=== test case ${val} === "
  ${test_prog} "${val}"
done

#echo {python -c "print 'a' *10"}
#testscript=`python -c 'print "A"*1025'`
#${test_prog} ${testscript}
