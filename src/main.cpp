#include <array>
#include <iostream>

#include "util/initializers.hpp"
#include "util/util.hpp"

int main(int argc, char *argv[])
{
  Game::Position position = Util::Initializers::starting_position();
  Util::cli_display_position(&position);
  Util::print_bitboard(static_cast<uint64_t>(2048));
  return 0;
}

// #include <iostream>
// #include "util/lookup.hpp"
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