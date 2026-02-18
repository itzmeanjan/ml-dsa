#pragma once
#include "ml_dsa/internals/utility/force_inline.hpp"
#include "sha3/sha3_224.hpp"
#include "sha3/sha3_256.hpp"
#include "sha3/sha3_384.hpp"
#include "sha3/sha3_512.hpp"
#include "sha3/shake128.hpp"
#include "sha3/shake256.hpp"
#include <array>
#include <cstddef>
#include <cstdint>
#include <span>

// Pre-hash utilities for HashML-DSA (FIPS 204, Section 5.4).
namespace ml_dsa_prehash {

// Supported pre-hash algorithms for HashML-DSA.
// Only SHA3-family algorithms (FIPS 202) are supported.
enum class hash_algorithm : uint8_t
{
  SHA3_224,
  SHA3_256,
  SHA3_384,
  SHA3_512,
  SHAKE_128,
  SHAKE_256,
};

// DER-encoded OID byte length. All SHA3/SHAKE OIDs are 11 bytes: tag(06) + length(09) + 9 value bytes.
static constexpr size_t OID_BYTE_LEN = 11;

// Returns the DER-encoded OID for the given hash algorithm.
//
// OID arc: 2.16.840.1.101.3.4.2.{7..12}
//
// SHA3-224: ...07,
// SHA3-256: ...08,
// SHA3-384: ...09,
// SHA3-512: ...0A
// SHAKE-128: ...0B,
// SHAKE-256: ...0C
static constexpr std::array<uint8_t, OID_BYTE_LEN>
oid_of(hash_algorithm alg)
{
  std::array<uint8_t, OID_BYTE_LEN> result{ 0x06, 0x09, 0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x00 };

  switch (alg) {
    case hash_algorithm::SHA3_224:
      result[10] = 0x07;
      break;
    case hash_algorithm::SHA3_256:
      result[10] = 0x08;
      break;
    case hash_algorithm::SHA3_384:
      result[10] = 0x09;
      break;
    case hash_algorithm::SHA3_512:
      result[10] = 0x0A;
      break;
    case hash_algorithm::SHAKE_128:
      result[10] = 0x0B;
      break;
    case hash_algorithm::SHAKE_256:
      result[10] = 0x0C;
      break;
  }

  return result;
}

// Maximum possible digest byte length across all supported hash algorithms.
static constexpr size_t MAX_DIGEST_BYTE_LEN = 64;

// Computes PH(M) for the given hash algorithm, writing into a fixed-size buffer.
// Returns the actual number of digest bytes written.
//
// For two XOFs, SHAKE-128 output = 32 bytes, SHAKE-256 output = 64 bytes, per FIPS 204 Table 2.
template<hash_algorithm alg>
static forceinline constexpr size_t
compute_prehash(std::span<const uint8_t> msg, std::span<uint8_t, MAX_DIGEST_BYTE_LEN> out)
{
  if constexpr (alg == hash_algorithm::SHA3_224) {
    sha3_224::sha3_224_t hasher;
    std::array<uint8_t, sha3_224::DIGEST_LEN> digest{};

    hasher.absorb(msg);
    hasher.finalize();
    hasher.digest(digest);
    std::copy(digest.begin(), digest.end(), out.begin());

    return sha3_224::DIGEST_LEN;
  } else if constexpr (alg == hash_algorithm::SHA3_256) {
    sha3_256::sha3_256_t hasher;
    std::array<uint8_t, sha3_256::DIGEST_LEN> digest{};

    hasher.absorb(msg);
    hasher.finalize();
    hasher.digest(digest);
    std::copy(digest.begin(), digest.end(), out.begin());

    return sha3_256::DIGEST_LEN;
  } else if constexpr (alg == hash_algorithm::SHA3_384) {
    sha3_384::sha3_384_t hasher;
    std::array<uint8_t, sha3_384::DIGEST_LEN> digest{};

    hasher.absorb(msg);
    hasher.finalize();
    hasher.digest(digest);
    std::copy(digest.begin(), digest.end(), out.begin());

    return sha3_384::DIGEST_LEN;
  } else if constexpr (alg == hash_algorithm::SHA3_512) {
    sha3_512::sha3_512_t hasher;
    std::array<uint8_t, sha3_512::DIGEST_LEN> digest{};

    hasher.absorb(msg);
    hasher.finalize();
    hasher.digest(digest);
    std::copy(digest.begin(), digest.end(), out.begin());

    return sha3_512::DIGEST_LEN;
  } else if constexpr (alg == hash_algorithm::SHAKE_128) {
    constexpr size_t OUTPUT_LEN = 32;
    shake128::shake128_t hasher;
    std::array<uint8_t, OUTPUT_LEN> digest{};

    hasher.absorb(msg);
    hasher.finalize();
    hasher.squeeze(digest);
    std::copy(digest.begin(), digest.end(), out.begin());

    return OUTPUT_LEN;
  } else if constexpr (alg == hash_algorithm::SHAKE_256) {
    constexpr size_t OUTPUT_LEN = 64;
    shake256::shake256_t hasher;
    std::array<uint8_t, OUTPUT_LEN> digest{};

    hasher.absorb(msg);
    hasher.finalize();
    hasher.squeeze(digest);
    std::copy(digest.begin(), digest.end(), out.begin());

    return OUTPUT_LEN;
  }
}

}
