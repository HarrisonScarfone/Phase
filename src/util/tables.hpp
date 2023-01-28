#ifndef LOOKUP_H
#define LOOKUP_H

#include <array>
#include <cstdint>

#include "constants.hpp"
#include "global.hpp"

/*

INCUDES DEFINTIONS FOR THE FOLLOWING LOOKUP TABLES

constexpr auto pawn_attacks = make_pawn_attacks(pawn_attacks_for_bitboard);
constexpr auto pawn_moves = make_pawn_moves(pawn_moves_for_bitboard);
constexpr auto knight_moves = make_knight_moves(knight_moves_for_bitboard);
constexpr auto king_moves = make_king_moves(king_moves_for_bitboard);
constexpr auto bishop_occupancy = make_bishop_occupancy(bishop_full_occupancy_for_bitboard);
constexpr auto rook_occupancy = make_rook_occupancy(rook_full_occupancy_for_bitboard);

*/

template <typename Generator>
constexpr auto pawn_table_generator(Generator&& f)
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

constexpr auto pawn_attacks_for_bitboard(bool white, uint64_t bitboard)
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

constexpr auto pawn_moves_for_bitboard(bool white, uint64_t bitboard)
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

constexpr auto pawn_attacks = pawn_table_generator(pawn_attacks_for_bitboard);
constexpr auto pawn_moves = pawn_table_generator(pawn_moves_for_bitboard);

template <typename Generator>
constexpr auto piece_table_generator(Generator&& f)
{
  std::array<uint64_t, 64> table{};
  uint64_t bitboard = 1;

  for (int i = 0; i < 64; i++)
  {
    table[i] = f(bitboard);
    bitboard <<= 1;
  }

  return table;
}

constexpr auto knight_moves_for_bitboard(uint64_t bitboard)
{
  // starting from the knight position, right and up then ctrclkwise about the knight square
  return ((bitboard & NOT_FILE_GH & NOT_RANK_8) >> 6) | ((bitboard & NOT_RANK_78 & NOT_FILE_H) >> 15) |
         ((bitboard & NOT_RANK_78 & NOT_FILE_A) >> 17) | ((bitboard & NOT_FILE_AB & NOT_RANK_8) >> 10) |
         ((bitboard & NOT_FILE_AB & NOT_RANK_1) << 6) | ((bitboard & NOT_RANK_12 & NOT_FILE_A) << 15) |
         ((bitboard & NOT_RANK_12 & NOT_FILE_H) << 17) | ((bitboard & NOT_FILE_GH & NOT_RANK_1) << 10);
}

constexpr auto king_moves_for_bitboard(uint64_t bitboard)
{
  // starting from kings square, the square to the right then ctrclkwise about the king
  return ((bitboard & NOT_FILE_H) << 1) | ((bitboard & NOT_FILE_H & NOT_RANK_8) >> 7) | ((bitboard & NOT_RANK_8) >> 8) |
         ((bitboard & NOT_FILE_A & NOT_RANK_8) >> 9) | ((bitboard & NOT_FILE_A) >> 1) |
         ((bitboard & NOT_FILE_A & NOT_RANK_1) << 7) | ((bitboard & NOT_RANK_1) << 8) |
         ((bitboard & NOT_FILE_H & NOT_RANK_1) << 9);
}

constexpr auto bishop_full_occupancy_for_bitboard(uint64_t bitboard)
{
  uint64_t bishop_occupancy_bitboard = 0;
  for (int i = 0; i < 30; i++)
  {
    if (bitboard & diagonal_masks[i])
    {
      bishop_occupancy_bitboard |= ~bitboard & diagonal_masks[i] & NOT_RANK_18_FILE_AH;
    }
  }

  return bishop_occupancy_bitboard;
}

constexpr auto rook_full_occupancy_for_bitboard(uint64_t bitboard)
{
  uint64_t rook_occupancy_bitboard = 0;
  uint64_t edge_mask = 0;

  if (bitboard & NOT_FILE_A)
  {
    edge_mask |= FILE_A;
  }
  if (bitboard & NOT_FILE_H)
  {
    edge_mask |= FILE_H;
  }
  if (bitboard & NOT_RANK_1)
  {
    edge_mask |= RANK_1;
  }
  if (bitboard & NOT_RANK_8)
  {
    edge_mask |= RANK_8;
  }

  for (int i = 0; i < 16; i++)
  {
    if (bitboard & rank_and_file_masks[i])
    {
      rook_occupancy_bitboard |= ~bitboard & rank_and_file_masks[i] & ~edge_mask;
    }
  }

  return rook_occupancy_bitboard;
}

