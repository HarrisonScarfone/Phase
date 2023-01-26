#include <array>
#include <iostream>

#include "util/initializers.hpp"
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
  Util::print_bitboard(pawn_attacks[0][1]);
  Util::print_bitboard(pawn_attacks[0][25]);
  Util::print_bitboard(pawn_attacks[0][27]);
  Util::print_bitboard(pawn_attacks[0][39]);
  // return 0;
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