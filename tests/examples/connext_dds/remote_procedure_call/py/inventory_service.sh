#!/bin/bash

cd ${BaseExamplePath}/py
/bin/python3 inventory_service.py $@
rm -rf __pycache__
