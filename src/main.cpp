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

  Util::print_bitboard(pawn_attacks[0][14]);
  Util::print_bitboard(1ULL << 23);
  Util::print_bitboard(1ULL << 21);
  Util::print_bitboard(1ULL << 14);

  return 0;
}

// #include <iostream>
// #include <array>
// #include "util/util.hpp"

// int main()
// {
//   uint64_t test = 1;
//   test <<= 31;
//   std::cout << lut_bitboard_to_coordinate[Util::bitscan(test)];
//   std::cout << lut_bitboard_to_coordinate[Util::bitscan(test)];
//   return 0;
// }
