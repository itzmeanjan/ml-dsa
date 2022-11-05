#!/usr/bin/python3

from ctypes import c_size_t, CDLL, c_char_p, create_string_buffer, c_bool
from posixpath import exists, abspath
from typing import Tuple

SO_PATH: str = abspath("../libdilithium.so")
assert exists(SO_PATH), "Use `make lib` to generate shared library object !"

SO_LIB: CDLL = CDLL(SO_PATH)


def keygen2(seed: bytes) -> Tuple[bytes, bytes]:
    """
    Given a 32 -bytes seed, this routine deterministically generates
    a Dilithium-2 key pair ( i.e. public key and secret key, in order )
    """
    pkey = create_string_buffer(1312)
    skey = create_string_buffer(2528)

    args = [c_char_p, c_char_p, c_char_p]
    SO_LIB.keygen2.argtypes = args

    SO_LIB.keygen2(seed, pkey, skey)

    return pkey.raw, skey.raw


def sign2(skey: bytes, msg: bytes, mlen: int) -> bytes:
    """
    Given a message of length mlen -bytes, this routine uses Dilithium-2 secret key
    to sign that message and returns signature.
    """
    sig = create_string_buffer(2420)

    args = [c_char_p, c_char_p, c_size_t, c_char_p]
    SO_LIB.sign2.argtypes = args

    SO_LIB.sign2(skey, msg, mlen, sig)

    return sig.raw


def verify2(pkey: bytes, msg: bytes, mlen: int, sig: bytes) -> bool:
    """
    Given a Dilithium-2 signature, along with original message ( of length mlen -bytes ),
    this routine uses public key to verify correctness of signature.
    """
    args = [c_char_p, c_char_p, c_size_t, c_char_p]
    SO_LIB.verify2.argtypes = args
    SO_LIB.verify2.restype = c_bool

    verified = SO_LIB.verify2(pkey, msg, mlen, sig)
    return verified


def keygen3(seed: bytes) -> Tuple[bytes, bytes]:
    """
    Given a 32 -bytes seed, this routine deterministically generates
    a Dilithium-3 key pair ( i.e. public key and secret key, in order )
    """
    pkey = create_string_buffer(1952)
    skey = create_string_buffer(4000)

    args = [c_char_p, c_char_p, c_char_p]
    SO_LIB.keygen3.argtypes = args

    SO_LIB.keygen3(seed, pkey, skey)

    return pkey.raw, skey.raw


def sign3(skey: bytes, msg: bytes, mlen: int) -> bytes:
    """
    Given a message of length mlen -bytes, this routine uses Dilithium-3 secret key
    to sign that message and returns signature.
    """
    sig = create_string_buffer(3293)

    args = [c_char_p, c_char_p, c_size_t, c_char_p]
    SO_LIB.sign3.argtypes = args

    SO_LIB.sign3(skey, msg, mlen, sig)

    return sig.raw


def verify3(pkey: bytes, msg: bytes, mlen: int, sig: bytes) -> bool:
    """
    Given a Dilithium-3 signature, along with original message ( of length mlen -bytes ),
    this routine uses public key to verify correctness of signature.
    """
    args = [c_char_p, c_char_p, c_size_t, c_char_p]
    SO_LIB.verify3.argtypes = args
    SO_LIB.verify3.restype = c_bool

    verified = SO_LIB.verify3(pkey, msg, mlen, sig)
    return verified


def keygen5(seed: bytes) -> Tuple[bytes, bytes]:
    """
    Given a 32 -bytes seed, this routine deterministically generates
    a Dilithium-5 key pair ( i.e. public key and secret key, in order )
    """
    pkey = create_string_buffer(2592)
    skey = create_string_buffer(4864)

    args = [c_char_p, c_char_p, c_char_p]
    SO_LIB.keygen5.argtypes = args

    SO_LIB.keygen5(seed, pkey, skey)

    return pkey.raw, skey.raw


def sign5(skey: bytes, msg: bytes, mlen: int) -> bytes:
    """
    Given a message of length mlen -bytes, this routine uses Dilithium-5 secret key
    to sign that message and returns signature.
    """
    sig = create_string_buffer(4595)

    args = [c_char_p, c_char_p, c_size_t, c_char_p]
    SO_LIB.sign5.argtypes = args

    SO_LIB.sign5(skey, msg, mlen, sig)

    return sig.raw


def verify5(pkey: bytes, msg: bytes, mlen: int, sig: bytes) -> bool:
    """
    Given a Dilithium-5 signature, along with original message ( of length mlen -bytes ),
    this routine uses public key to verify correctness of signature.
    """
    args = [c_char_p, c_char_p, c_size_t, c_char_p]
    SO_LIB.verify5.argtypes = args
    SO_LIB.verify5.restype = c_bool

    verified = SO_LIB.verify5(pkey, msg, mlen, sig)
    return verified


if __name__ == "__main__":
    print("Use `dilithium` as library module")
