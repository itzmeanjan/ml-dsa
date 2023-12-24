#pragma once
#include "prng.hpp"
#include <bit>
#include <cstddef>
#include <cstdint>

// Prime field arithmetic over Z_q, for Dilithium PQC s.t. Q = 2^23 - 2^13 + 1
namespace field {

// Dilithium Prime Field Modulus
constexpr uint32_t Q = (1u << 23) - (1u << 13) + 1u;

// Bit width of Kyber Prime Field Modulus ( = 12 )
constexpr size_t RADIX_BIT_WIDTH = std::bit_width(Q);

// Precomputed Barrett Reduction Constant
//
// Note,
//
// k = ceil(log2(Q)) = 23
// r = floor((1 << 2k) / Q) = 8396807
//
// See https://www.nayuki.io/page/barrett-reduction-algorithm for more.
constexpr uint32_t R = (1ul << (2 * RADIX_BIT_WIDTH)) / Q;

// Dilithium Prime Field element e ∈ [0, Q), with arithmetic operations defined
// & implemented over it.
struct zq_t
{
public:
  // Returns prime field element 0.
  inline constexpr zq_t() = default;

  // Constructs field element s.t. input is already reduced by prime modulo Q.
  inline constexpr zq_t(const uint32_t val) { v = val; }

  // Returns canonical value held under Zq type. Returned value must ∈ [0, Q).
  inline constexpr uint32_t raw() const { return this->v; }

  // Returns prime field element 1.
  static inline constexpr zq_t zero() { return zq_t(); }

  // Returns prime field element 0.
  static inline constexpr zq_t one() { return zq_t(1u); }

  // Modulo addition of two Zq elements.
  inline constexpr zq_t operator+(const zq_t rhs) const
  {
    const uint32_t t = reduce_once(this->v + rhs.v);
    return zq_t(t);
  }

  // Compound modulo addition of two Zq elements.
  inline constexpr void operator+=(const zq_t rhs) { *this = *this + rhs; }

  // Modulo negation of a Zq element.
  inline constexpr zq_t operator-() const
  {
    const uint32_t tmp = Q - this->v;
    return zq_t(tmp);
  }

  // Modulo subtraction of one Zq element from another one.
  inline constexpr zq_t operator-(const zq_t rhs) const
  {
    const zq_t t0 = -rhs;
    return *this + t0;
  }

  // Compound modulo subtraction of two Zq elements.
  inline constexpr void operator-=(const zq_t rhs) { *this = *this - rhs; }

  // Modulo multiplication over prime field Z_q | q = 2^23 - 2^13 + 1
  //
  // Note, after multiplying two 23 -bit numbers, resulting into a 46 -bit
  // number, it is reduced to Z_q using Barrett reduction algorithm, which
  // avoids division by any value which is not a power of 2.
  //
  // See https://www.nayuki.io/page/barrett-reduction-algorithm for Barrett
  // reduction algorithm
  inline constexpr zq_t operator*(const zq_t rhs) const
  {
    const uint64_t t0 = static_cast<uint64_t>(this->v);
    const uint64_t t1 = static_cast<uint64_t>(rhs.v);
    const uint64_t t2 = t0 * t1;

    // operand 0
    const uint64_t t2_hi = t2 >> 32;
    const uint64_t t2_lo = t2 & 0xfffffffful;

    // operand 1
    constexpr uint64_t r_hi = 0ul;
    constexpr uint64_t r_lo = static_cast<uint64_t>(R);

    const uint64_t hi = t2_hi * r_hi;                 // high bits
    const uint64_t mid = t2_hi * r_lo + t2_lo * r_hi; // mid bits
    const uint64_t lo = t2_lo * r_lo;                 // low bits

    const uint64_t mid_hi = mid >> 32;          // high 32 -bits of mid
    const uint64_t mid_lo = mid & 0xfffffffful; // low 32 -bits of mid

    const uint64_t t3 = lo >> 32;
    const uint64_t t4 = t3 + mid_lo;
    const uint64_t carry = t4 >> 32;

    const uint64_t res_hi = hi + mid_hi + carry;
    const uint64_t res_lo = lo + (mid_lo << 32);

    // It must be the case that,
    //
    // if   t3 = t2 * R
    // then ((res_hi << 64) | res_lo) == t3
    //
    // Though result is 128 -bit, with two limbs ( such as res_hi and res_lo ),
    // representing lower & higher 64 -bits, only lower (23 + 23 + 24) -bits are
    // of significance.
    //
    // t0 -> 23 -bit number
    // t1 -> 23 -bit number
    // t2 -> (23 + 23) -bit number      | t2 = t0 * t1
    // R -> 24 -bit number
    // t3 -> (23 + 23 + 24) -bit number | t3 = t2 * R
    //
    // Now we can drop lower 46 -bits of 128 -bit result ( remember, which has
    // only 70 significant bits ) & keep 24 -bits of interest, in res, see
    // below.

    const uint64_t res = ((res_hi & 0x3ful) << 18) | (res_lo >> 46);
    const uint64_t t5 = res * static_cast<uint64_t>(Q);
    const uint32_t t6 = static_cast<uint32_t>(t2 - t5);

    const uint32_t t7 = reduce_once(t6);
    return zq_t(t7);
  }

