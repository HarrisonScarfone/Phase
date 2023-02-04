#include <math.h>

#include <array>
#include <iostream>

#include "util/initializers.hpp"
#include "util/magicbitboards.hpp"
#include "util/util.hpp"

#define DOUBLE_SPACE std::cout << std::endl << std::endl;

int main(int argc, char *argv[])
{
  // Position position = Util::Initializers::starting_position();
  // Util::cli_display_position(&position);

  uint64_t blocker_board = square_to_bitboard(e6) | square_to_bitboard(g6) | square_to_bitboard(a4) |
                           square_to_bitboard(c4) | square_to_bitboard(d3);
  Util::print_bitboard(blocker_board);
  DOUBLE_SPACE
  Util::print_bitboard(queen_attacks(blocker_board, e4));
}

// // 8 - - - - - - - -
// // 7 - - - - - - - -
// // 6 - - - - - - - X
// // 5 - - - - - - - -
// // 4 - - - - - - - -
// // 3 - - - - - - - -
// // 2 - - - - - - - -
// // 1 - - - - - - - -
// //   a b c d e f g h
