#include "dilithium.hpp"

// Thin C wrapper on top of underlying C++ implementation of Dilithium{2, 3, 5}
// digital signature algorithm (DSA), which can be used for producing shared
// library object with conformant C-ABI & interfaced from other languages such
// as Rust, Python etc.
//
// This interface is being exposed for testing this implementation's functional
// correctness and conformance with Dilithium specification
// https://pq-crystals.org/dilithium/data/dilithium-specification-round3-20210208.pdf
// using known answer tests produced using
// https://gist.github.com/itzmeanjan/d14afc3866b82119221682f0f3c9822d

// Function prototypes
extern "C"
{

  // Dilithium-2 key pair generation
  void keygen2(const uint8_t* const __restrict seed,
               uint8_t* const __restrict pubkey,
               uint8_t* const __restrict seckey);

  // Dilithium-2 message signing algorithm
  void sign2(const uint8_t* const __restrict seckey,
             const uint8_t* const __restrict msg,
             const size_t mlen,
             uint8_t* const __restrict sig);

  // Dilithium-2 signature verification algorithm
  bool verify2(const uint8_t* const __restrict pubkey,
               const uint8_t* const __restrict msg,
               const size_t mlen,
               const uint8_t* const __restrict sig);

  // Dilithium-3 key pair generation
  void keygen3(const uint8_t* const __restrict seed,
               uint8_t* const __restrict pubkey,
               uint8_t* const __restrict seckey);

  // Dilithium-3 message signing algorithm
  void sign3(const uint8_t* const __restrict seckey,
             const uint8_t* const __restrict msg,
             const size_t mlen,
             uint8_t* const __restrict sig);

  // Dilithium-3 signature verification algorithm
  bool verify3(const uint8_t* const __restrict pubkey,
               const uint8_t* const __restrict msg,
               const size_t mlen,
               const uint8_t* const __restrict sig);

  // Dilithium-5 key pair generation
  void keygen5(const uint8_t* const __restrict seed,
               uint8_t* const __restrict pubkey,
               uint8_t* const __restrict seckey);

  // Dilithium-5 message signing algorithm
  void sign5(const uint8_t* const __restrict seckey,
             const uint8_t* const __restrict msg,
             const size_t mlen,
             uint8_t* const __restrict sig);

  // Dilithium-5 signature verification algorithm
  bool verify5(const uint8_t* const __restrict pubkey,
               const uint8_t* const __restrict msg,
               const size_t mlen,
               const uint8_t* const __restrict sig);
}

// Function implementations
extern "C"
{

  void keygen2(const uint8_t* const __restrict seed,
               uint8_t* const __restrict pubkey,
               uint8_t* const __restrict seckey)
  {
    dilithium::keygen<4, 4, 13, 2>(seed, pubkey, seckey);
  }

  void sign2(const uint8_t* const __restrict seckey,
             const uint8_t* const __restrict msg,
             const size_t mlen,
             uint8_t* const __restrict sig)
  {
    constexpr size_t k = 4;
    constexpr size_t l = 4;
    constexpr uint32_t η = 2;
    constexpr size_t d = 13;
    constexpr uint32_t γ1 = 1u << 17;
    constexpr uint32_t γ2 = (ff::Q - 1) / 88;
    constexpr uint32_t τ = 39;
    constexpr uint32_t β = τ * η;
    constexpr size_t ω = 80;

    dilithium::sign<k, l, d, η, γ1, γ2, τ, β, ω>(seckey, msg, mlen, sig);
  }

  bool verify2(const uint8_t* const __restrict pubkey,
               const uint8_t* const __restrict msg,
               const size_t mlen,
               const uint8_t* const __restrict sig)
  {
    constexpr size_t k = 4;
    constexpr size_t l = 4;
    constexpr uint32_t η = 2;
    constexpr size_t d = 13;
    constexpr uint32_t γ1 = 1u << 17;
    constexpr uint32_t γ2 = (ff::Q - 1) / 88;
    constexpr uint32_t τ = 39;
    constexpr uint32_t β = τ * η;
    constexpr size_t ω = 80;

    return dilithium::verify<k, l, d, γ1, γ2, τ, β, ω>(pubkey, msg, mlen, sig);
  }

  void keygen3(const uint8_t* const __restrict seed,
               uint8_t* const __restrict pubkey,
               uint8_t* const __restrict seckey)
  {
    dilithium::keygen<6, 5, 13, 4>(seed, pubkey, seckey);
  }

  void sign3(const uint8_t* const __restrict seckey,
             const uint8_t* const __restrict msg,
             const size_t mlen,
             uint8_t* const __restrict sig)
  {
    constexpr size_t k = 6;
    constexpr size_t l = 5;
    constexpr uint32_t η = 4;
    constexpr size_t d = 13;
    constexpr uint32_t γ1 = 1u << 19;
    constexpr uint32_t γ2 = (ff::Q - 1) / 32;
    constexpr uint32_t τ = 49;
    constexpr uint32_t β = τ * η;
    constexpr size_t ω = 55;

    dilithium::sign<k, l, d, η, γ1, γ2, τ, β, ω>(seckey, msg, mlen, sig);
  }

  bool verify3(const uint8_t* const __restrict pubkey,
               const uint8_t* const __restrict msg,
               const size_t mlen,
               const uint8_t* const __restrict sig)
  {
    constexpr size_t k = 6;
    constexpr size_t l = 5;
    constexpr uint32_t η = 4;
    constexpr size_t d = 13;
    constexpr uint32_t γ1 = 1u << 19;
    constexpr uint32_t γ2 = (ff::Q - 1) / 32;
    constexpr uint32_t τ = 49;
    constexpr uint32_t β = τ * η;
    constexpr size_t ω = 55;

    return dilithium::verify<k, l, d, γ1, γ2, τ, β, ω>(pubkey, msg, mlen, sig);
  }

  void keygen5(const uint8_t* const __restrict seed,
               uint8_t* const __restrict pubkey,
               uint8_t* const __restrict seckey)
  {
    dilithium::keygen<8, 7, 13, 2>(seed, pubkey, seckey);
  }

  void sign5(const uint8_t* const __restrict seckey,
             const uint8_t* const __restrict msg,
             const size_t mlen,
             uint8_t* const __restrict sig)
  {
    constexpr size_t k = 8;
    constexpr size_t l = 7;
    constexpr uint32_t η = 2;
    constexpr size_t d = 13;
    constexpr uint32_t γ1 = 1u << 19;
    constexpr uint32_t γ2 = (ff::Q - 1) / 32;
    constexpr uint32_t τ = 60;
    constexpr uint32_t β = τ * η;
    constexpr size_t ω = 75;

    dilithium::sign<k, l, d, η, γ1, γ2, τ, β, ω>(seckey, msg, mlen, sig);
  }

  bool verify5(const uint8_t* const __restrict pubkey,
               const uint8_t* const __restrict msg,
               const size_t mlen,
               const uint8_t* const __restrict sig)
  {
    constexpr size_t k = 8;
    constexpr size_t l = 7;
    constexpr uint32_t η = 2;
    constexpr size_t d = 13;
    constexpr uint32_t γ1 = 1u << 19;
    constexpr uint32_t γ2 = (ff::Q - 1) / 32;
    constexpr uint32_t τ = 60;
    constexpr uint32_t β = τ * η;
    constexpr size_t ω = 75;

    return dilithium::verify<k, l, d, γ1, γ2, τ, β, ω>(pubkey, msg, mlen, sig);
  }
}
