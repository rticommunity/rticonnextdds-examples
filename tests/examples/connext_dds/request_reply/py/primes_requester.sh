#!/bin/bash

cd ${BaseExamplePath}/py
/bin/python3 primes_requester.py $@
rm -rf __pycache__
