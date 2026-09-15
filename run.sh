#! /bin/bash

set -e

if [ -d "build" ]; then
	echo "Proceeding to build ... \n"
else
	echo "Build made... \n"
	mkdir build
fi

cd build
cmake ..
make -j4
./kompiler main.kol

# echo "................KOL LANG ....................."
# # the assembly part next
# nasm -f elf -g -F dwarf Assembly.asm 
# ld -m elf_i386 -s -o Assembly Assembly.o
#
# ./Assembly
