#!/usr/bin/python

import json
import sys
import typing

ML_DSA_44_ACVP_KAT_FILE_NAME="ml_dsa_44_keygen.acvp.kat"
ML_DSA_65_ACVP_KAT_FILE_NAME="ml_dsa_65_keygen.acvp.kat"
ML_DSA_87_ACVP_KAT_FILE_NAME="ml_dsa_87_keygen.acvp.kat"

def extract_and_write_ml_dsa_keygen_kats(test_group: dict[str, typing.Any], write_to_file: str):
    assert test_group["testType"] == "AFT"

    with open(write_to_file, "wt") as fd:
        for test in test_group["tests"]:
            fd.write(f'seed = {test["seed"]}\n')
            fd.write(f'pkey = {test["pk"]}\n')
            fd.write(f'skey = {test["sk"]}\n')

            fd.write('\n')
        
        fd.flush()


def main():
    json_as_str = ''
    for line in sys.stdin:
        json_as_str += line
    
    acvp_kats = json.loads(json_as_str)

    ml_dsa_44_param_set = acvp_kats["testGroups"][0]
    ml_dsa_65_param_set = acvp_kats["testGroups"][1]
    ml_dsa_87_param_set = acvp_kats["testGroups"][2]

    extract_and_write_ml_dsa_keygen_kats(ml_dsa_44_param_set, ML_DSA_44_ACVP_KAT_FILE_NAME)
    extract_and_write_ml_dsa_keygen_kats(ml_dsa_65_param_set, ML_DSA_65_ACVP_KAT_FILE_NAME)
    extract_and_write_ml_dsa_keygen_kats(ml_dsa_87_param_set, ML_DSA_87_ACVP_KAT_FILE_NAME)


if __name__=='__main__':
    main()
