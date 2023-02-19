#define QUEEN_VALUE 900
#define ROOK_VALUE 500
#define BISHOP_VALUE 320
#define KNIGHT_VALUE 300
#define PAWN_VALUE 100

#include "evaluator.hpp"

int white_material_score(Position* position)
{
  int score = 0;
  int location;

  uint64_t queens = white_queens(position);
  while (queens)
  {
    location = bitscan(queens);
    score += QUEEN_VALUE;
    score += queen_pst[location];
    queens = set_bit_low(queens, location);
  }

  uint64_t rooks = white_rooks(position);
  while (rooks)
  {
    location = bitscan(rooks);
    score += ROOK_VALUE;
    score += rook_pst[location];
    rooks = set_bit_low(rooks, location);
  }

  uint64_t bishops = white_bishops(position);
  while (bishops)
  {
    location = bitscan(bishops);
    score += BISHOP_VALUE;
    score += white_bishop_pst[location];
    bishops = set_bit_low(bishops, location);
  }

  uint64_t knights = white_knights(position);
  while (knights)
  {
    location = bitscan(knights);
    score += KNIGHT_VALUE;
    score += knight_pst[location];
    knights = set_bit_low(knights, location);
  }

  uint64_t pawns = white_pawns(position);
  while (pawns)
  {
    location = bitscan(pawns);
    score += PAWN_VALUE;
    score += white_pawn_pst[location];
    pawns = set_bit_low(pawns, location);
  }

  return score;
}

int black_material_score(Position* position)
{
  int score = 0;
  int location;

  uint64_t queens = black_queens(position);
  while (queens)
  {
    location = bitscan(queens);
    score += QUEEN_VALUE;
    score += queen_pst[location];
    queens = set_bit_low(queens, location);
  }

  uint64_t rooks = black_rooks(position);
  while (rooks)
  {
    location = bitscan(rooks);
    score += ROOK_VALUE;
    score += rook_pst[location];
    rooks = set_bit_low(rooks, location);
  }

  uint64_t bishops = black_bishops(position);
  while (bishops)
  {
    location = bitscan(bishops);
    score += BISHOP_VALUE;
    score += black_bishop_pst[location];
    bishops = set_bit_low(bishops, location);
  }

  uint64_t knights = black_knights(position);
  while (knights)
  {
    location = bitscan(knights);
    score += KNIGHT_VALUE;
    score += knight_pst[location];
    knights = set_bit_low(knights, location);
  }

  uint64_t pawns = black_pawns(position);
  while (pawns)
  {
    location = bitscan(pawns);
    score += PAWN_VALUE;
    score += black_pawn_pst[location];
    pawns = set_bit_low(pawns, location);
  }

  return score;
}

int evaluate_position(Position* position)
{
  int white_score = white_material_score(position);
  int black_score = black_material_score(position);

  return position->white_to_move ? white_score - black_score : black_score - white_score;
}