#ifndef LOOKUP_H
#define LOOKUP_H

#include <array>
#include <cstdint>

#include "constants.hpp"
#include "global.hpp"

/*

INCUDES DEFINTIONS FOR THE FOLLOWING LOOKUP TABLES

constexpr auto pawn_attacks = make_pawn_attacks(pawn_attacks_for_square);
constexpr auto pawn_moves = make_pawn_moves(pawn_moves_for_square);
constexpr auto knight_moves = make_knight_moves(knight_moves_for_square);
constexpr auto king_moves = make_king_moves(king_moves_for_square);
constexpr auto bishop_occupancy = make_bishop_occupancy(bishop_occupancy_for_square);
constexpr auto rook_occupancy = make_rook_occupancy(rook_occupancy_for_square);
constexpr auto queen_occupancy = make_queen_occupancy(queen_occupancy_for_square);

*/

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
    return ((bitboard & NOT_FILE_H & NOT_RANK_18) >> 7) | ((bitboard & NOT_FILE_A & NOT_RANK_18) >> 9);
  }
  else
  {
    return ((bitboard & NOT_FILE_A & NOT_RANK_18) << 7) | ((bitboard & NOT_FILE_H & NOT_RANK_18) << 9);
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
    return ((bitboard & NOT_RANK_18 & NOT_RANK_7) >> 8) | ((bitboard & RANK_2) >> 16);
  }
  else
  {
    return ((bitboard & NOT_RANK_18 & NOT_RANK_2) << 8) | ((bitboard & RANK_7) << 16);
  }
}

constexpr auto pawn_moves = make_pawn_moves(pawn_moves_for_square);

template <typename Generator>
constexpr auto make_knight_moves(Generator&& f)
{
  std::array<uint64_t, 64> knight_move_table{};
  uint64_t bitboard = 1;

  for (int i = 0; i < 64; i++)
  {
    knight_move_table[i] = f(bitboard);
    bitboard <<= 1;
  }

  return knight_move_table;
}

constexpr auto knight_moves_for_square(uint64_t bitboard)
{
  // starting from the knight position, right and up then ctrclkwise about the knight square
  return ((bitboard & NOT_FILE_GH & NOT_RANK_8) >> 6) | ((bitboard & NOT_RANK_78 & NOT_FILE_H) >> 15) |
         ((bitboard & NOT_RANK_78 & NOT_FILE_A) >> 17) | ((bitboard & NOT_FILE_AB & NOT_RANK_8) >> 10) |
         ((bitboard & NOT_FILE_AB & NOT_RANK_1) << 6) | ((bitboard & NOT_RANK_12 & NOT_FILE_A) << 15) |
         ((bitboard & NOT_RANK_12 & NOT_FILE_H) << 17) | ((bitboard & NOT_FILE_GH & NOT_RANK_1) << 10);
}

constexpr auto knight_moves = make_knight_moves(knight_moves_for_square);

template <typename Generator>
constexpr auto make_king_moves(Generator&& f)
{
  std::array<uint64_t, 64> king_move_table{};
  uint64_t bitboard = 1;

  for (int i = 0; i < 64; i++)
  {
    king_move_table[i] = f(bitboard);
    bitboard <<= 1;
  }

  return king_move_table;
}

constexpr auto king_moves_for_square(uint64_t bitboard)
{
  // starting from kings square, the square to the right then ctrclkwise about the king
  return ((bitboard & NOT_FILE_H) << 1) | ((bitboard & NOT_FILE_H & NOT_RANK_8) >> 7) | ((bitboard & NOT_RANK_8) >> 8) |
         ((bitboard & NOT_FILE_A & NOT_RANK_8) >> 9) | ((bitboard & NOT_FILE_A) >> 1) |
         ((bitboard & NOT_FILE_A & NOT_RANK_1) << 7) | ((bitboard & NOT_RANK_1) << 8) |
         ((bitboard & NOT_FILE_H & NOT_RANK_1) << 9);
}

constexpr auto king_moves = make_king_moves(king_moves_for_square);

template <typename Generator>
constexpr auto make_bishop_occupancy(Generator&& f)
{
  std::array<uint64_t, 64> bishop_occupancy{};
  uint64_t bitboard = 1;

  for (int i = 0; i < 64; i++)
  {
    bishop_occupancy[i] = f(bitboard);
    bitboard <<= 1;
  }

  return bishop_occupancy;
}

constexpr auto bishop_occupancy_for_square(uint64_t bitboard)
{
  uint64_t bishop_occupancy_bitboard = 0;
  // note the 30 entries in the diagonal constant generation (constants.hpp)
  for (int i = 0; i < 30; i++)
  {
    // each 1 bit bitboard only intersects 2 unique diagonals
    if (bitboard & diagonal_masks[i])
    {
      bishop_occupancy_bitboard |= ~bitboard & diagonal_masks[i] & NOT_RANK_18_FILE_AH;
    }
  }

  return bishop_occupancy_bitboard;
}

constexpr auto bishop_occupancy = make_bishop_occupancy(bishop_occupancy_for_square);

template <typename Generator>
constexpr auto make_rook_occupancy(Generator&& f)
{
  std::array<uint64_t, 64> rook_occupancy{};
  uint64_t bitboard = 1;

  for (int i = 0; i < 64; i++)
  {
    rook_occupancy[i] = f(bitboard);
    bitboard <<= 1;
  }

  return rook_occupancy;
}

constexpr auto rook_occupancy_for_square(uint64_t bitboard)
{
  uint64_t rook_occupancy_bitboard = 0;
  // note the 30 entries in the diagonal constant generation (constants.hpp)
  for (int i = 0; i < 16; i++)
  {
    // each 1 bit bitboard only intersects 2 unique diagonals
    if (bitboard & rank_and_file_masks[i])
    {
      rook_occupancy_bitboard |= ~bitboard & rank_and_file_masks[i] & NOT_RANK_18_FILE_AH;
    }
  }

  return rook_occupancy_bitboard;
}

constexpr auto rook_occupancy = make_rook_occupancy(rook_occupancy_for_square);

constexpr auto make_queen_occupancy()
{
  std::array<uint64_t, 64> queen_occupancy{};

  for (int i = 0; i < 64; i++)
  {
    queen_occupancy[i] = bishop_occupancy[i] | rook_occupancy[i];
  }

  return queen_occupancy;
}

constexpr auto queen_occupancy = make_queen_occupancy();

#endif
