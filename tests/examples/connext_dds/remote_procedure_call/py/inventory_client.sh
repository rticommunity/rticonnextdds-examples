#!/bin/bash

cd ${BaseExamplePath}/py
/bin/python3 inventory_client.py $@
rm -rf __pycache__
