#include "global.hpp"

#include "util.hpp"

std::string square_to_string(Square sq)
{
  int file = sq % 8;
  int rank = sq / 8;
  return (char)('a' + file) + std::to_string(rank + 1);
}

Square string_to_square(const std::string &str)
{
  int file = str[0] - 'a';
  int rank = str[1] - '1';
  return static_cast<Square>(rank * 8 + file);
}

std::string bitboard_to_string(uint64_t bitboard)
{
  std::string str;
  for (int i = 0; i < 64; i++)
  {
    if ((bitboard >> i) & 1)
    {
      str += square_to_string(static_cast<Square>(i));
      str += " ";
    }
  }
  return str;
}

uint64_t set_bit_high(uint64_t bitboard, int square) { return bitboard | (static_cast<uint64_t>(1) << square); }

uint64_t set_bit_low(uint64_t bitboard, int square)
{
  return bitboard & ~(set_bit_high(static_cast<uint64_t>(0), square));
}
