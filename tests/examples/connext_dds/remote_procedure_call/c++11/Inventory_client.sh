#!/bin/bash

# Change directory to the location of the example working directory
# to workaround DIT-193

cd /test/connextdds_examples_suite/connextdds_examples_repo/examples/connext_dds/remote_procedure_call/c++11
./build/Inventory_client $@