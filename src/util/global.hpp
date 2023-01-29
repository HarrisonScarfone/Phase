#ifndef GLOBAL_H
#define GLOBAL_H

#include <cstdint>
#include <string>

// clang-format off
constexpr int index64[64] = {
  0,  1,  48, 2,  57, 49, 28, 3,  61, 58, 50, 42, 38, 29, 17, 4,  62, 55, 59, 36, 53, 51,
  43, 22, 45, 39, 33, 30, 24, 18, 12, 5,  63, 47, 56, 27, 60, 41, 37, 16, 54, 35, 52, 21,
  44, 32, 23, 11, 46, 26, 40, 15, 34, 20, 31, 10, 25, 14, 19, 9,  13, 8,  7,  6};
// clang-format on

// A forward bitscan (get index of lsb)
constexpr int bitscan(uint64_t bitboard)
{
  const uint64_t debruijn64 = static_cast<uint64_t>(0x03f79d71b4cb0a89);
  return index64[((bitboard & -bitboard) * debruijn64) >> 58];
}

constexpr int bitcount(uint64_t bitboard)
{
  int count = 0;
  while (bitboard)
  {
    count++;
    bitboard &= bitboard - 1;
  }
  return count;
}

// clang-format off
enum Square 
{
  a8, b8, c8, d8, e8, f8, g8, h8,
  a7, b7, c7, d7, e7, f7, g7, h7,
  a6, b6, c6, d6, e6, f6, g6, h6,
  a5, b5, c5, d5, e5, f5, g5, h5,
  a4, b4, c4, d4, e4, f4, g4, h4,
  a3, b3, c3, d3, e3, f3, g3, h3,
  a2, b2, c2, d2, e2, f2, g2, h2,
  a1, b1, c1, d1, e1, f1, g1, h1
};
// clang-format on

constexpr int file(Square sq) { return sq % 8; }
constexpr int rank(Square sq) { return sq / 8; }
constexpr uint64_t square_to_bitboard(Square sq) { return 1ull << sq; }
constexpr Square bitboard_to_square(uint64_t bitboard)
{
  return static_cast<Square>(__builtin_ctzll(bitboard & -bitboard));
}

struct Position
{
  uint64_t black = 0;
  uint64_t white = 0;
  uint64_t kings = 0;
  uint64_t queens = 0;
  uint64_t rooks = 0;
  uint64_t bishops = 0;
  uint64_t knights = 0;
  uint64_t pawns = 0;

  bool white_to_move = false;

  bool white_can_castle_kingside = false;
  bool white_can_castle_queenside = false;
  bool black_can_castle_kingside = false;
  bool black_can_castle_queenside = false;

  uint64_t enPassantTarget = 0;

  short int half_move_clock = 0;
  short int full_move_clock = 0;
};

std::string square_to_string(Square sq);
Square string_to_square(const std::string &str);
std::string bitboard_to_string(uint64_t bitboard);

struct Move
{
  uint64_t move;
};

#endif