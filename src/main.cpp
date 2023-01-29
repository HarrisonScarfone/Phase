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
  display_rook_magic_numbers();
  DOUBLE_SPACE
  display_bishop_magic_numbers();
  DOUBLE_SPACE

  // uint64_t attacks[4096] = {0ull};
  // uint64_t occupancies[4096] = {0ull};
  // uint64_t bitboard = 1ull;

  // int square = 0;

  // Util::print_bitboard(rook_full_occupancy[square]);
  // std::cout << std::endl << std::endl;

  // for (int i = 0; i < 100; i++)
  // {
  //   occupancies[i] = construct_occupancies(i, rook_occupancy[square], rook_full_occupancy[square]);
  //   attacks[i] = rook_attack_mask_for_bitboard(bitboard, occupancies[i]);

  //   std::cout << "Attacks at: " << i << std::endl << std::endl;
  //   Util::print_bitboard(attacks[i]);
  //   std::cout << std::endl << "Occupancy at: " << i << std::endl << std::endl;
  //   Util::print_bitboard(occupancies[i]);
  //   std::cout << std::endl << std::endl << std::endl;
  // }
  // uint64_t blocker_board = square_to_bitboard(e6) | square_to_bitboard(g4) | square_to_bitboard(a4);
  // Util::print_bitboard(blocker_board);
  // DOUBLE_SPACE
  // Util::print_bitboard(magic_rook_table[e4].mask);
  // DOUBLE_SPACE
  // Util::print_bitboard(rook_attacks(blocker_board, e4));
  // DOUBLE_SPACE
  // Util::print_bitboard(rook_attack_mask_for_bitboard(square_to_bitboard(e4), blocker_board));

  // rand count 943805
  // rel bits 12
  // magic as dec 9979994641325359136
  Util::print_bitboard(282578800148862ull);
  Util::print_bitboard(72340172838076926ull);

  // for (int i = 0; i < 50; i++)
  // {
  //   Util::print_bitboard(magic_rook_attacks[0][i]);
  //   DOUBLE_SPACE;
  // }
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
