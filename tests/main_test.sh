#!/bin/sh

success() { printf '\033[1;32msuccess\033[0m\n'; }
failure() { printf '\033[1;33mfailure\033[0m\n'; exit 1; }

make

for file in code/*.sten; do
    printf '\033[1m%s\033[0m\n' "$file"
    ./stencc < "$file" && success || failure
done

for file in code/error/*.sten; do
    printf '\033[1m%s\033[0m\n' "$file"
    ./stencc < "$file" && failure || success
done

for file in code/*.sten; do
    printf '\033[1m%s\033[0m\n' "$file"
    ./stencc < "$file" | grep -A1000 MIPS > mips.out
    spim -file mips.out
done
