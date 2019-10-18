#!/bin/bash

# re-initialize large_file to be 50 MiB
dd if=/dev/urandom of=large_file bs=1048576 count=50 &>/dev/null

echo -e "\nOriginal file contents:"
echo -e "\t---"
head large_file -n 1
echo -e "\t---"

./mmap_concept

echo -e "\nNew file contents:"
echo -e "\t---"
head large_file -n 1
echo -e "\t---"
