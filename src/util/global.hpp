#ifndef GLOBAL_H
#define GLOBAL_H

#include <array>
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

uint64_t set_bit_high(uint64_t bitboard, int square);
uint64_t set_bit_low(uint64_t bitboard, int square);

constexpr int file_of(int square) { return square % 8; }

constexpr int rank_of(int square) { return square / 8; }

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

constexpr std::array<std::string_view, 64> square_names =
{
  "a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8",
  "a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7",
  "a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6",
  "a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5",
  "a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4",
  "a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3",
  "a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2",
  "a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1"
};
// clang-format on

constexpr std::string_view int_to_square_string(int square) { return square_names[square]; }
constexpr int file(Square sq) { return sq % 8; }
constexpr int rank(Square sq) { return sq / 8; }
constexpr uint64_t square_to_bitboard(Square sq) { return 1ull << sq; }
constexpr uint64_t int_location_to_bitboard(int sq) { return 1ull << sq; }
constexpr Square bitboard_to_square(uint64_t bitboard) { return static_cast<Square>(bitscan(bitboard)); }

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

inline uint64_t white_kings(Position* position) { return position->white & position->kings; }
inline uint64_t white_queens(Position* position) { return position->white & position->queens; }
inline uint64_t white_rooks(Position* position) { return position->white & position->rooks; }
inline uint64_t white_bishops(Position* position) { return position->white & position->bishops; }
inline uint64_t white_knights(Position* position) { return position->white & position->knights; }
inline uint64_t white_pawns(Position* position) { return position->white & position->pawns; }
inline uint64_t black_kings(Position* position) { return position->black & position->kings; }
inline uint64_t black_queens(Position* position) { return position->black & position->queens; }
inline uint64_t black_rooks(Position* position) { return position->black & position->rooks; }
inline uint64_t black_bishops(Position* position) { return position->black & position->bishops; }
inline uint64_t black_knights(Position* position) { return position->black & position->knights; }
inline uint64_t black_pawns(Position* position) { return position->black & position->pawns; }
inline uint64_t white_occupied_no_king(Position* position) { return position->white & ~position->kings; }
inline uint64_t black_occupied_no_king(Position* position) { return position->black & ~position->kings; }
inline uint64_t all_occupied(Position* position) { return position->black | position->white; }

std::string square_to_string(Square sq);
int string_to_int(std::string str);
std::string bitboard_to_string(uint64_t bitboard);
std::string int_to_string(int sq);
std::string uint_move_to_engine_string_move(uint32_t move);

#endif