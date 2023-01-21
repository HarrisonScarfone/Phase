#include "global.hpp"

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