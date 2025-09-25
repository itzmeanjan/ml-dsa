#!/bin/bash

ACVP_SERVER_REPO_PATH="ACVP-Server"
if [ -d "$ACVP_SERVER_REPO_PATH" ]; then
    echo "> $ACVP_SERVER_REPO_PATH repository exists, let's just fetch latest."

    pushd $ACVP_SERVER_REPO_PATH
    git checkout master
    git fetch
    git pull origin master
    popd
else
    echo "> $ACVP_SERVER_REPO_PATH repository doesn't exist, let's clone it."
    git clone https://github.com/usnistgov/ACVP-Server.git
fi

cat ./ACVP-Server/gen-val/json-files/ML-DSA-keyGen-FIPS204/internalProjection.json | python parse_ml_dsa_keygen_acvp_kat.py
cat ./ACVP-Server/gen-val/json-files/ML-DSA-sigGen-FIPS204/internalProjection.json | python parse_ml_dsa_sign_acvp_kat.py
cat ./ACVP-Server/gen-val/json-files/ML-DSA-sigGen-FIPS204/internalProjection.json | python parse_ml_dsa_sign_internal_acvp_kat.py
cat ./ACVP-Server/gen-val/json-files/ML-DSA-sigVer-FIPS204/internalProjection.json | python parse_ml_dsa_verify_acvp_kat.py

mv *.acvp.kat ..

echo "> Generated all NIST ACVP KATs."
