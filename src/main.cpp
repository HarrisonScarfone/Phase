#include <math.h>

#include <array>
#include <iostream>

#include "util/initializers.hpp"
#include "util/magicbitboards.hpp"
#include "util/tables.hpp"
#include "util/util.hpp"

int main(int argc, char *argv[])
{
  // Position position = Util::Initializers::starting_position();
  // Util::cli_display_position(&position);
  // Util::print_bitboard(static_cast<uint64_t>(2048));
  // for (int i = 0; i < 65; i++)
  // {
  //   Util::print_bitboard(pawn_attacks[0][i]);
  // }
  // Util::print_bitboard(pawn_attacks[0][1]);
  // Util::print_bitboard(pawn_attacks[0][25]);
  // Util::print_bitboard(pawn_attacks[0][27]);
  // Util::print_bitboard(pawn_attacks[0][39]);
  // return 0;
  // uint64_t t = 1;
  // uint64_t a = ~0;
  // for (int i = 0; i < 100; i++)
  // {
  //   // std::cout << i << ":   " << rook_attack_masks[i] << std::endl;
  //   // Util::print_bitboard(rook_attack_masks[i]);
  //   // std::cout << std::endl << std::endl << std::endl;
  //   std::cout << t << std::endl;
  //   std::cout << a << std::endl;
  //   std::cout << std::endl;

  //   t <<= 1;
  //   a >>= 1;
  // }
  Util::print_bitboard(RandGen::generate_random_32_bit_number());
  Util::print_bitboard(RandGen::generate_random_32_bit_number());
  Util::print_bitboard(RandGen::generate_random_64_bit_number());
}

// 8 - - - - - - - -
// 7 - - - - - - - -
// 6 - - - - - - - X
// 5 - - - - - - - -
// 4 - - - - - - - -
// 3 - - - - - - - -
// 2 - - - - - - - -
// 1 - - - - - - - -
//   a b c d e f g h