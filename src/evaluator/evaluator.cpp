#include "evaluator.hpp"

#include "../util/magicbitboards.hpp"

int material_score(Position* position)
{
  constexpr int pawn_score = 100;
  constexpr int knight_score = 320;
  constexpr int bishop_score = 330;
  constexpr int rook_score = 500;
  constexpr int queen_score = 900;

  int score = 0;
  score += bitcount(white_pawns(position)) * pawn_score;
  score += bitcount(white_knights(position)) * knight_score;
  score += bitcount(white_bishops(position)) * bishop_score;
  score += bitcount(white_rooks(position)) * rook_score;
  score += bitcount(white_queens(position)) * queen_score;

  score -= bitcount(black_pawns(position)) * pawn_score;
  score -= bitcount(black_knights(position)) * knight_score;
  score -= bitcount(black_bishops(position)) * bishop_score;
  score -= bitcount(black_rooks(position)) * rook_score;
  score -= bitcount(black_queens(position)) * queen_score;

  return score;
}

int evaluate_position(Position* position)
{
  int score = 0;

  score += material_score(position);

  return score * (position->white_to_move ? -1 : 1);
}