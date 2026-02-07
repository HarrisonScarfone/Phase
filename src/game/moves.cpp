#include "moves.hpp"

#include <algorithm>

#include "../util/zobrist.hpp"

PieceAsInt victim_on_square(Position* position, Square square)
{
  uint64_t target_bitboard = int_location_to_bitboard(square);
  if (position->pawns & target_bitboard)
  {
    return PAWN;
  }
  else if (position->knights & target_bitboard)
  {
    return KNIGHT;
  }
  else if (position->bishops & target_bitboard)
  {
    return BISHOP;
  }
  else if (position->rooks & target_bitboard)
  {
    return ROOK;
  }
  else if (position->queens & target_bitboard)
  {
    return QUEEN;
  }
  else if (position->kings & target_bitboard)
  {
    return KING;
  }
  else
  {
    // we are attacking an enpassant square
    return PAWN;
  }
}

Position make_move(Position* position, uint32_t move)
{
  Position new_position = *position;

  int from_sq = decode_from_square(move);
  int to_sq = decode_to_square(move);
  uint64_t from_square = int_location_to_bitboard(from_sq);
  uint64_t to_square = int_location_to_bitboard(to_sq);
  bool double_push = decode_double_push(move);
  bool castling = decode_castling(move);
  bool capture = decode_capture(move);
  bool enpassant = decode_enpassant(move);
  PieceAsInt moved_piece = decode_moved_piece(move);
  PieceAsInt promotion_piece = decode_promoted_to_piece(move);

  // Color of moving side: 1 = white, 0 = black
  int us = position->white_to_move ? 1 : 0;
  int them = 1 - us;

  // Start with current hash
  uint64_t hash = position->hash;

  // XOR out old castling rights
  hash ^= zobrist::castling_keys[zobrist::castling_rights_index(*position)];

  // XOR out old en passant file if any
  if (position->enPassantTarget)
  {
    int ep_file = file_of(bitscan(position->enPassantTarget));
    hash ^= zobrist::ep_file_keys[ep_file];
  }

  // Flip side to move
  hash ^= zobrist::side_key;

  // Update half-move clock (reset on pawn move or capture)
  if (moved_piece == PAWN || capture)
  {
    new_position.half_move_clock = 0;
  }
  else
  {
    new_position.half_move_clock++;
  }

  // Increment full move clock after black's move
  if (!position->white_to_move)
  {
    new_position.full_move_clock++;
  }

  new_position.enPassantTarget = 0ull;
  new_position.white_to_move = !position->white_to_move;

  // Remove moving piece from source square (hash)
  hash ^= zobrist::piece_keys[us][moved_piece][from_sq];

  if (position->white_to_move)
  {
    new_position.white ^= from_square | to_square;
    new_position.black &= ~to_square;
    if (double_push)
    {
      new_position.enPassantTarget |= to_square << 8;
    }
  }
  else
  {
    new_position.black ^= from_square | to_square;
    new_position.white &= ~to_square;
    if (double_push)
    {
      new_position.enPassantTarget |= to_square >> 8;
    }
  }

  if (capture)
  {
    // Determine captured piece type for hash update
    PieceAsInt captured_piece;
    int captured_sq = to_sq;

    if (enpassant)
    {
      captured_piece = PAWN;
      captured_sq = position->white_to_move ? to_sq + 8 : to_sq - 8;
    }
    else
    {
      // Determine captured piece from position bitboards
      if (position->pawns & to_square)
        captured_piece = PAWN;
      else if (position->knights & to_square)
        captured_piece = KNIGHT;
      else if (position->bishops & to_square)
        captured_piece = BISHOP;
      else if (position->rooks & to_square)
        captured_piece = ROOK;
      else
        captured_piece = QUEEN;
    }

    // XOR out captured piece from hash
    hash ^= zobrist::piece_keys[them][captured_piece][captured_sq];

    // Also revoke castling rights if capturing a rook on its starting square
    if (to_square == int_location_to_bitboard(h1))
      new_position.white_can_castle_kingside = false;
    else if (to_square == int_location_to_bitboard(a1))
      new_position.white_can_castle_queenside = false;
    else if (to_square == int_location_to_bitboard(h8))
      new_position.black_can_castle_kingside = false;
    else if (to_square == int_location_to_bitboard(a8))
      new_position.black_can_castle_queenside = false;

    new_position.bishops &= ~to_square;
    new_position.rooks &= ~to_square;
    new_position.pawns &= ~to_square;
    new_position.queens &= ~to_square;
    new_position.knights &= ~to_square;

    if (enpassant)
    {
      if (position->white_to_move)
      {
        new_position.pawns ^= to_square << 8;
        new_position.black ^= to_square << 8;
      }
      else
      {
        new_position.pawns ^= to_square >> 8;
        new_position.white ^= to_square >> 8;
      }
    }
  }

  switch (moved_piece)
  {
    case KNIGHT:
      new_position.knights ^= from_square;
      new_position.knights |= to_square;
      hash ^= zobrist::piece_keys[us][KNIGHT][to_sq];
      break;
    case BISHOP:
      new_position.bishops ^= from_square;
      new_position.bishops |= to_square;
      hash ^= zobrist::piece_keys[us][BISHOP][to_sq];
      break;
    case ROOK:
      new_position.rooks ^= from_square;
      new_position.rooks |= to_square;
      hash ^= zobrist::piece_keys[us][ROOK][to_sq];

      if (position->white_to_move)
      {
        if (from_square == int_location_to_bitboard(h1))
        {
          new_position.white_can_castle_kingside = false;
        }
        else if (from_square == int_location_to_bitboard(a1))
        {
          new_position.white_can_castle_queenside = false;
        }
      }
      else
      {
        if (from_square == int_location_to_bitboard(h8))
        {
          new_position.black_can_castle_kingside = false;
        }
        else if (from_square == int_location_to_bitboard(a8))
        {
          new_position.black_can_castle_queenside = false;
        }
      }
      break;
    case QUEEN:
      new_position.queens ^= from_square;
      new_position.queens |= to_square;
      hash ^= zobrist::piece_keys[us][QUEEN][to_sq];
      break;
    case PAWN:
      new_position.pawns ^= from_square;

      if (promotion_piece != NO_PIECE)
      {
        // Add promoted piece to hash (not pawn)
        hash ^= zobrist::piece_keys[us][promotion_piece][to_sq];
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
          default:
            break;
        }
      }
      else
      {
        new_position.pawns |= to_square;
        hash ^= zobrist::piece_keys[us][PAWN][to_sq];
      }
      break;
    case KING:
      new_position.kings ^= from_square;
      new_position.kings |= to_square;
      hash ^= zobrist::piece_keys[us][KING][to_sq];

      if (castling)
      {
        if (position->white_to_move)
        {
          if (to_square == int_location_to_bitboard(g1))
          {
            // Kingside castle - move rook from h1 to f1
            new_position.rooks ^= int_location_to_bitboard(h1);
            new_position.rooks |= int_location_to_bitboard(f1);
            new_position.white ^= int_location_to_bitboard(h1);
            new_position.white |= int_location_to_bitboard(f1);
            hash ^= zobrist::piece_keys[1][ROOK][h1];
            hash ^= zobrist::piece_keys[1][ROOK][f1];
          }
          else if (to_square == int_location_to_bitboard(c1))
          {
            // Queenside castle - move rook from a1 to d1
            new_position.rooks ^= int_location_to_bitboard(a1);
            new_position.rooks |= int_location_to_bitboard(d1);
            new_position.white ^= int_location_to_bitboard(a1);
            new_position.white |= int_location_to_bitboard(d1);
            hash ^= zobrist::piece_keys[1][ROOK][a1];
            hash ^= zobrist::piece_keys[1][ROOK][d1];
          }
        }
        else
        {
          if (to_square == int_location_to_bitboard(g8))
          {
            // Kingside castle - move rook from h8 to f8
            new_position.rooks ^= int_location_to_bitboard(h8);
            new_position.rooks |= int_location_to_bitboard(f8);
            new_position.black ^= int_location_to_bitboard(h8);
            new_position.black |= int_location_to_bitboard(f8);
            hash ^= zobrist::piece_keys[0][ROOK][h8];
            hash ^= zobrist::piece_keys[0][ROOK][f8];
          }
          else if (to_square == int_location_to_bitboard(c8))
          {
            // Queenside castle - move rook from a8 to d8
            new_position.rooks ^= int_location_to_bitboard(a8);
            new_position.rooks |= int_location_to_bitboard(d8);
            new_position.black ^= int_location_to_bitboard(a8);
            new_position.black |= int_location_to_bitboard(d8);
            hash ^= zobrist::piece_keys[0][ROOK][a8];
            hash ^= zobrist::piece_keys[0][ROOK][d8];
          }
        }
      }
      if (position->white_to_move)
      {
        new_position.white_can_castle_queenside = false;
        new_position.white_can_castle_kingside = false;
      }
      else
      {
        new_position.black_can_castle_kingside = false;
        new_position.black_can_castle_queenside = false;
      }
      break;
    default:
      break;
  }

  // XOR in new castling rights
  hash ^= zobrist::castling_keys[zobrist::castling_rights_index(new_position)];

  // XOR in new en passant file if any
  if (new_position.enPassantTarget)
  {
    int ep_file = file_of(bitscan(new_position.enPassantTarget));
    hash ^= zobrist::ep_file_keys[ep_file];
  }

  new_position.hash = hash;

  return new_position;
}

