#!/bin/bash

test_prog="./my_wae"
tc1="{+ 1 2}"
tc2="{with {x {with {t 3} {+ t 1}}} {+ 3 4}}"
tc3="{with {x 3} {+ x 1}}"
tc4="{+ {- {+ 2 2} {+ {- 2 4} 4}}}"
tc5="hel"
tc6="{with {x {with {t x} {+ t 1}}} {+ 3 4}}" # free variable
tc7="{with {x 2} {+ {with {x 3} {+ x 1}} 4}}" # 7

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
