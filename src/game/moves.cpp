#include "moves.hpp"

#include <algorithm>

Position make_move(Position* position, uint32_t move)
{
  Position new_position = *position;

  uint64_t from_square = int_location_to_bitboard(decode_from_square(move));
  uint64_t to_square = int_location_to_bitboard(decode_to_square(move));
  bool double_push = decode_double_push(move);
  bool castling = decode_castling(move);
  bool capture = decode_capture(move);
  bool enpassant = decode_enpassant(move);
  PieceAsInt promotion_piece = decode_promoted_to_piece(move);

  new_position.enPassantTarget = 0ull;
  new_position.white_to_move = !position->white_to_move;

  if (position->white_to_move)
  {
    new_position.white ^= from_square | to_square;
    if (double_push)
    {
      new_position.enPassantTarget |= to_square << 8;
    }
  }
  else
  {
    new_position.black ^= from_square | to_square;
    if (double_push)
    {
      new_position.enPassantTarget |= to_square >> 8;
    }
  }

  if (capture)
  {
    new_position.bishops ^= to_square;
    new_position.rooks ^= to_square;
    new_position.pawns ^= to_square;
    new_position.queens ^= to_square;
    new_position.knights ^= to_square;

    if (enpassant)
    {
      if (position->white_to_move)
      {
        new_position.pawns ^= to_square >> 8;
        new_position.black ^= to_square >> 8;
      }
      else
      {
        new_position.pawns ^= to_square << 8;
        new_position.white ^= to_square << 8;
      }
    }
  }

  switch (decode_moved_piece(move))
  {
    case KNIGHT:
      new_position.knights ^= from_square;
      new_position.knights |= to_square;
      break;
    case BISHOP:
      new_position.bishops ^= from_square;
      new_position.bishops |= to_square;
      break;
    case ROOK:
      new_position.rooks ^= from_square;
      new_position.rooks |= to_square;
      break;
    case QUEEN:
      new_position.queens ^= from_square;
      new_position.queens |= to_square;
      break;
    case PAWN:
      new_position.pawns ^= from_square;

      if (promotion_piece != NO_PIECE)
      {
        switch (promotion_piece)
        {
          case BISHOP:
            new_position.bishops |= to_square;
            break;
          case KNIGHT:
            new_position.knights |= to_square;
            break;
          case ROOK:
            new_position.rooks |= to_square;
            break;
          case QUEEN:
            new_position.queens |= to_square;
            break;
        }
      }
      else
      {
        new_position.pawns |= to_square;
      }
      break;
    case KING:
      new_position.kings ^= from_square;
      new_position.kings |= to_square;
      if (castling)
      {
        new_position.rooks ^= from_square;
        new_position.rooks |= to_square;

        switch (bitboard_to_square(to_square))
        {
          case (f1):
            new_position.white_can_castle_kingside = false;
            break;
          case (d1):
            new_position.white_can_castle_queenside = false;
            break;
          case (f8):
            new_position.black_can_castle_kingside = false;
            break;
          case (d8):
            new_position.black_can_castle_queenside = false;
            break;
        }
      }
      break;
    default:
      throw std::invalid_argument("Should never see this");
  }

  return new_position;
}

bool is_square_attacked(bool white, int square, Position* position)
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
    attacked |= is_square_attacked(white, i, &position);
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

std::vector<uint32_t> pseudolegal_possible_moves(Position* position)
{
  std::vector<uint32_t> pseudolegal_moves;
  if (position->white_to_move)
  {
    white_quiet_pawn_moves(position, &pseudolegal_moves);
    white_king_moves(position, &pseudolegal_moves);
    white_pawn_attacks(position, &pseudolegal_moves);
    white_knight_moves(position, &pseudolegal_moves);
  }
  else
  {
    black_quiet_pawn_moves(position, &pseudolegal_moves);
    black_king_moves(position, &pseudolegal_moves);
    black_pawn_attacks(position, &pseudolegal_moves);
    black_knight_moves(position, &pseudolegal_moves);
  }

  bishop_moves(position, &pseudolegal_moves);
  rook_moves(position, &pseudolegal_moves);
  queen_moves(position, &pseudolegal_moves);

  return pseudolegal_moves;
}

