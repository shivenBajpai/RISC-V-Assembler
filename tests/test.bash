#!/bin/bash

tests=("example_one" "example_two" "example_three" "example_four" "all_instructions" "gcd")

echo "===== Running Tests ====="

for test in ${tests[@]}; do
    rm -f "outputs/${test}.txt"
    ../bin/riscv_asm -q -i "cases/${test}.s" -o "outputs/${test}.txt"
    #hd -e '1/4 "%08x" "\n"' "outputs/${test}.hex" &> "outputs/${test}.txt"

    if cmp -s "outputs/${test}.txt" "cases/expected_${test}.txt"; then
        printf "Case ${test}: PASS\n\n"
    
    else
        printf "Case ${test}: FAIL\n\n"
    
    fi
done

rm cleaned.s

echo "===== Finished Tests ====="