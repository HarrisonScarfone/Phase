#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <array>
#include <cstdint>

// Rank constants
constexpr uint64_t RANK_1 = 0xff00000000000000;
constexpr uint64_t RANK_2 = 0x00ff000000000000;
constexpr uint64_t RANK_3 = 0x0000ff0000000000;
constexpr uint64_t RANK_4 = 0x000000ff00000000;
constexpr uint64_t RANK_5 = 0x00000000ff000000;
constexpr uint64_t RANK_6 = 0x0000000000ff0000;
constexpr uint64_t RANK_7 = 0x000000000000ff00;
constexpr uint64_t RANK_8 = 0x00000000000000ff;
constexpr uint64_t NOT_RANK_1 = ~RANK_1;
constexpr uint64_t NOT_RANK_8 = ~RANK_8;
constexpr uint64_t NOT_RANK_18 = ~(RANK_1 | RANK_8);
constexpr uint64_t NOT_RANK_7 = ~RANK_7;
constexpr uint64_t NOT_RANK_2 = ~RANK_2;
constexpr uint64_t NOT_RANK_78 = NOT_RANK_7 & NOT_RANK_8;
constexpr uint64_t NOT_RANK_12 = NOT_RANK_1 & NOT_RANK_2;

// File constants
constexpr uint64_t FILE_A = 0x0101010101010101ull;
constexpr uint64_t FILE_B = FILE_A << 1;
constexpr uint64_t FILE_C = FILE_A << 2;
constexpr uint64_t FILE_D = FILE_A << 3;
constexpr uint64_t FILE_E = FILE_A << 4;
constexpr uint64_t FILE_F = FILE_A << 5;
constexpr uint64_t FILE_G = FILE_A << 6;
constexpr uint64_t FILE_H = FILE_A << 7;
constexpr uint64_t NOT_FILE_A = ~FILE_A;
constexpr uint64_t NOT_FILE_B = ~FILE_B;
constexpr uint64_t NOT_FILE_G = ~FILE_G;
constexpr uint64_t NOT_FILE_H = ~FILE_H;
constexpr uint64_t FILE_AB = FILE_A | FILE_B;
constexpr uint64_t NOT_FILE_AB = ~FILE_AB;
constexpr uint64_t FILE_GH = FILE_G | FILE_H;
constexpr uint64_t NOT_FILE_GH = ~FILE_GH;
constexpr uint64_t NOT_FILE_AH = NOT_FILE_A & NOT_FILE_H;

// Rank and file hybrid
constexpr uint64_t NOT_RANK_18_FILE_AH = NOT_RANK_18 & NOT_FILE_AH;

// Rank and file constnats as Array for slider move generation (need to be iterable)
// Create 16 masks, top to bottom, left to right;
constexpr std::array<uint64_t, 16> make_rank_and_file_masks()
{
  std::array<uint64_t, 16> rank_and_file_masks{};
  uint64_t rank = RANK_8;
  uint64_t file = FILE_A;

  for (int i = 0; i < 8; i++)
  {
    rank_and_file_masks[i] = rank;
    rank_and_file_masks[i + 8] = file;
    rank <<= 8;
    file <<= 1;
  }

  return rank_and_file_masks;
}

constexpr auto rank_and_file_masks = make_rank_and_file_masks();

// Diagonal Constants as Array for slider move generation (need to be iterable)
// Create 30 masks, top left to bottom right, top right to bottom left
constexpr std::array<uint64_t, 30> make_diagonal_masks()
{
  std::array<uint64_t, 30> diagonal_masks{};
  int array_index = 0;

  for (int i = 0; i < 8; i++)
  {
    uint64_t bitboard = 1;
    bitboard <<= i;
    uint64_t temp_bitboard = 1;
    temp_bitboard <<= i;
    for (int j = 0; j < i; j++)
    {
      temp_bitboard <<= 7;
      bitboard |= temp_bitboard;
    }
    diagonal_masks[array_index] = bitboard;
    array_index++;
  }

  for (int i = 6; i > -1; i--)
  {
    uint64_t bitboard = 1ULL << 63;
    bitboard >>= i;
    uint64_t temp_bitboard = 1ULL << 63;
    temp_bitboard >>= i;
    for (int j = 0; j < i; j++)
    {
      temp_bitboard >>= 7;
      bitboard |= temp_bitboard;
    }
    diagonal_masks[array_index] = bitboard;
    array_index++;
  }

  for (int i = 0; i < 8; i++)
  {
    uint64_t bitboard = 1ULL << 7;
    bitboard >>= i;
    uint64_t temp_bitboard = 1ULL << 7;
    temp_bitboard >>= i;
    for (int j = 0; j < i; j++)
    {
      temp_bitboard <<= 9;
      bitboard |= temp_bitboard;
    }
    diagonal_masks[array_index] = bitboard;
    array_index++;
  }

  for (int i = 6; i > -1; i--)
  {
    uint64_t bitboard = 1ULL << 56;
    bitboard <<= i;
    uint64_t temp_bitboard = 1ULL << 56;
    temp_bitboard <<= i;
    for (int j = 0; j < i; j++)
    {
      temp_bitboard >>= 9;
      bitboard |= temp_bitboard;
    }
    diagonal_masks[array_index] = bitboard;
    array_index++;
  }

  return diagonal_masks;
}

constexpr auto diagonal_masks = make_diagonal_masks();

#endif
