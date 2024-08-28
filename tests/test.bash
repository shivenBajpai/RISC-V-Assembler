#!/bin/bash

tests=("example_one" "example_two" "example_three" "example_four" "gcd")

for test in ${tests[@]}; do
    ../bin/riscv_asm -q -i "cases/${test}.s" -o "outputs/${test}.hex"
    hd -e '1/4 "%08x" "\n"' "outputs/${test}.hex" > "outputs/${test}.txt"

    if cmp -s "outputs/${test}.txt" "cases/expected_${test}.txt"; then
        echo "Case ${test}: PASS"
    
    else
        echo "Case ${test}: FAIL"
    
    fi
done

rm cleaned.s