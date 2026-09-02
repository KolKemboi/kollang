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
./kompiler
