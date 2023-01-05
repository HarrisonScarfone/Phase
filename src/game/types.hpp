#ifndef TYPES_H
#define TYPES_H

#include <cstdint>

namespace Game
{
  struct Position
  {
    uint64_t black;
    uint64_t white;
    uint64_t kings;
    uint64_t queens;
    uint64_t rooks;
    uint64_t bishops;
    uint64_t knights;
    uint64_t pawns;

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