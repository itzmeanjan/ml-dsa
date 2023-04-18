#!/bin/bash

# 1. Make shared library object
make lib

# 2. Run test cases
pushd wrapper/python
cp ../../kats/dilithium{2,3,5}.kat .
python3 -m pytest -k test_dilithium -v
rm dilithium{2,3,5}.kat
popd
