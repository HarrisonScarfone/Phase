#include "global.hpp"

#include "util.hpp"

std::string square_to_string(Square sq)
{
  int file = sq % 8;
  int rank = sq / 8;
  return (char)('a' + file) + std::to_string(rank + 1);
}

int string_to_int(std::string str)
{
  int file = str[0] - 'a';
  int rank = str[1] - '1' + 1;
  return (8 - rank) * 8 + file;
}

std::string bitboard_to_string(uint64_t bitboard)
{
  std::string str;
  int count = -1;
  while (bitboard)
  {
    bitboard >>= 1;
    count++;
  }

  return static_cast<std::string>(square_names[count]);
}

uint64_t set_bit_high(uint64_t bitboard, int square) { return bitboard | (static_cast<uint64_t>(1) << square); }

uint64_t set_bit_low(uint64_t bitboard, int square)
{
  return bitboard & ~(set_bit_high(static_cast<uint64_t>(0), square));
}
