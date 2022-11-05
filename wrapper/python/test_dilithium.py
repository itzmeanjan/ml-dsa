#!/usr/bin/python3

import dilithium


def test_dilithium2():
    """
    Ensures correctness and compatibility of this Dilithium2 DSA implementation,
    against original reference implementation https://github.com/pq-crystals/dilithium

    Known Answer Tests generated using https://gist.github.com/itzmeanjan/d14afc3866b82119221682f0f3c9822d
    """
    with open("dilithium2.kat", "r") as fd:
        while True:
            seed = fd.readline()
            if not seed:
                # no more KAT
                break

            pkey = fd.readline()
            skey = fd.readline()
            mlen = fd.readline()
            msg = fd.readline()
            sig = fd.readline()

            # extract out required fields
            seed = [i.strip() for i in seed.split("=")][-1]
            pkey = [i.strip() for i in pkey.split("=")][-1]
            skey = [i.strip() for i in skey.split("=")][-1]
            mlen = [i.strip() for i in mlen.split("=")][-1]
            msg = [i.strip() for i in msg.split("=")][-1]
            sig = [i.strip() for i in sig.split("=")][-1]

            # convert input hex strings to bytes object
            seed = bytes(
                [
                    int(f"0x{seed[(i << 1): ((i+1) << 1)]}", base=16)
                    for i in range(len(seed) >> 1)
                ]
            )
            pkey = bytes(
                [
                    int(f"0x{pkey[(i << 1): ((i+1) << 1)]}", base=16)
                    for i in range(len(pkey) >> 1)
                ]
            )
            skey = bytes(
                [
                    int(f"0x{skey[(i << 1): ((i+1) << 1)]}", base=16)
                    for i in range(len(skey) >> 1)
                ]
            )
            mlen = int(mlen)
            msg = bytes(
                [int(f"0x{msg[(i << 1): ((i+1) << 1)]}", base=16) for i in range(mlen)]
            )
            sig = bytes(
                [
                    int(f"0x{sig[(i << 1): ((i+1) << 1)]}", base=16)
                    for i in range(len(sig) >> 1)
                ]
            )

            (pk, sk) = dilithium.keygen2(seed)
            sig_ = dilithium.sign2(skey, msg, mlen)
            verified = dilithium.verify2(pkey, msg, mlen, sig_)

            assert verified, "[Dilithium-2] Failed to verify signature"
            assert pk == pkey, "[Dilithium-2] Public key doesn't match"
            assert sk == skey, "[Dilithium-2] Secret key doesn't match"
            assert sig == sig_, "[Dilithium-2] Signature doesn't match"

            fd.readline()


def test_dilithium3():
    """
    Ensures correctness and compatibility of this Dilithium3 DSA implementation,
    against original reference implementation https://github.com/pq-crystals/dilithium

    Known Answer Tests generated using https://gist.github.com/itzmeanjan/d14afc3866b82119221682f0f3c9822d
    """
    with open("dilithium3.kat", "r") as fd:
        while True:
            seed = fd.readline()
            if not seed:
                # no more KAT
                break

            pkey = fd.readline()
            skey = fd.readline()
            mlen = fd.readline()
            msg = fd.readline()
            sig = fd.readline()

            # extract out required fields
            seed = [i.strip() for i in seed.split("=")][-1]
            pkey = [i.strip() for i in pkey.split("=")][-1]
            skey = [i.strip() for i in skey.split("=")][-1]
            mlen = [i.strip() for i in mlen.split("=")][-1]
            msg = [i.strip() for i in msg.split("=")][-1]
            sig = [i.strip() for i in sig.split("=")][-1]

            # convert input hex strings to bytes object
            seed = bytes(
                [
                    int(f"0x{seed[(i << 1): ((i+1) << 1)]}", base=16)
                    for i in range(len(seed) >> 1)
                ]
            )
            pkey = bytes(
                [
                    int(f"0x{pkey[(i << 1): ((i+1) << 1)]}", base=16)
                    for i in range(len(pkey) >> 1)
                ]
            )
            skey = bytes(
                [
                    int(f"0x{skey[(i << 1): ((i+1) << 1)]}", base=16)
                    for i in range(len(skey) >> 1)
                ]
            )
            mlen = int(mlen)
            msg = bytes(
                [int(f"0x{msg[(i << 1): ((i+1) << 1)]}", base=16) for i in range(mlen)]
            )
            sig = bytes(
                [
                    int(f"0x{sig[(i << 1): ((i+1) << 1)]}", base=16)
                    for i in range(len(sig) >> 1)
                ]
            )

            (pk, sk) = dilithium.keygen3(seed)
            sig_ = dilithium.sign3(skey, msg, mlen)
            verified = dilithium.verify3(pkey, msg, mlen, sig_)

            assert verified, "[Dilithium-3] Failed to verify signature"
            assert pk == pkey, "[Dilithium-3] Public key doesn't match"
            assert sk == skey, "[Dilithium-3] Secret key doesn't match"
            assert sig == sig_, "[Dilithium-3] Signature doesn't match"

            fd.readline()


def test_dilithium5():
    """
    Ensures correctness and compatibility of this Dilithium5 DSA implementation,
    against original reference implementation https://github.com/pq-crystals/dilithium

    Known Answer Tests generated using https://gist.github.com/itzmeanjan/d14afc3866b82119221682f0f3c9822d
    """
    with open("dilithium5.kat", "r") as fd:
        while True:
            seed = fd.readline()
            if not seed:
                # no more KAT
                break

            pkey = fd.readline()
            skey = fd.readline()
            mlen = fd.readline()
            msg = fd.readline()
            sig = fd.readline()

            # extract out required fields
            seed = [i.strip() for i in seed.split("=")][-1]
            pkey = [i.strip() for i in pkey.split("=")][-1]
            skey = [i.strip() for i in skey.split("=")][-1]
            mlen = [i.strip() for i in mlen.split("=")][-1]
            msg = [i.strip() for i in msg.split("=")][-1]
            sig = [i.strip() for i in sig.split("=")][-1]

            # convert input hex strings to bytes object
            seed = bytes(
                [
                    int(f"0x{seed[(i << 1): ((i+1) << 1)]}", base=16)
                    for i in range(len(seed) >> 1)
                ]
            )
            pkey = bytes(
                [
                    int(f"0x{pkey[(i << 1): ((i+1) << 1)]}", base=16)
                    for i in range(len(pkey) >> 1)
                ]
            )
            skey = bytes(
                [
                    int(f"0x{skey[(i << 1): ((i+1) << 1)]}", base=16)
                    for i in range(len(skey) >> 1)
                ]
            )
            mlen = int(mlen)
            msg = bytes(
                [int(f"0x{msg[(i << 1): ((i+1) << 1)]}", base=16) for i in range(mlen)]
            )
            sig = bytes(
                [
                    int(f"0x{sig[(i << 1): ((i+1) << 1)]}", base=16)
                    for i in range(len(sig) >> 1)
                ]
            )

            (pk, sk) = dilithium.keygen5(seed)
            sig_ = dilithium.sign5(skey, msg, mlen)
            verified = dilithium.verify5(pkey, msg, mlen, sig_)

            assert verified, "[Dilithium-5] Failed to verify signature"
            assert pk == pkey, "[Dilithium-5] Public key doesn't match"
            assert sk == skey, "[Dilithium-5] Secret key doesn't match"
            assert sig == sig_, "[Dilithium-5] Signature doesn't match"

            fd.readline()


if __name__ == "__main__":
    print("Use `pytest` for running test cases")
