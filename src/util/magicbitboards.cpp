#include "magicbitboards.hpp"

uint64_t bishop_attacks(uint64_t occupancy, int square)
{
  occupancy &= magic_bishop_table[square].mask;
  occupancy *= magic_bishop_table[square].magic;
  occupancy >>= 64 - bishop_occupancy[square];
  return magic_bishop_attacks[square][occupancy];
}

uint64_t rook_attacks(uint64_t occupancy, int square)
{
  occupancy &= magic_rook_table[square].mask;
  occupancy *= magic_rook_table[square].magic;
  occupancy >>= 64 - rook_occupancy[square];
  return magic_rook_attacks[square][occupancy];
}

uint64_t queen_attacks(uint64_t occupancy, int square)
{
  return bishop_attacks(occupancy, square) | rook_attacks(occupancy, square);
}
