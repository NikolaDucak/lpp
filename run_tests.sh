#!/bin/bash
#https://stackoverflow.com/questions/691844/how-would-one-go-about-testing-an-interpreter-or-a-compiler

function fail {
	printf "\tTest %1 failed!"
	echo "\tDiff:"
	diff $2 $3
	exit 1
}

temp=$(tempfile)
for test in ./test/*.l; do	
	printf "%-5s %-30s ->" TEST: $test
	./build//lpp $test > $temp
	cmp -s "$test.out" "$temp" || fail $test $test.out $temp 
	printf " OK\n"
done

echo "--------- All OK ---------"
