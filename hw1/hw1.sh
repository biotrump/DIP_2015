#!/bin/bash
#list the read me
more README
read
mkdir -p build
cd build
cmake ..
make
#list the executables
ls -l bin/
echo "flip raw_file to flip the raw_file horizontally and vertically"
echo "the output file is I_h.raw and I_v.raw respective."
ls -l I_h.raw
ls -l I_v.raw