bool is_square_attacked(bool white, int square, Position* position)
{
  return (white && (pawn_attacks[1][square] & black_pawns(position))) ||
         (!white && (pawn_attacks[0][square] & white_pawns(position))) ||
         (knight_moves[square] & (white ? black_knights(position) : white_knights(position))) ||
         (bishop_attacks(all_occupied(position), square) &
          (white ? black_bishops(position) : white_bishops(position))) ||
         (rook_attacks(all_occupied(position), square) & (white ? black_rooks(position) : white_rooks(position))) ||
         (queen_attacks(all_occupied(position), square) & (white ? black_queens(position) : white_queens(position))) ||
         (king_moves[square] & (white ? black_kings(position) : white_kings(position)));
}

uint64_t attacked_squares(bool white, Position* position)
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
                     bool capture, bool double_push, bool enpassant, bool castling)
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
  castling_moves(position, &pseudolegal_moves);

  return pseudolegal_moves;
}

bool validate_move(Position* position, uint32_t move)
{
  Position intermediate_position = make_move(position, move);
  int king_position =
      bitscan(position->white_to_move ? white_kings(&intermediate_position) : black_kings(&intermediate_position));

  return !is_square_attacked(position->white_to_move, king_position, &intermediate_position);
}

bool is_a_check(Position* position, uint32_t move)
{
  Position intermediate_position = make_move(position, move);
  int king_position =
      bitscan(position->white_to_move ? black_kings(&intermediate_position) : white_kings(&intermediate_position));
  bool is_check = is_square_attacked(intermediate_position.white_to_move, king_position, &intermediate_position);

  return is_check;
}