bool validate_move(Position* position, uint32_t move)
{
  Position intermediate_position = make_move(position, move);
  int king_position = bitboard_to_square(position->white_to_move ? white_kings(position) : black_kings(position));
  return is_square_attacked(position->white_to_move, king_position, position);
}

std::vector<uint32_t> valid_moves_for_position(Position position)
{
  std::vector<uint32_t> pseudolegal_moves = pseudolegal_possible_moves(&position);
  std::vector<uint32_t> legal_moves;

  for (uint32_t move : pseudolegal_moves)
  {
    if (validate_move(&position, move))
    {
      legal_moves.push_back(move);
    }
  }

  // needs a special validiaty check so handle it in the generation
  castling_moves(&position, &legal_moves);

  return legal_moves;
}

void white_quiet_pawn_moves(Position* position, std::vector<uint32_t>* moves)
{
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
        moves->push_back(
            encode_move(pawn_location, potential_location, true, PAWN, KNIGHT, false, false, false, false));
        moves->push_back(
            encode_move(pawn_location, potential_location, true, PAWN, BISHOP, false, false, false, false));
        moves->push_back(encode_move(pawn_location, potential_location, true, PAWN, ROOK, false, false, false, false));
        moves->push_back(encode_move(pawn_location, potential_location, true, PAWN, QUEEN, false, false, false, false));
      }
      else
      {
        moves->push_back(
            encode_move(pawn_location, potential_location, true, PAWN, NO_PIECE, false, false, false, false));

        if (potential_location > 39 && potential_location < 48)
        {
          potential_location -= 8;
          if (not_occupied_squares & int_location_to_bitboard(potential_location))
          {
            moves->push_back(
                encode_move(pawn_location, potential_location, true, PAWN, NO_PIECE, false, true, false, false));
          }
        }
      }
    }

    white_pawns_in_position = set_bit_low(white_pawns_in_position, pawn_location);
  }
}

void black_quiet_pawn_moves(Position* position, std::vector<uint32_t>* moves)
{
  int pawn_location, potential_location;
  uint64_t black_pawns_in_position = black_pawns(position);
  uint64_t not_occupied_squares = ~all_occupied(position);

  while (black_pawns_in_position)
  {
    pawn_location = bitscan(black_pawns_in_position);
    potential_location = pawn_location + 8;

    if (potential_location < 64 && (not_occupied_squares & int_location_to_bitboard(potential_location)))
    {
      if (potential_location > 55)
      {
        moves->push_back(
            encode_move(pawn_location, potential_location, false, PAWN, KNIGHT, false, false, false, false));
        moves->push_back(
            encode_move(pawn_location, potential_location, false, PAWN, BISHOP, false, false, false, false));
        moves->push_back(encode_move(pawn_location, potential_location, false, PAWN, ROOK, false, false, false, false));
        moves->push_back(
            encode_move(pawn_location, potential_location, false, PAWN, QUEEN, false, false, false, false));
      }
      else
      {
        moves->push_back(
            encode_move(pawn_location, potential_location, false, PAWN, NO_PIECE, false, false, false, false));

        if (potential_location > 15 && potential_location < 23)
        {
          potential_location += 8;
          if (not_occupied_squares & int_location_to_bitboard(potential_location))
          {
            moves->push_back(
                encode_move(pawn_location, potential_location, false, PAWN, NO_PIECE, false, true, false, false));
          }
        }
      }
    }

    black_pawns_in_position = set_bit_low(black_pawns_in_position, pawn_location);
  }
}

