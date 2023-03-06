#define QUEEN_VALUE 1100
#define ROOK_VALUE 500
#define BISHOP_VALUE 350
#define KNIGHT_VALUE 325
#define PAWN_VALUE 100

#include "evaluator.hpp"

#include "../game/moves.hpp"

int_fast32_t board_control_difference(Position* position)
{
  int_fast32_t score = 0;

  score += bitcount(attacked_squares(true, position)) * 5;
  score -= bitcount(attacked_squares(false, position)) * 5;

  return score;
}

int_fast32_t pawn_score(uint64_t my_pawns, uint64_t my_pieces, uint64_t their_pawns)
{
  uint64_t current_mask;
  uint64_t all_my_pawns = my_pawns;
  int_fast32_t score = 0;

  while (my_pawns)
  {
    int location = bitscan(my_pawns);

    // Pawn peice square val
    score += PAWN_VALUE;
    score += white_pawn_pst[location];

    // Doubled pawns and passed pawns
    current_mask = pawn_file_masks[location];
    if (current_mask & all_my_pawns)
    {
      score -= 10;
    }
    // we also don't want to block pawns with pieces, this is bad
    if (current_mask & (~all_my_pawns & my_pieces))
    {
      score -= 30;
    }
    if (!(current_mask & their_pawns))
    {
      score += 20;
    }

    // Isolated and connected pawns
    current_mask = pawn_isolation_masks[location];
    if (!(current_mask & all_my_pawns))
    {
      score -= 10;
    }
    else
    {
      score += 10;
    }

    my_pawns = set_bit_low(my_pawns, location);
  }

  return score;
}

int_fast32_t pawn_evaluation(Position* position)
{
  uint64_t wpawns = white_pawns(position);
  uint64_t bpawns = black_pawns(position);
  uint64_t wpieces = white_occupied_no_king(position);
  uint64_t bpieces = black_occupied_no_king(position);

  return pawn_score(wpawns, wpieces, bpawns) - pawn_score(bpawns, bpieces, wpawns);
}

int_fast32_t white_material_score(Position* position)
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

  return score;
}

int_fast32_t black_material_score(Position* position)
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

  return score;
}

int_fast32_t evaluate_position(Position* position)
{
  int_fast32_t white_score = white_material_score(position);
  int_fast32_t black_score = black_material_score(position);

  int_fast32_t pawn_score = pawn_evaluation(position);
  int_fast32_t board_control_score = board_control_difference(position);

  return ((white_score - black_score) + pawn_score + board_control_score) * (position->white_to_move ? 1 : -1);
}
