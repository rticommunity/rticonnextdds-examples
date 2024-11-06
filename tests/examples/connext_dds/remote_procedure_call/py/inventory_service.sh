#!/bin/bash

cd "$(dirname "$0")"
file_name=$(basename $0 .sh)
/bin/python3 $file_name.py $@
rm -rf __pycache__