void white_pawn_attacks(Position* position, std::vector<uint32_t>* moves)
{
  int white_pawn_location, white_pawn_attack_location;
  uint64_t white_pawn_locations = white_pawns(position);
  uint64_t black_no_king = black_occupied_no_king(position);
  uint64_t potential_white_pawn_attack_bitboard;

  while (white_pawn_locations)
  {
    white_pawn_location = bitscan(white_pawn_locations);
    white_pawn_attack_location = white_pawn_location - 7;

    if (white_pawn_attack_location > 0)
    {
      potential_white_pawn_attack_bitboard = int_location_to_bitboard(white_pawn_attack_location);
      if (black_no_king & potential_white_pawn_attack_bitboard)
      {
        moves->push_back(encode_move(white_pawn_location, white_pawn_attack_location, true, PAWN, NO_PIECE, true, false,
                                     false, false));
      }

      white_pawn_attack_location -= 2;
      potential_white_pawn_attack_bitboard = int_location_to_bitboard(white_pawn_attack_location);
      if (white_pawn_attack_location > 0 && (black_no_king & potential_white_pawn_attack_bitboard))
      {
        moves->push_back(encode_move(white_pawn_location, white_pawn_attack_location, true, PAWN, NO_PIECE, true, false,
                                     false, false));
      }
    }

    white_pawn_locations = set_bit_low(white_pawn_locations, white_pawn_location);
  }
}

void black_pawn_attacks(Position* position, std::vector<uint32_t>* moves)
{
  int black_pawn_location, black_pawn_attack_location;
  uint64_t black_pawn_locations = black_pawns(position);
  uint64_t white_no_king = white_occupied_no_king(position);
  uint64_t potential_black_pawn_attack_bitboard;

  while (black_pawn_locations)
  {
    black_pawn_location = bitscan(black_pawn_locations);
    black_pawn_attack_location = black_pawn_location - 7;

    if (black_pawn_attack_location > 0)
    {
      potential_black_pawn_attack_bitboard = int_location_to_bitboard(black_pawn_attack_location);
      if (white_no_king & potential_black_pawn_attack_bitboard)
      {
        moves->push_back(encode_move(black_pawn_location, black_pawn_attack_location, true, PAWN, NO_PIECE, true, false,
                                     false, false));
      }

      black_pawn_attack_location -= 2;
      potential_black_pawn_attack_bitboard = int_location_to_bitboard(black_pawn_attack_location);
      if (black_pawn_attack_location > 0 && (white_no_king & potential_black_pawn_attack_bitboard))
      {
        moves->push_back(encode_move(black_pawn_location, black_pawn_attack_location, true, PAWN, NO_PIECE, true, false,
                                     false, false));
      }
    }

    black_pawn_locations = set_bit_low(black_pawn_locations, black_pawn_location);
  }
}

void white_king_moves(Position* position, std::vector<uint32_t>* moves)
{
  int king_location = bitscan(white_kings(position));
  uint64_t to_square_candidates = king_moves[king_location] & (~all_occupied(position));
  int to_square;

  while (to_square_candidates)
  {
    to_square = bitscan(to_square_candidates);
    moves->push_back(encode_move(king_location, to_square, true, KING, NO_PIECE, false, false, false, false));
    to_square_candidates = set_bit_low(to_square_candidates, to_square);
  }

  to_square_candidates = king_moves[king_location] & black_occupied_no_king(position);

  while (to_square_candidates)
  {
    to_square = bitscan(to_square_candidates);
    moves->push_back(encode_move(king_location, to_square, true, KING, NO_PIECE, true, false, false, false));
    to_square_candidates = set_bit_low(to_square_candidates, to_square);
  }
}

void black_king_moves(Position* position, std::vector<uint32_t>* moves)
{
  int king_location = bitscan(black_kings(position));
  uint64_t to_square_candidates = king_moves[king_location] & (~all_occupied(position));
  int to_square;

  while (to_square_candidates)
  {
    to_square = bitscan(to_square_candidates);
    moves->push_back(encode_move(king_location, to_square, false, KING, NO_PIECE, false, false, false, false));
    to_square_candidates = set_bit_low(to_square_candidates, to_square);
  }

  to_square_candidates = king_moves[king_location] & white_occupied_no_king(position);

  while (to_square_candidates)
  {
    to_square = bitscan(to_square_candidates);
    moves->push_back(encode_move(king_location, to_square, false, KING, NO_PIECE, true, false, false, false));
    to_square_candidates = set_bit_low(to_square_candidates, to_square);
  }
}

