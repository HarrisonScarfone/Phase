#ifndef MAGICBITBOARDS_H
#define MAGICBITBOARDS_H

#include <cstdint>

#include "tables.hpp"

struct MagicBB
{
  uint64_t mask;
  uint64_t magic;
};

namespace RandGen
{
extern uint32_t RANDOM_STATE;

constexpr uint32_t generate_random_32_bit_number()
{
  uint32_t random_number = RANDOM_STATE;

  random_number ^= random_number << 13;
  random_number ^= random_number >> 17;
  random_number ^= random_number << 5;

  RANDOM_STATE = random_number;
  return random_number;
}

constexpr uint64_t generate_random_64_bit_number()
{
  uint64_t n1 = static_cast<uint64_t>(generate_random_32_bit_number() & 0xFFFF);
  uint64_t n2 = static_cast<uint64_t>(generate_random_32_bit_number() & 0xFFFF);
  uint64_t n3 = static_cast<uint64_t>(generate_random_32_bit_number() & 0xFFFF);
  uint64_t n4 = static_cast<uint64_t>(generate_random_32_bit_number() & 0xFFFF);

  return n1 | (n2 << 16) | (n3 << 32) | (n4 << 48);
}

constexpr uint64_t generate_magic_number_candidate()
{
  return generate_random_64_bit_number() & generate_random_64_bit_number() & generate_random_64_bit_number();
}

constexpr uint64_t generate_magic_number(int square, int relevant_bits, uint64_t occupancy_mask)
{
  // uint64_t occupan
}

}  // namespace RandGen

#endif
