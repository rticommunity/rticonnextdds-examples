#!/bin/bash

cd /test/connextdds_examples_suite/connextdds_examples_repo/examples/connext_dds/remote_procedure_call/py
/bin/python3 inventory_service.py $@
rm -rf __pycache__
