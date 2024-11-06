#!/bin/bash

# Change directory to the location of the example working directory
# to workaround DIT-193

cd "$(dirname "$0")"
file_name=$(basename $0 .sh)
./build/$file_name $@