constexpr auto knight_moves = piece_table_generator(knight_moves_for_bitboard);
constexpr auto king_moves = piece_table_generator(king_moves_for_bitboard);
constexpr auto bishop_full_occupancy = piece_table_generator(bishop_full_occupancy_for_bitboard);
constexpr auto rook_full_occupancy = piece_table_generator(rook_full_occupancy_for_bitboard);

constexpr auto bishop_attack_mask_for_bitboard(uint64_t bitboard, uint64_t blocker_bitboard)
{
  uint64_t attack_mask = 0;
  uint64_t temp_bitboard = 0;

  // down the right
  for (int i = 1; i < 8; i++)
  {
    temp_bitboard = bitboard << (9 * i);
    attack_mask |= temp_bitboard;
    if (temp_bitboard & blocker_bitboard)
    {
      break;
    }
  }

  // down the left
  for (int i = 1; i < 8; i++)
  {
    temp_bitboard = bitboard << (7 * i);
    attack_mask |= temp_bitboard;
    if (temp_bitboard & blocker_bitboard)
    {
      break;
    }
  }

  // up to the right
  for (int i = 1; i < 8; i++)
  {
    temp_bitboard = bitboard >> (7 * i);
    attack_mask |= temp_bitboard;
    if (temp_bitboard & blocker_bitboard)
    {
      break;
    }
  }

  // down the right
  for (int i = 1; i < 8; i++)
  {
    temp_bitboard = bitboard >> (9 * i);
    attack_mask |= temp_bitboard;
    if (temp_bitboard & blocker_bitboard)
    {
      break;
    }
  }

  return attack_mask;
}

constexpr auto rook_attack_mask_for_bitboard(uint64_t bitboard, uint64_t blocker_bitboard)
{
  uint64_t attack_mask = 0;
  uint64_t temp_bitboard = 0;

  int file = bitscan(bitboard) % 8;

  // to the right
  for (int i = 1; i < 8 - file; i++)
  {
    temp_bitboard = bitboard << i;
    attack_mask |= temp_bitboard;
    if (temp_bitboard & blocker_bitboard)
    {
      break;
    }
  }

  // to the left
  for (int i = 1; i < 1 + file; i++)
  {
    temp_bitboard = bitboard >> i;
    attack_mask |= temp_bitboard;
    if (temp_bitboard & blocker_bitboard)
    {
      break;
    }
  }

  // up
  for (int i = 1; i < 8; i++)
  {
    temp_bitboard = bitboard >> (8 * i);
    attack_mask |= temp_bitboard;
    if (temp_bitboard & blocker_bitboard)
    {
      break;
    }
  }

  // down
  for (int i = 1; i < 8; i++)
  {
    temp_bitboard = bitboard << (8 * i);
    attack_mask |= temp_bitboard;
    if (temp_bitboard & blocker_bitboard)
    {
      break;
    }
  }

  return attack_mask;
}

constexpr uint64_t construct_occupancies(int occupancy_index, int occupancy_map_bits, uint64_t occupancy_map)
{
  uint64_t bitboard = 0;

  for (int i = 0; i < occupancy_map_bits; i++)
  {
    int square = bitscan(occupancy_map);
    occupancy_map ^= 1ull << square;

    if (occupancy_index & (1 << i))
    {
      bitboard |= (1ull << square);
    }
  }

  return bitboard;
}

template <typename Generator>
constexpr auto occupancy_bits(Generator&& f)
{
  std::array<int, 64> occupancy_bits_for_square{};

  for (int i = 0; i < 64; i++)
  {
    occupancy_bits_for_square[i] = f(i);
  }

  return occupancy_bits_for_square;
}

constexpr auto bishop_occupany_bits_for_square(int square) { return bitcount(bishop_full_occupancy[square]); }

constexpr auto rook_occupany_bits_for_square(int square) { return bitcount(rook_full_occupancy[square]); }

constexpr auto bishop_occupancy = occupancy_bits(bishop_occupany_bits_for_square);
constexpr auto rook_occupancy = occupancy_bits(rook_occupany_bits_for_square);

#endif
