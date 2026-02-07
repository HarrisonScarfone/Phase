#ifndef ZOBRIST_H
#define ZOBRIST_H

#include <cstdint>

#include "global.hpp"

namespace zobrist
{

// Zobrist key tables
// piece_keys[color][piece_type][square] - color: 0=black, 1=white
// piece_type uses PieceAsInt enum: KNIGHT=0, BISHOP=1, ROOK=2, QUEEN=3, PAWN=4, KING=5
extern uint64_t piece_keys[2][6][64];
extern uint64_t castling_keys[16];  // 4-bit index for all castling combinations
extern uint64_t ep_file_keys[8];    // en passant file (0-7)
extern uint64_t side_key;           // XOR when black to move

// Initialize all Zobrist keys (call once at startup)
void init();

// Compute hash from scratch for a position
uint64_t compute_hash(const Position& pos);

// Get castling rights as a 4-bit index
// Bit 0: white kingside, Bit 1: white queenside, Bit 2: black kingside, Bit 3: black queenside
inline int castling_rights_index(const Position& pos)
{
  return (pos.white_can_castle_kingside ? 1 : 0) | (pos.white_can_castle_queenside ? 2 : 0) |
         (pos.black_can_castle_kingside ? 4 : 0) | (pos.black_can_castle_queenside ? 8 : 0);
}

}  // namespace zobrist

#endif
