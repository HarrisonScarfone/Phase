#ifndef LOOKUP_H
#define LOOKUP_H

#include <array>
#include <cstdint>

#include "constants.hpp"
#include "global.hpp"

template <typename Generator>
constexpr auto make_pawn_attacks(Generator&& f)
{
  std::array<std::array<uint64_t, 64>, 2> pawn_attack_table{};
  uint64_t bitboard = 1;

  for (int i = 0; i < 64; i++)
  {
    pawn_attack_table[1][i] = f(1, bitboard);
    pawn_attack_table[0][i] = f(0, bitboard);
    bitboard <<= 1;
  }

  return pawn_attack_table;
}

constexpr auto pawn_attacks_for_square(bool white, uint64_t bitboard)
{
  if (white)
  {
    return ((bitboard & NOT_FILE_H & NOT_RANK_1_8) >> 7) | ((bitboard & NOT_FILE_A & NOT_RANK_1_8) >> 9);
  }
  else
  {
    return ((bitboard & NOT_FILE_A & NOT_RANK_1_8) << 7) | ((bitboard & NOT_FILE_H & NOT_RANK_1_8) << 9);
  }
}

constexpr auto pawn_attacks = make_pawn_attacks(pawn_attacks_for_square);

template <typename Generator>
constexpr auto make_pawn_moves(Generator&& f)
{
  std::array<std::array<uint64_t, 64>, 2> pawn_move_table{};
  uint64_t bitboard = 1;

  for (int i = 0; i < 64; i++)
  {
    pawn_move_table[1][i] = f(1, bitboard);
    pawn_move_table[0][i] = f(0, bitboard);
    bitboard <<= 1;
  }

  return pawn_move_table;
}

constexpr auto pawn_moves_for_square(bool white, uint64_t bitboard)
{
  if (white)
  {
    return ((bitboard && NOT_RANK_1_8) >> 8) | ((bitboard & RANK_2) >> 16);
  }
  else
  {
    return ((bitboard & NOT_RANK_1_8) << 8) | ((bitboard & RANK_7) << 16);
  }
}

constexpr auto pawn_moves = make_pawn_moves(pawn_moves_for_square);

#endif
