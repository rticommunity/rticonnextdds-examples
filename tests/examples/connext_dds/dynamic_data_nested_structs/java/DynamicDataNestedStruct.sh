#!/bin/bash
cd /test/connextdds_examples_suite/connextdds_examples_repo/examples/connext_dds/dynamic_data_nested_structs/java
java -Djava.library.path=$NDDSHOME/lib/x64Linux4gcc7.3.0 -cp ".:$NDDSHOME/lib/java/nddsjava.jar" DynamicDataNestedStruct $@
