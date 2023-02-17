#ifndef EVALUATOR_H
#define EVALUATOR_H

#include <array>

#include "../util/global.hpp"

#define QUEEN_VALUE 900
#define ROOK_VALUE 500
#define BISHOP_VALUE 320
#define KNIGHT_VALUE 300
#define PAWN_VALUE 100

// clang-format off
constexpr std::array<int, 64> white_pawn_location_values = {
  0, 0, 0, 0, 0, 0, 0, 0,
  30, 50, 50, 50, 50, 50, 50, 30,
  30, 48, 48, 48, 48, 48, 48, 30,
  30, 45, 46, 48, 48, 46, 45, 30,
  30, 35, 40, 40, 40, 40, 35, 30,
  20, 20, 20, 20, 20, 20, 20, 20,
  0, 0, 0, 0, 0, 0, 0, 0,
};

constexpr std::array<int, 64> black_pawn_location_values = {
  0, 0, 0, 0, 0, 0, 0, 0,
  20, 20, 20, 20, 20, 20, 20, 20,
  30, 35, 40, 40, 40, 40, 35, 30,
  30, 45, 46, 48, 48, 46, 45, 30,
  30, 48, 48, 48, 48, 48, 48, 30,
  30, 50, 50, 50, 50, 50, 50, 30,
  0, 0, 0, 0, 0, 0, 0, 0,
};
// clang-format on

constexpr std::array<int, 64> pawn_square_value = {};

int evaluate_position(Position* position);

#endif