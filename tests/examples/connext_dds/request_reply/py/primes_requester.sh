#!/bin/bash

cd /test/connextdds_examples_suite/rticonnextdds-examples/examples/connext_dds/request_reply/py
/bin/python3 primes_requester.py $@
rm -rf __pycache__
