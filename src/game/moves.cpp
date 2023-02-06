#include "moves.hpp"

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

uint32_t encode_move(int from_square, int to_square, bool whites_turn, int moved_peice, int promoted_to_piece,
                     bool capture, bool double_push, int enpassant, int castling)
{
  return (static_cast<uint32_t>(castling) << 22) | (static_cast<uint32_t>(enpassant) << 21) |
         (static_cast<uint32_t>(double_push) << 20) | (static_cast<uint32_t>(capture) << 19) |
         (static_cast<uint32_t>(promoted_to_piece) << 16) | (static_cast<uint32_t>(moved_peice) << 13) |
         (static_cast<uint32_t>(whites_turn) << 12) | (static_cast<uint32_t>(to_square) << 6) |
         (static_cast<uint32_t>(from_square));
}

std::vector<uint32_t> white_quiet_pawn_moves(Position position)
{
  std::vector<uint32_t> moves;
  int pawn_location, potential_location;
  uint64_t white_pawns_in_position = white_pawns(position);
  uint64_t not_occupied_squares = ~all_occupied(position);

  while (white_pawns_in_position)
  {
    pawn_location = bitscan(white_pawns_in_position);
    potential_location = pawn_location - 8;

    if (potential_location > 0 && (not_occupied_squares & int_location_to_bitboard(potential_location)))
    {
      if (potential_location < 8)
      {
        moves.push_back(encode_move(pawn_location, potential_location, true, PAWN, KNIGHT, false, false, false, false));
        moves.push_back(encode_move(pawn_location, potential_location, true, PAWN, BISHOP, false, false, false, false));
        moves.push_back(encode_move(pawn_location, potential_location, true, PAWN, ROOK, false, false, false, false));
        moves.push_back(encode_move(pawn_location, potential_location, true, PAWN, QUEEN, false, false, false, false));
      }
      else
      {
        moves.push_back(
            encode_move(pawn_location, potential_location, true, PAWN, NO_PIECE, false, false, false, false));

        if (potential_location > 39 && potential_location < 48)
        {
          potential_location -= 8;
          if (not_occupied_squares & int_location_to_bitboard(potential_location))
          {
            moves.push_back(
                encode_move(pawn_location, potential_location, true, PAWN, NO_PIECE, false, true, true, false));
          }
        }
      }
    }

    white_pawns_in_position = set_bit_low(white_pawns_in_position, pawn_location);
  }

  return moves;
}