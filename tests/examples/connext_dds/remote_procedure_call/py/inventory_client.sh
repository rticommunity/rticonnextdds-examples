#!/bin/bash

cd /test/connextdds_examples_suite/rticonnextdds-examples/examples/connext_dds/remote_procedure_call/py
/bin/python3 inventory_client.py $@
rm -rf __pycache__
