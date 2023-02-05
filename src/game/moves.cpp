#include "moves.hpp"

#include "../util/magicbitboards.hpp"

bool is_square_attacked(bool white, int square, Position position)
{
  return (white && (pawn_attacks[0][square] & white_pawns(position))) ||
         (!white && (pawn_attacks[1][square] & black_pawns(position))) ||
         (knight_moves[square] & (white ? white_knights(position) : black_knights(position))) ||
         (bishop_attacks(all_occupied(position), square) &
          (white ? white_bishops(position) : black_bishops(position))) ||
         (rook_attacks(all_occupied(position), square) & (white ? white_rooks(position) : black_rooks(position))) ||
         (queen_attacks(all_occupied(position), square) & (white ? white_queens(position) : black_queens(position))) ||
         (king_moves[square] & (white ? white_kings(position) : black_kings(position)));
}

uint64_t attacked_squares(bool white, Position position)
{
  uint64_t attacked = 0;
  for (int i = 63; i > -1; i--)
  {
    attacked <<= 1;
    attacked |= is_square_attacked(white, i, position);
  }

  return attacked;
}