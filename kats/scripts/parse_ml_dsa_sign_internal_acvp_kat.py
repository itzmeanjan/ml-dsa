#!/usr/bin/python

import json
import sys
import typing

ML_DSA_44_ACVP_KAT_FILE_NAME="ml_dsa_44_sign_internal.acvp.kat"
ML_DSA_65_ACVP_KAT_FILE_NAME="ml_dsa_65_sign_internal.acvp.kat"
ML_DSA_87_ACVP_KAT_FILE_NAME="ml_dsa_87_sign_internal.acvp.kat"

def extract_and_write_ml_dsa_sign_internal_kats(test_groups: list[dict[str, typing.Any]], write_to_file: str):
    with open(write_to_file, "wt") as fd:
        for test_group in test_groups:
            assert test_group["testType"] == "AFT"
            assert test_group["signatureInterface"] == "internal"
            assert test_group["preHash"] == "none"
            assert test_group["externalMu"] == True

            for test in test_group["tests"]:
                fd.write(f'mu = {test["mu"]}\n')
                fd.write(f'pkey = {test["pk"]}\n')
                fd.write(f'skey = {test["sk"]}\n')
  
                if test_group["deterministic"]:
                    fd.write(f'rnd = {"00" * 32}\n')
                else:
                    fd.write(f'rnd = {test["rnd"]}\n')
  
                fd.write(f'sig = {test["signature"]}\n')
                fd.write('\n')

        fd.flush()


def main():
    json_as_str = ''
    for line in sys.stdin:
        json_as_str += line
    
    acvp_kats = json.loads(json_as_str)

    ml_dsa_44_param_set = [acvp_kats["testGroups"][6], acvp_kats["testGroups"][18]]
    ml_dsa_65_param_set = [acvp_kats["testGroups"][8], acvp_kats["testGroups"][20]]
    ml_dsa_87_param_set = [acvp_kats["testGroups"][10], acvp_kats["testGroups"][22]]

    extract_and_write_ml_dsa_sign_internal_kats(ml_dsa_44_param_set, ML_DSA_44_ACVP_KAT_FILE_NAME)
    extract_and_write_ml_dsa_sign_internal_kats(ml_dsa_65_param_set, ML_DSA_65_ACVP_KAT_FILE_NAME)
    extract_and_write_ml_dsa_sign_internal_kats(ml_dsa_87_param_set, ML_DSA_87_ACVP_KAT_FILE_NAME)


if __name__=='__main__':
    main()
