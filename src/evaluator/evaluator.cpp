#include "evaluator.hpp"

#include "../util/magicbitboards.hpp"

int pawn_score(Position* position)
{
  int score = 0;
  int location;
  uint64_t white_pawn_locations = white_pawns(position);
  uint64_t black_pawn_locations = black_pawns(position);

  while (white_pawn_locations)
  {
    location = bitscan(white_pawn_locations);
    score += white_pawn_location_values[location];
    score += PAWN_VALUE;
    white_pawn_locations = set_bit_low(white_pawn_locations, location);
  }

  while (black_pawn_locations)
  {
    location = bitscan(black_pawn_locations);
    score -= black_pawn_location_values[location];
    score -= PAWN_VALUE;
    black_pawn_locations = set_bit_low(black_pawn_locations, location);
  }

  return score * (position->white_to_move ? -1 : 1);
}

int queen_score(Position* position)
{
  int score = 0;

  uint64_t wql = white_queens(position);
  uint64_t bql = black_queens(position);

  while (wql)
  {
    score += QUEEN_VALUE;
    location = bitscan(wql);
  }
}

int evaluate_position(Position* position)
{
  int score = 0;

  score += pawn_score(position);
  score += queen_score(position);

  return score;
}