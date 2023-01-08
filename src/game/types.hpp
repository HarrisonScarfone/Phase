#ifndef TYPES_H
#define TYPES_H

#include <cstdint>

namespace Game
{
  struct Position
  {
    uint64_t black = 0;
    uint64_t white = 0;
    uint64_t kings = 0;
    uint64_t queens = 0;
    uint64_t rooks = 0;
    uint64_t bishops = 0;
    uint64_t knights = 0;
    uint64_t pawns = 0;

    bool white_to_move = false;

    bool white_can_castle_kingside = false;
    bool white_can_castle_queenside = false;
    bool black_can_castle_kingside = false;
    bool black_can_castle_queenside = false;

    uint64_t enPassantTarget = 0;

    short int half_move_clock = 0;
    short int full_move_clock = 0;
  };

  struct Move
  {
    /* data */
  };
}

#endif