std::vector<uint32_t> valid_moves_for_position(Position position)
{
  std::vector<uint32_t> pseudolegal_moves = pseudolegal_possible_moves(&position);
  std::vector<uint32_t> legal_moves;

  for (uint32_t move : pseudolegal_moves)
  {
    if (validate_move(&position, move))
    {
      if (is_a_check(&position, move))
      {
        legal_moves.push_back(move | move_masks[9]);
      }
      else
      {
        legal_moves.push_back(move);
      }
    }
  }

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

    if (potential_location > -1 && (not_occupied_squares & int_location_to_bitboard(potential_location)))
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

        if (potential_location > 15 && potential_location < 24)
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
  uint64_t targets = black_no_king | position->enPassantTarget;

  while (white_pawn_locations)
  {
    white_pawn_location = bitscan(white_pawn_locations);
    white_pawn_attack_location = white_pawn_location - 7;

    if (white_pawn_attack_location > -1)
    {
      potential_white_pawn_attack_bitboard = int_location_to_bitboard(white_pawn_attack_location);

      if ((targets & potential_white_pawn_attack_bitboard) && (white_pawn_location % 8 != 7))
      {
        if (white_pawn_attack_location < 8)
        {
          moves->push_back(encode_move(white_pawn_location, white_pawn_attack_location, true, PAWN, KNIGHT, true, false,
                                       position->enPassantTarget & int_location_to_bitboard(white_pawn_attack_location),
                                       false));
          moves->push_back(encode_move(white_pawn_location, white_pawn_attack_location, true, PAWN, BISHOP, true, false,
                                       position->enPassantTarget & int_location_to_bitboard(white_pawn_attack_location),
                                       false));
          moves->push_back(encode_move(white_pawn_location, white_pawn_attack_location, true, PAWN, ROOK, true, false,
                                       position->enPassantTarget & int_location_to_bitboard(white_pawn_attack_location),
                                       false));
          moves->push_back(encode_move(white_pawn_location, white_pawn_attack_location, true, PAWN, QUEEN, true, false,
                                       position->enPassantTarget & int_location_to_bitboard(white_pawn_attack_location),
                                       false));
        }
        else
        {
          moves->push_back(
              encode_move(white_pawn_location, white_pawn_attack_location, true, PAWN, NO_PIECE, true, false,
                          position->enPassantTarget & int_location_to_bitboard(white_pawn_attack_location), false));
        }
      }

      white_pawn_attack_location -= 2;

      potential_white_pawn_attack_bitboard = int_location_to_bitboard(white_pawn_attack_location);
      if (white_pawn_attack_location > -1 && (white_pawn_location % 8 != 0) &&
          (targets & potential_white_pawn_attack_bitboard))
      {
        if (white_pawn_attack_location < 8)
        {
          moves->push_back(encode_move(white_pawn_location, white_pawn_attack_location, true, PAWN, KNIGHT, true, false,
                                       position->enPassantTarget & int_location_to_bitboard(white_pawn_attack_location),
                                       false));
          moves->push_back(encode_move(white_pawn_location, white_pawn_attack_location, true, PAWN, BISHOP, true, false,
                                       position->enPassantTarget & int_location_to_bitboard(white_pawn_attack_location),
                                       false));
          moves->push_back(encode_move(white_pawn_location, white_pawn_attack_location, true, PAWN, ROOK, true, false,
                                       position->enPassantTarget & int_location_to_bitboard(white_pawn_attack_location),
                                       false));
          moves->push_back(encode_move(white_pawn_location, white_pawn_attack_location, true, PAWN, QUEEN, true, false,
                                       position->enPassantTarget & int_location_to_bitboard(white_pawn_attack_location),
                                       false));
        }
        else
        {
          moves->push_back(
              encode_move(white_pawn_location, white_pawn_attack_location, true, PAWN, NO_PIECE, true, false,
                          position->enPassantTarget & int_location_to_bitboard(white_pawn_attack_location), false));
        }
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
  uint64_t targets = white_no_king | position->enPassantTarget;

  while (black_pawn_locations)
  {
    black_pawn_location = bitscan(black_pawn_locations);
    black_pawn_attack_location = black_pawn_location + 7;

    if (black_pawn_attack_location < 64)
    {
      potential_black_pawn_attack_bitboard = int_location_to_bitboard(black_pawn_attack_location);
      if ((targets & potential_black_pawn_attack_bitboard) && (black_pawn_location % 8 != 0))
      {
        if (black_pawn_attack_location > 55)
        {
          moves->push_back(encode_move(black_pawn_location, black_pawn_attack_location, true, PAWN, KNIGHT, true, false,
                                       position->enPassantTarget & int_location_to_bitboard(black_pawn_attack_location),
                                       false));
          moves->push_back(encode_move(black_pawn_location, black_pawn_attack_location, true, PAWN, BISHOP, true, false,
                                       position->enPassantTarget & int_location_to_bitboard(black_pawn_attack_location),
                                       false));
          moves->push_back(encode_move(black_pawn_location, black_pawn_attack_location, true, PAWN, ROOK, true, false,
                                       position->enPassantTarget & int_location_to_bitboard(black_pawn_attack_location),
                                       false));
          moves->push_back(encode_move(black_pawn_location, black_pawn_attack_location, true, PAWN, QUEEN, true, false,
                                       position->enPassantTarget & int_location_to_bitboard(black_pawn_attack_location),
                                       false));
        }
        else
        {
          moves->push_back(
              encode_move(black_pawn_location, black_pawn_attack_location, true, PAWN, NO_PIECE, true, false,
                          position->enPassantTarget & int_location_to_bitboard(black_pawn_attack_location), false));
        }
      }

      black_pawn_attack_location += 2;
      potential_black_pawn_attack_bitboard = int_location_to_bitboard(black_pawn_attack_location);
      if (black_pawn_attack_location < 64 && (targets & potential_black_pawn_attack_bitboard) &&
          (black_pawn_location % 8 != 7))
      {
        if (black_pawn_attack_location > 55)
        {
          moves->push_back(encode_move(black_pawn_location, black_pawn_attack_location, true, PAWN, KNIGHT, true, false,
                                       position->enPassantTarget & int_location_to_bitboard(black_pawn_attack_location),
                                       false));
          moves->push_back(encode_move(black_pawn_location, black_pawn_attack_location, true, PAWN, BISHOP, true, false,
                                       position->enPassantTarget & int_location_to_bitboard(black_pawn_attack_location),
                                       false));
          moves->push_back(encode_move(black_pawn_location, black_pawn_attack_location, true, PAWN, ROOK, true, false,
                                       position->enPassantTarget & int_location_to_bitboard(black_pawn_attack_location),
                                       false));
          moves->push_back(encode_move(black_pawn_location, black_pawn_attack_location, true, PAWN, QUEEN, true, false,
                                       position->enPassantTarget & int_location_to_bitboard(black_pawn_attack_location),
                                       false));
        }
        else
        {
          moves->push_back(
              encode_move(black_pawn_location, black_pawn_attack_location, true, PAWN, NO_PIECE, true, false,
                          position->enPassantTarget & int_location_to_bitboard(black_pawn_attack_location), false));
        }
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

    to_square_candidates = knight_moves[knight_location] & black_occupied_no_king(position);

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
  uint64_t black_knights_in_position = black_knights(position);

  while (black_knights_in_position)
  {
    int knight_location = bitscan(black_knights_in_position);
    uint64_t to_square_candidates = knight_moves[knight_location] & (~all_occupied(position));
    int to_square;

    while (to_square_candidates)
    {
      to_square = bitscan(to_square_candidates);
      moves->push_back(encode_move(knight_location, to_square, false, KNIGHT, NO_PIECE, false, false, false, false));
      to_square_candidates = set_bit_low(to_square_candidates, to_square);
    }

    to_square_candidates = knight_moves[knight_location] & white_occupied_no_king(position);

    while (to_square_candidates)
    {
      to_square = bitscan(to_square_candidates);
      moves->push_back(encode_move(knight_location, to_square, false, KNIGHT, NO_PIECE, true, false, false, false));
      to_square_candidates = set_bit_low(to_square_candidates, to_square);
    }

    black_knights_in_position = set_bit_low(black_knights_in_position, knight_location);
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
        (all_occupied(position) & WHITE_KINGSIDE_CASTLE_MASK) == 0 &&
        (white_rooks(position) & int_location_to_bitboard(h1)))
    {
      moves->push_back(encode_move(e1, g1, true, KING, NO_PIECE, false, false, false, true));
    }
    if (position->white_can_castle_queenside && !is_square_attacked(position->white_to_move, c1, position) &&
        !is_square_attacked(position->white_to_move, d1, position) &&
        !is_square_attacked(position->white_to_move, e1, position) &&
        (all_occupied(position) & WHITE_QUEENSIDE_CASTLE_MASK) == 0 &&
        (white_rooks(position) & int_location_to_bitboard(a1)))
    {
      moves->push_back(encode_move(e1, c1, true, KING, NO_PIECE, false, false, false, true));
    }
  }
  else
  {
    if (position->black_can_castle_kingside && !is_square_attacked(position->white_to_move, e8, position) &&
        !is_square_attacked(position->white_to_move, f8, position) &&
        !is_square_attacked(position->white_to_move, g8, position) &&
        (all_occupied(position) & BLACK_KINGSIDE_CASTLE_MASK) == 0 &&
        (black_rooks(position) & int_location_to_bitboard(h8)))
    {
      moves->push_back(encode_move(e8, g8, false, KING, NO_PIECE, false, false, false, true));
    }
    if (position->black_can_castle_queenside && !is_square_attacked(position->white_to_move, c8, position) &&
        !is_square_attacked(position->white_to_move, d8, position) &&
        !is_square_attacked(position->white_to_move, e8, position) &&
        (all_occupied(position) & BLACK_QUEENSIDE_CASTLE_MASK) == 0 &&
        (black_rooks(position) & int_location_to_bitboard(a8)))
    {
      moves->push_back(encode_move(e8, c8, false, KING, NO_PIECE, false, false, false, true));
    }
  }
}
