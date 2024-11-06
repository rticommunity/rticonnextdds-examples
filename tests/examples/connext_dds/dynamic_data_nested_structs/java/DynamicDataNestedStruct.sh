#!/bin/bash

cd "$(dirname "$0")"
file_name=$(basename $0 .sh)
java -Djava.library.path=$NDDSHOME/lib/x64Linux4gcc7.3.0 -cp ".:$NDDSHOME/lib/java/nddsjava.jar" $file_name $@
