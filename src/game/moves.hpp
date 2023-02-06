#ifndef MOVES_H
#define MOVES_H

#include <array>
#include <cstdint>
#include <map>
#include <vector>

#include "../util/global.hpp"
#include "../util/magicbitboards.hpp"

/*

Definition of a move
using a uinsigned 32 bit number

0000 0000 0000 0000 0000 0000 0011 1111   |   from square
0000 0000 0000 0000 0000 1111 1100 0000   |   to square
0000 0000 0000 0000 0001 0000 0000 0000   |   whites turn
0000 0000 0000 0000 1110 0000 0000 0000   |   moved peice
0000 0000 0000 0111 0000 0000 0000 0000   |   promoted to peice
0000 0000 0000 1000 0000 0000 0000 0000   |   capture
0000 0000 0001 0000 0000 0000 0000 0000   |   double push
0000 0000 0010 0000 0000 0000 0000 0000   |   enpassant
0000 0000 0100 0000 0000 0000 0000 0000   |   castling

*/

#define DONT_CARE 0

enum PieceAsInt
{
  KNIGHT,
  BISHOP,
  ROOK,
  QUEEN,
  PAWN,
  KING,
  NO_PIECE,
};

enum MoveMasksIndices
{
  FROM_SQUARE,
  TO_SQUARE,
  WHITES_TURN,
  MOVED_PIECE,
  PROMOTED_TO_PIECE,
  CAPTURE,
  DOUBLE_PUSH,
  ENPASSANT,
  CASTLING,
};

constexpr std::array<std::string_view, 9> move_mask_names = {
    "FROM_SQUARE", "TO_SQUARE",   "WHITES_TURN", "MOVED_PIECE", "PROMOTED_TO_PIECE",
    "CAPTURE",     "DOUBLE_PUSH", "ENPASSANT",   "CASTLING",
};

constexpr std::array<std::string_view, 7> piece_names = {
    "KNIGHT", "BISHOP", "ROOK", "QUEEN", "PAWN", "KING", "NO_PIECE",
};

constexpr std::array<uint32_t, 9> move_masks = {
    0x0000003F,  // from square
    0x00000FC0,  // to square
    0x00001000,  // whites turn
    0x0000E000,  // moved piece
    0x00070000,  // promoted to piece
    0x00080000,  // capture
    0x00100000,  // double push
    0x00200000,  // enpassant
    0x00400000,  // castling
};

inline int decode_from_square(uint32_t move) { return static_cast<int>(move & move_masks[0]); }
inline int decode_to_square(uint32_t move) { return static_cast<int>((move & move_masks[1]) >> 6); }
inline bool decode_whites_turn(uint32_t move) { return move & move_masks[2]; }
inline int decode_moved_piece(uint32_t move) { return static_cast<int>((move & move_masks[3]) >> 13); }
inline int decode_promoted_to_piece(uint32_t move) { return static_cast<int>((move & move_masks[4]) >> 16); }
inline bool decode_capture(uint32_t move) { return move & move_masks[5]; }
inline bool decode_double_push(uint32_t move) { return move & move_masks[6]; }
inline bool decode_enpassant(uint32_t move) { return move & move_masks[7]; }
inline bool decode_castling(uint32_t move) { return move & move_masks[8]; }

bool is_square_attacked(bool white, int square, Position position);
uint64_t attacked_squares(bool white, Position position);
uint32_t encode_move(int from_square, int to_square, bool whites_turn, int moved_peice, int promoted_to_piece,
                     bool capture, bool double_push, int enpassant, int castling);

std::vector<uint32_t> white_quiet_pawn_moves(Position position);
std::vector<uint32_t> pesudolegal_possible_moves(Position position);

#endif