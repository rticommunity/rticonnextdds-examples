#!/bin/bash
cd ${BaseExamplePath}/java
java -Djava.library.path=$NDDSHOME/lib/x64Linux4gcc7.3.0 -cp ".:$NDDSHOME/lib/java/nddsjava.jar" DynamicDataNestedStruct $@
