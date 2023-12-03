#pragma once
#include "shake128.hpp"
#include <array>
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
  shake128::shake128_t state;

public:
  inline prng_t()
  {
    std::array<uint8_t, 32> seed{};
    auto _seed = std::span(seed);

    // Read more @
    // https://en.cppreference.com/w/cpp/numeric/random/random_device/random_device
    std::random_device rd{};

    size_t off = 0;
    while (off < sizeof(seed)) {
      const uint32_t v = rd();
      std::memcpy(_seed.subspan(off, sizeof(v)).data(), &v, sizeof(v));

      off += sizeof(v);
    }

    state.absorb(_seed);
    state.finalize();
  }

  inline explicit constexpr prng_t(std::span<const uint8_t> seed)
  {
    state.absorb(seed);
    state.finalize();
  }

  inline void constexpr read(std::span<uint8_t> bytes) { state.squeeze(bytes); }
};

}