void white_knight_moves(Position* position, std::vector<uint32_t>* moves)
{
  uint64_t white_knights_in_position = white_knights(position);

  while (white_knights_in_position)
  {
    int knight_location = bitscan(white_knights_in_position);
    uint64_t to_square_candidates = knight_moves[knight_location] & (~all_occupied(position));
    int to_square;

    while (to_square_candidates)
    {
      to_square = bitscan(to_square_candidates);
      moves->push_back(encode_move(knight_location, to_square, true, KNIGHT, NO_PIECE, false, false, false, false));
      to_square_candidates = set_bit_low(to_square_candidates, to_square);
    }

    to_square_candidates = king_moves[knight_location] & black_occupied_no_king(position);

    while (to_square_candidates)
    {
      to_square = bitscan(to_square_candidates);
      moves->push_back(encode_move(knight_location, to_square, true, KNIGHT, NO_PIECE, true, false, false, false));
      to_square_candidates = set_bit_low(to_square_candidates, to_square);
    }

    white_knights_in_position = set_bit_low(white_knights_in_position, knight_location);
  }
}

void black_knight_moves(Position* position, std::vector<uint32_t>* moves)
{
  int knight_location = bitscan(black_knights(position));
  uint64_t to_square_candidates = knight_moves[knight_location] & (~all_occupied(position));
  int to_square;

  while (to_square_candidates)
  {
    to_square = bitscan(to_square_candidates);
    moves->push_back(encode_move(knight_location, to_square, false, KNIGHT, NO_PIECE, false, false, false, false));
    to_square_candidates = set_bit_low(to_square_candidates, to_square);
  }

  to_square_candidates = king_moves[knight_location] & white_occupied_no_king(position);

  while (to_square_candidates)
  {
    to_square = bitscan(to_square_candidates);
    moves->push_back(encode_move(knight_location, to_square, false, KNIGHT, NO_PIECE, true, false, false, false));
    to_square_candidates = set_bit_low(to_square_candidates, to_square);
  }
}

void bishop_moves(Position* position, std::vector<uint32_t>* moves)
{
  bool white = position->white_to_move;
  int bishop_location, target_location;
  uint64_t occupancy = all_occupied(position);
  uint64_t my_pieces = white ? position->white : position->black;
  uint64_t bishops = white ? white_bishops(position) : black_bishops(position);
  uint64_t opponent_no_king = white ? black_occupied_no_king(position) : white_occupied_no_king(position);
  uint64_t quiet_moves, attacks;

  while (bishops)
  {
    bishop_location = bitscan(bishops);
    quiet_moves = bishop_attacks(occupancy, bishop_location);
    attacks = quiet_moves & opponent_no_king;
    quiet_moves &= ~(my_pieces | attacks);

    while (attacks)
    {
      target_location = bitscan(attacks);
      moves->push_back(
          encode_move(bishop_location, target_location, white, BISHOP, NO_PIECE, true, false, false, false));
      attacks = set_bit_low(attacks, target_location);
    }

    while (quiet_moves)
    {
      target_location = bitscan(quiet_moves);
      moves->push_back(
          encode_move(bishop_location, target_location, white, BISHOP, NO_PIECE, false, false, false, false));
      quiet_moves = set_bit_low(quiet_moves, target_location);
    }

    bishops = set_bit_low(bishops, bishop_location);
  }
}

