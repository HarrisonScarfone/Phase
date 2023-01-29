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
  // display_rook_magic_numbers();
  // DOUBLE_SPACE
  // display_bishop_magic_numbers();
  // DOUBLE_SPACE

  uint64_t blocker_board =
      square_to_bitboard(e6) | square_to_bitboard(g4) | square_to_bitboard(a4) | square_to_bitboard(c4);
  Util::print_bitboard(blocker_board);
  DOUBLE_SPACE
  Util::print_bitboard(magic_rook_table[e4].mask);
  DOUBLE_SPACE
  Util::print_bitboard(rook_attacks(blocker_board, e4));
  DOUBLE_SPACE
  Util::print_bitboard(rook_attack_mask_for_bitboard(square_to_bitboard(e4), blocker_board));
  DOUBLE_SPACE
  Util::print_bitboard(knight_moves[e4]);
  DOUBLE_SPACE
  Util::print_bitboard(king_moves[b6]);
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
