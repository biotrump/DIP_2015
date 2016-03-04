#!/bin/bash
#list the read me
echo "---------------------------------------------------------------"
echo "dump the README, press enter to continue ...."
echo "---------------------------------------------------------------"
read
more README
echo "press enter to continue...."
read
mkdir -p build
cd build
cmake ..
make
#list the executables
echo "---------------------------------------------------------------"
echo "list the executables"
echo "---------------------------------------------------------------"
ls -l bin/
echo "press enter to continue...."
read
#flip
echo "---------------------------------------------------------------"
echo "flip raw_file to flip the raw_file horizontally and vertically"
echo "the output file is I_h.raw and I_v.raw respective."
echo "---------------------------------------------------------------"
ls -l I_h.raw
ls -l I_v.raw
#PROBLEM 1: IMAGE ENHANCEMENT
#PROBLEM 2: NOISE REMOVAL

