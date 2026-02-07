#include "zobrist.hpp"

#include "../game/moves.hpp"

namespace zobrist
{

// Global Zobrist key tables
uint64_t piece_keys[2][6][64];
uint64_t castling_keys[16];
uint64_t ep_file_keys[8];
uint64_t side_key;

// Simple xorshift64 PRNG for generating random keys
// Using a fixed seed for reproducibility
static uint64_t rand_state = 0x98f107a3d5e8b2c1ULL;

static uint64_t xorshift64()
{
  rand_state ^= rand_state >> 12;
  rand_state ^= rand_state << 25;
  rand_state ^= rand_state >> 27;
  return rand_state * 0x2545F4914F6CDD1DULL;
}

void init()
{
  // Reset PRNG state for reproducible keys
  rand_state = 0x98f107a3d5e8b2c1ULL;

  // Generate piece keys for each color, piece type, and square
  for (int color = 0; color < 2; color++)
  {
    for (int piece = 0; piece < 6; piece++)
    {
      for (int square = 0; square < 64; square++)
      {
        piece_keys[color][piece][square] = xorshift64();
      }
    }
  }

  // Generate castling keys for all 16 combinations
  for (int i = 0; i < 16; i++)
  {
    castling_keys[i] = xorshift64();
  }

  // Generate en passant file keys
  for (int file = 0; file < 8; file++)
  {
    ep_file_keys[file] = xorshift64();
  }

  // Generate side to move key
  side_key = xorshift64();
}

uint64_t compute_hash(const Position& pos)
{
  uint64_t hash = 0;

  // Hash all pieces on the board
  uint64_t occupied = pos.white | pos.black;
  while (occupied)
  {
    int sq = bitscan(occupied);
    uint64_t sq_bb = 1ULL << sq;

    // Determine color (0 = black, 1 = white)
    int color = (pos.white & sq_bb) ? 1 : 0;

    // Determine piece type
    int piece_type;
    if (pos.pawns & sq_bb)
      piece_type = PAWN;
    else if (pos.knights & sq_bb)
      piece_type = KNIGHT;
    else if (pos.bishops & sq_bb)
      piece_type = BISHOP;
    else if (pos.rooks & sq_bb)
      piece_type = ROOK;
    else if (pos.queens & sq_bb)
      piece_type = QUEEN;
    else
      piece_type = KING;

    hash ^= piece_keys[color][piece_type][sq];

    occupied &= occupied - 1;  // Clear LSB
  }

  // Hash castling rights
  hash ^= castling_keys[castling_rights_index(pos)];

  // Hash en passant file if there's an en passant target
  if (pos.enPassantTarget)
  {
    int ep_file = file_of(bitscan(pos.enPassantTarget));
    hash ^= ep_file_keys[ep_file];
  }

  // Hash side to move (XOR if black to move)
  if (!pos.white_to_move)
  {
    hash ^= side_key;
  }

  return hash;
}

}  // namespace zobrist
