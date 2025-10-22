#pragma once
#include "blake3.h"
#include <cstring>
#include <span>

namespace ml_dsa_hashing {

// BLAKE3 Hashing Wrapper
//
// C++ class wrapper on top of API definition @ https://github.com/BLAKE3-team/BLAKE3/blob/e0b1d91410fd0a344beda6ee0e6f1972ad04be08/c/README.md#api
struct blake3_hasher_t
{
private:
  blake3_hasher internal_hasher;
  bool finalized;
  size_t squeeze_from;

public:
  blake3_hasher_t()
  {
    blake3_hasher_init(&this->internal_hasher);
    this->finalized = false;
    this->squeeze_from = 0;
  };

  void absorb(std::span<const uint8_t> msg)
  {
    if (!this->finalized) {
      blake3_hasher_update(&this->internal_hasher, msg.data(), msg.size());
    }
  }

  void finalize()
  {
    if (!this->finalized) {
      blake3_hasher_finalize(&this->internal_hasher, nullptr, 0);
      this->finalized = true;
    }
  }

  void squeeze(std::span<uint8_t> dig)
  {
    if (this->finalized) {
      blake3_hasher_finalize_seek(&this->internal_hasher, this->squeeze_from, dig.data(), dig.size());
      this->squeeze_from += dig.size();
    }
  }

  void reset()
  {
    blake3_hasher_reset(&this->internal_hasher);
    this->finalized = false;
    this->squeeze_from = 0;
  }
};

}
