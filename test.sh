#!/bin/bash

test_prog="./mjyint "
tc1="{+ 1 2}"
tc2="{with {x {with {t 3} {+ t 1}}} {+ 3 4}}"
tc3="{with {x 3} {with {f {fun {y} {+ x y}}} {with {x 5} {f 4}}}}"
tc4="{+ {- {+ 2 2} {+ {- 2 4} 4}}}"
tc5="hel"
tc6="{with {x {with {t x} {+ t 1}}} {+ 3 4}}" # free variable
tc7="{with {x 2} {+ {with {x 3} {+ x 1}} 4}}" # 7
tc8="{+ {- {+ 2 2} {+ {with {x 3} {+ 1 x}} 2}} 1}"
tc9="{the 3}"
tc10="{{fun {x} {+ x 1}} 7}"

tc11="{with {x 3} {with {f {fun {y} {+ x y}}} {with {x 5} {f 4}}}}"
tc12="{with {f {dsfun {y} {+ x y}}} {with {x 5} {f 4}}}"

echo "=== test case 1 === "
echo "${tc1}"
${test_prog} "${tc1}" 

echo "=== test case 2 === "
echo "${tc2}"
${test_prog} "${tc2}" 

echo "=== test case 3 === "
echo "${tc3}"
${test_prog} "${tc3}"

echo "=== test case 4 === "
echo "${tc4}"
${test_prog} "${tc4}"

echo "=== test case 5 === "
echo "${tc5}"
${test_prog} "${tc5}"

echo "=== test case 6 === "
echo "${tc6}"
${test_prog} "${tc6}"

echo "=== test case 7 === "
echo "${tc7}"
${test_prog} "${tc7}"

echo "=== test case 8 === "
echo "${tc8}"
${test_prog} "${tc8}"

echo "=== test case 9 === "
echo "${tc9}"
${test_prog} "${tc9}"

echo "=== test case 10 === "
echo "${tc10}"
${test_prog} "${tc10}"

echo "=== test case 11 === "
echo "${tc11}"
${test_prog} "${tc11}"

echo "=== test case 12 === "
echo "${tc12}"
${test_prog} "${tc12}"
