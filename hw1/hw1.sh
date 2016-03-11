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