void rook_moves(Position* position, std::vector<uint32_t>* moves)
{
  bool white = position->white_to_move;
  int rook_location, target_location;
  uint64_t occupancy = all_occupied(position);
  uint64_t my_pieces = white ? position->white : position->black;
  uint64_t rooks = white ? white_rooks(position) : black_rooks(position);
  uint64_t opponent_no_king = white ? black_occupied_no_king(position) : white_occupied_no_king(position);
  uint64_t quiet_moves, attacks;

  while (rooks)
  {
    rook_location = bitscan(rooks);
    quiet_moves = rook_attacks(occupancy, rook_location);
    attacks = quiet_moves & opponent_no_king;
    quiet_moves &= ~(my_pieces | attacks);

    while (attacks)
    {
      target_location = bitscan(attacks);
      moves->push_back(encode_move(rook_location, target_location, white, ROOK, NO_PIECE, true, false, false, false));
      attacks = set_bit_low(attacks, target_location);
    }

    while (quiet_moves)
    {
      target_location = bitscan(quiet_moves);
      moves->push_back(encode_move(rook_location, target_location, white, ROOK, NO_PIECE, false, false, false, false));
      quiet_moves = set_bit_low(quiet_moves, target_location);
    }

    rooks = set_bit_low(rooks, rook_location);
  }
}

void queen_moves(Position* position, std::vector<uint32_t>* moves)
{
  bool white = position->white_to_move;
  int queen_location, target_location;
  uint64_t occupancy = all_occupied(position);
  uint64_t my_pieces = white ? position->white : position->black;
  uint64_t queens = white ? white_queens(position) : black_queens(position);
  uint64_t opponent_no_king = white ? black_occupied_no_king(position) : white_occupied_no_king(position);
  uint64_t quiet_moves, attacks;

  while (queens)
  {
    queen_location = bitscan(queens);
    quiet_moves = queen_attacks(occupancy, queen_location);
    attacks = quiet_moves & opponent_no_king;
    quiet_moves &= ~(my_pieces | attacks);

    while (attacks)
    {
      target_location = bitscan(attacks);
      moves->push_back(encode_move(queen_location, target_location, white, QUEEN, NO_PIECE, true, false, false, false));
      attacks = set_bit_low(attacks, target_location);
    }

    while (quiet_moves)
    {
      target_location = bitscan(quiet_moves);
      moves->push_back(
          encode_move(queen_location, target_location, white, QUEEN, NO_PIECE, false, false, false, false));
      quiet_moves = set_bit_low(quiet_moves, target_location);
    }

    queens = set_bit_low(queens, queen_location);
  }
}

void castling_moves(Position* position, std::vector<uint32_t>* moves)
{
  if (position->white_to_move)
  {
    if (position->white_can_castle_kingside && !is_square_attacked(position->white_to_move, e1, position) &&
        !is_square_attacked(position->white_to_move, f1, position) &&
        !is_square_attacked(position->white_to_move, g1, position) &&
        ~(all_occupied(position) & WHITE_KINGSIDE_CASTLE_MASK))
    {
      moves->push_back(encode_move(e1, g1, true, KING, NO_PIECE, false, false, false, true));
    }
    if (position->white_can_castle_queenside && !is_square_attacked(position->white_to_move, b1, position) &&
        !is_square_attacked(position->white, c1, position) &&
        !is_square_attacked(position->white_to_move, d1, position) &&
        !is_square_attacked(position->white_to_move, e1, position) &&
        ~(all_occupied(position) & WHITE_QUEENSIDE_CASTLE_MASK))
    {
      moves->push_back(encode_move(e1, c1, true, KING, NO_PIECE, false, false, false, true));
    }
  }
  else
  {
    if (position->black_can_castle_queenside && !is_square_attacked(position->white_to_move, e8, position) &&
        !is_square_attacked(position->white_to_move, f8, position) &&
        !is_square_attacked(position->white_to_move, g8, position) &&
        ~(all_occupied(position) & WHITE_KINGSIDE_CASTLE_MASK))
    {
      moves->push_back(encode_move(e1, g1, false, KING, NO_PIECE, false, false, false, true));
    }
    if (position->black_can_castle_queenside && !is_square_attacked(position->white_to_move, b8, position) &&
        !is_square_attacked(position->white, c8, position) &&
        !is_square_attacked(position->white_to_move, d8, position) &&
        !is_square_attacked(position->white_to_move, e8, position) &&
        ~(all_occupied(position) & BLACK_QUEENSIDE_CASTLE_MASK))
    {
      moves->push_back(encode_move(e1, c1, false, KING, NO_PIECE, false, false, false, true));
    }
  }
}