  // Compound modulo multiplication of two Zq elements.
  inline constexpr void operator*=(const zq_t rhs) { *this = *this * rhs; }

  // Raises field element to N -th power, using exponentiation by repeated
  // squaring rule.
  //
  // Taken from
  // https://github.com/itzmeanjan/kyber/blob/3cd41a5/include/ff.hpp#L224-L246
  inline constexpr zq_t operator^(const size_t n) const
  {
    zq_t base = *this;

    const zq_t br[]{ zq_t(1), base };
    zq_t res = br[n & 0b1ul];

    const size_t zeros = std::countl_zero(n);
    const size_t till = 64ul - zeros;

    for (size_t i = 1; i < till; i++) {
      base = base * base;

      const zq_t br[]{ zq_t(1), base };
      res = res * br[(n >> i) & 0b1ul];
    }

    return res;
  }

  // Computes multiplicative inverse of Zq element s.t. a * a.inv() = 1 (mod Q).
  //
  // Note, if Zq element is 0, we can't compute multiplicative inverse and 0 is
  // returned.
  inline constexpr zq_t inv() const { return *this ^ static_cast<size_t>(Q - 2); }

  // Modulo division of two Zq elements.
  //
  // Note, if denominator is 0, returned result is 0 too, becaue we can't
  // compute multiplicative inverse of 0.
  inline constexpr zq_t operator/(const zq_t rhs) const { return *this * rhs.inv(); }

  // Equality check between two field elements ∈ Z_q | q = 2^23 - 2^13 + 1
  inline constexpr bool operator==(const zq_t rhs) const { return !static_cast<bool>(this->v ^ rhs.v); }

  // Non-equality check between two field elements ∈ Z_q | q = 2^23 - 2^13 + 1
  inline constexpr bool operator!=(const zq_t rhs) const { return !(*this == rhs); }

  // Greater than operator applied to elements ∈ Z_q | q = 2^23 - 2^13 + 1
  inline constexpr bool operator>(const zq_t rhs) const { return this->v > rhs.v; }

  // Greater than equal operator applied to elements ∈ Z_q | q = 2^23 - 2^13 + 1
  inline constexpr bool operator>=(const zq_t rhs) const { return this->v >= rhs.v; }

  // Lesser than operator applied to elements ∈ Z_q | q = 2^23 - 2^13 + 1
  inline constexpr bool operator<(const zq_t rhs) const { return this->v < rhs.v; }

  // Lesser than equal operator applied to elements ∈ Z_q | q = 2^23 - 2^13 + 1
  inline constexpr bool operator<=(const zq_t rhs) const { return this->v <= rhs.v; }

  // Shifts operand ∈ Z_q, leftwards by l bit positions | q = 2^23 - 2^13 + 1
  inline constexpr zq_t operator<<(const size_t l) const { return zq_t(this->v << l); }

  // Generate a random field element ∈ Z_q | q = 2^23 - 2^13 + 1
  static inline zq_t random(prng::prng_t& prng)
  {
    uint32_t res = 0;
    prng.read(std::span(reinterpret_cast<uint8_t*>(&res), sizeof(res)));

    // Modulo reduce random sampled 32 -bit unsigned integer value, because
    // explicit constructor of Zq expects its input ∈ [0, Q).
    return zq_t(barrett_reduce(res));
  }

private:
  // Underlying value held in this type.
  //
  // Note, v is always kept in its canonical form i.e. v ∈ [0, Q).
  uint32_t v = 0u;

  // Given a 32 -bit unsigned integer value v, this routine can be used for
  // reducing it by modulo prime Q = 2^23 - 2^13 + 1, computing v' ∈ [0, Q),
  // without using division/ modulo division operator.
  //
  // ∀ v ∈ [0, 2^32), barrett_reduce(v) == (v % Q) - must hold !
  static inline constexpr uint32_t barrett_reduce(const uint32_t val)
  {
    constexpr uint32_t mask23 = (1u << 23) - 1u;
    constexpr uint32_t mask13 = (1u << 13) - 1u;
    constexpr uint32_t u23_max = mask23;

    const uint32_t hi = val >> 23;
    const uint32_t lo = val & mask23;

    const uint32_t t0 = (hi << 13) - hi;
    const uint32_t t1 = t0 + lo;
    const bool flg0 = t0 > (u23_max - lo);
    const uint32_t t2 = (-static_cast<uint32_t>(flg0)) & mask13;
    const uint32_t t3 = t1 + t2;
    const uint32_t t4 = t3 & mask23;

    const uint32_t t5 = reduce_once(t4);
    return t5;
  }

  // Given a 32 -bit unsigned integer `v` such that `v` ∈ [0, 2*Q), this routine can be invoked for reducing `v` modulo
  // prime Q.
  static inline constexpr uint32_t reduce_once(const uint32_t val)
  {
    const uint32_t t0 = val - Q;
    const uint32_t t1 = -(t0 >> 31);
    const uint32_t t2 = t1 & Q;
    const uint32_t t3 = t0 + t2;

    return t3;
  }
};

}
