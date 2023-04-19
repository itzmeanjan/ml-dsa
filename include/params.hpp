#pragma once
#include "field.hpp"
#include <bit>
#include <cstddef>
#include <cstdint>

// Compile-time executable functions, ensuring that Dilithium DSA routines
// are always invoked with proper arguments.
namespace dilithium_params {

// Compile-time check to ensure that significant bit width of Z_q element
// doesn't cross maximum bit width of field prime q ( = 2^23 - 2^13 + 1 )
constexpr bool
check_sbw(const size_t sbw)
{
  constexpr size_t mbw = std::bit_width(field::Q - 1);
  return sbw <= mbw;
}

// Compile-time check to ensure that η ∈ {2, 4}, so that sampled secret key
// range stays short i.e. [-η, η]
constexpr bool
check_η(const uint32_t η)
{
  return (η == 2u) || (η == 4u);
}

// Compile-time check to ensure that starting nonce belongs to allowed set of
// values when uniform sampling polynomial coefficients in [-η, η]
constexpr bool
check_nonce(const size_t nonce)
{
  return (nonce == 0) || (nonce == 4) || (nonce == 5) || (nonce == 7);
}

// Compile-time check to ensure that γ1 has recommended value
constexpr bool
check_γ1(const uint32_t γ1)
{
  return (γ1 == (1u << 17)) || (γ1 == (1u << 19));
}

// Compile-time check to ensure that γ2 has recommended value
constexpr bool
check_γ2(const uint32_t γ2)
{
  return (γ2 == ((field::Q - 1) / 88)) || (γ2 == ((field::Q - 1) / 32));
}

// Compile-time check to ensure that τ is set to parameter recommended in
// Dilithium specification
constexpr bool
check_τ(const uint32_t τ)
{
  return (τ == 39) || (τ == 49) || (τ == 60);
}

// Compile-time check to ensure that number of bits to be dropped from a
// polynomial coefficient is supplied correctly.
constexpr bool
check_d(const size_t d)
{
  return d == 13;
}

}
