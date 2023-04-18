#pragma once
#include "shake128.hpp"
#include <random>

// Pseudo Random Number Generator
namespace prng {

// Pseudo Random Number Generator s.t. N (>0) -many random bytes are read
// from SHAKE128 XOF state, by calling it arbitrary many times, s.t. SHAKE128
// state is obtained by
//
// - either absorbing 32 -bytes, sampled using std::random_device ( default )
// - or absorbing M(>0) -bytes, supplied as argument ( explicit )
//
// Note, std::random_device's behaviour is implementation defined feature, so
// this PRNG implementation doesn't guarantee that it'll generate cryptographic
// secure random bytes if you opt for using default constructor of this struct.
//
// I suggest you read
// https://en.cppreference.com/w/cpp/numeric/random/random_device/random_device
// before using default constructor. When using explicit constructor, it's
// your responsibility to supply M -many random seed bytes.
//
// This implementation is taken from
// https://github.com/itzmeanjan/kyber/blob/7b107470/include/prng.hpp
struct prng_t
{
private:
  shake128::shake128<false> state;

public:
  inline prng_t()
  {
    uint8_t seed[32];

    // Read more @
    // https://en.cppreference.com/w/cpp/numeric/random/random_device/random_device
    std::random_device rd{};

    size_t off = 0;
    while (off < sizeof(seed)) {
      const uint32_t v = rd();
      std::memcpy(seed + off, &v, sizeof(v));

      off += sizeof(v);
    }

    state.hash(seed, sizeof(seed));
  }

  inline explicit prng_t(const uint8_t* const seed, const size_t slen)
  {
    state.hash(seed, slen);
  }

  inline void read(uint8_t* const bytes, const size_t len)
  {
    state.read(bytes, len);
  }
};

}
