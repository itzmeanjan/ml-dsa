#!/usr/bin/python

import json
import sys
import typing

ML_DSA_44_ACVP_KAT_FILE_NAME="ml_dsa_44_hash_verify.acvp.kat"
ML_DSA_65_ACVP_KAT_FILE_NAME="ml_dsa_65_hash_verify.acvp.kat"
ML_DSA_87_ACVP_KAT_FILE_NAME="ml_dsa_87_hash_verify.acvp.kat"

SHA3_FAMILY_HASH_ALGS = {"SHA3-224", "SHA3-256", "SHA3-384", "SHA3-512", "SHAKE-128", "SHAKE-256"}

def extract_and_write_ml_dsa_hash_verify_kats(test_group: dict[str, typing.Any], write_to_file: str):
    with open(write_to_file, "wt") as fd:
        assert test_group["testType"] == "AFT"
        assert test_group["signatureInterface"] == "external"
        assert test_group["preHash"] == "preHash"
        assert test_group["externalMu"] == False

        for test in test_group["tests"]:
            if test["hashAlg"] not in SHA3_FAMILY_HASH_ALGS:
                continue

            fd.write(f'msg = {test["message"]}\n')
            fd.write(f'pkey = {test["pk"]}\n')
            fd.write(f'skey = {test["sk"]}\n')
            fd.write(f'ctx = {test["context"]}\n')
            fd.write(f'hashAlg = {test["hashAlg"]}\n')
            fd.write(f'sig = {test["signature"]}\n')
            fd.write(f'testPassed = {test["testPassed"]}\n')
            fd.write(f'reason = {test["reason"]}\n')

            fd.write('\n')

        fd.flush()


def main():
    json_as_str = ''
    for line in sys.stdin:
        json_as_str += line

    acvp_kats = json.loads(json_as_str)

    ml_dsa_44_param_set = acvp_kats["testGroups"][1]
    ml_dsa_65_param_set = acvp_kats["testGroups"][3]
    ml_dsa_87_param_set = acvp_kats["testGroups"][5]

    extract_and_write_ml_dsa_hash_verify_kats(ml_dsa_44_param_set, ML_DSA_44_ACVP_KAT_FILE_NAME)
    extract_and_write_ml_dsa_hash_verify_kats(ml_dsa_65_param_set, ML_DSA_65_ACVP_KAT_FILE_NAME)
    extract_and_write_ml_dsa_hash_verify_kats(ml_dsa_87_param_set, ML_DSA_87_ACVP_KAT_FILE_NAME)


if __name__=='__main__':
    main()
