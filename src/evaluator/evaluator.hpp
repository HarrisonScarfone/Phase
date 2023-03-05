#ifndef EVALUATOR_H
#define EVALUATOR_H

#include <array>

#include "../util/global.hpp"

// clang-format off
constexpr std::array<int, 64> white_pawn_pst = {
    0,  0,  0,  0,  0,  0,  0,  0,
    10, 10, 10, 10, 10, 10, 10, 10,
    5, 5, 7, 8, 8, 7, 5, 5,
    2,  2, 3, 5, 5, 4,  2,  2,
    0,  0,  0, 5, 5,  0,  0,  0,
    2, -2,-3,  0,  0,-3, -2,  2,
    2, 3, 3,-4,-4, 3, 3,  2,
    0,  0,  0,  0,  0,  0,  0,  0
};

constexpr std::array<int, 64> black_pawn_pst = {
    0,  0,  0,  0,  0,  0,  0,  0,
    2, 3, 3,-4,-4, 3, 3,  2,
    2, -2,-3,  0,  0,-3, -2,  2,
    0,  0,  0, 5, 5,  0,  0,  0,
    2,  2, 3, 5, 5, 4,  2,  2,
    5, 5, 7, 8, 8, 7, 5, 5,
    10, 10, 10, 10, 10, 10, 10, 10,
    0,  0,  0,  0,  0,  0,  0,  0
};

constexpr std::array<int, 64> knight_pst = {
    -50, -40, -30, -30, -30, -30, -40, -50,
    -40, -20,   0,   0,   0,   0, -20, -40,
    -30,   0,  10,  15,  15,  10,   0, -30,
    -30,   5,  15,  20,  20,  15,   5, -30,
    -30,   0,  15,  20,  20,  15,   0, -30,
    -30,   5,  10,  15,  15,  10,   5, -30,
    -40, -20,   0,   5,   5,   0, -20, -40,
    -50, -40, -30, -30, -30, -30, -40, -50
};

constexpr std::array<int, 64> white_bishop_pst = {
    -20, -10, -10, -10, -10, -10, -10, -20,
    -10,   0,   0,   0,   0,   0,   0, -10,
    -10,   0,   5,  10,  10,   5,   0, -10,
    -10,   5,   5,  10,  10,   5,   5, -10,
    -10,   0,  10,  10,  10,  10,   0, -10,
    -10,  10,  10,  10,  10,  10,  10, -10,
    -10,   5,   0,   0,   0,   0,   5, -10,
    -20, -10, -10, -10, -10, -10, -10, -20
};

constexpr std::array<int, 64> black_bishop_pst = {
    -20, -10, -10, -10, -10, -10, -10, -20,
    -10,   5,   0,   0,   0,   0,   5, -10,
    -10,  10,  10,  10,  10,  10,  10, -10,
    -10,   0,  10,  10,  10,  10,   0, -10,
    -10,   5,   5,  10,  10,   5,   5, -10,
    -10,   0,   5,  10,  10,   5,   0, -10,
    -10,   0,   0,   0,   0,   0,   0, -10,
    -20, -10, -10, -10, -10, -10, -10, -20
};

constexpr std::array<int, 64> rook_pst = {
    -5, -3, 1, 3, 3, 1, -3, -5,
    -3, 1, 5, 7, 7, 5, 1, -3,
    -1, 5, 10, 12, 12, 10, 5, -1,
    0, 6, 11, 13, 13, 11, 6, 0,
    0, 6, 11, 13, 13, 11, 6, 0,
    -1, 5, 10, 12, 12, 10, 5, -1,
    -3, 1, 5, 7, 7, 5, 1, -3,
    -5, -3, 1, 3, 3, 1, -3, -5
};

constexpr std::array<int, 64> queen_pst = {
    -20, -10, -10, -5, -5, -10, -10, -20,
    -10,  0,   0,  0,  0,   0,   0, -10,
    -10,  0,   5,  5,  5,   5,   0, -10,
     -5,  0,   5,  5,  5,   5,   0,  -5,
      0,  0,   5,  5,  5,   5,   0,  -5,
    -10,  5,   5,  5,  5,   5,   0, -10,
    -10,  0,   5,  0,  0,   0,   0, -10,
    -20, -10, -10, -5, -5, -10, -10, -20
};
// clang-format on

template <typename Generator>
constexpr std::array<uint64_t, 64> create_pawn_table(Generator&& f)
{
  std::array<uint64_t, 64> square_values = {};
  for (int i = 0; i < 64; i++)
  {
    square_values[i] = f(i);
  }
  return square_values;
}

constexpr uint64_t pawn_file_mask(int square)
{
  uint64_t mask = 0;

  for (int i = 0; i < 64; i++)
  {
    if ((i != square) && ((i % 8) == square))
    {
      mask |= int_location_to_bitboard(i);
    }
  }

  return mask;
}

// define this as no pawn in file beside it (either side)
constexpr uint64_t pawn_isolation_mask(int square)
{
  uint64_t mask = 0;

  for (int i = 0; i < 64; i++)
  {
    if (i % 8 != 0 && i == square && square > -1)
    {
      mask |= int_location_to_bitboard(square - 1);
    }
    if (i % 8 != 7 && i == square && square < 64)
    {
      mask |= int_location_to_bitboard(square + 1);
    }
  }

  return mask;
}

constexpr auto pawn_file_masks = create_pawn_table(pawn_file_mask);
constexpr auto pawn_isolation_masks = create_pawn_table(pawn_isolation_mask);

int_fast32_t evaluate_position(Position* position);

#endif