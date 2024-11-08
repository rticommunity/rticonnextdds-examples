#!/bin/bash

cd ${BaseExamplePath}/py
/bin/python3 primes_replier.py $@
rm -rf __pycache__
