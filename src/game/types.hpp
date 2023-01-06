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

    bool white_to_move;

    bool white_can_castle_kingside;
    bool white_can_castle_queenside;
    bool black_can_castle_kingside;
    bool black_can_castle_queenside;

    uint64_t enPassantTarget;

    short int half_move_clock;
    short int full_move_clock;
  };

  struct Move
  {
    /* data */
  };
}

#endif