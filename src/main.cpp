#include <math.h>

#include <array>
#include <chrono>
#include <iostream>

#include "game/moves.hpp"
#include "util/initializers.hpp"
#include "util/magicbitboards.hpp"
#include "util/util.hpp"

#define DOUBLE_SPACE std::cout << std::endl << std::endl;

int main(int argc, char *argv[])
{
  // Position position = Util::Initializers::starting_position();
  std::string fen_position = "rnbq1bn1/pppppppp/4q3/3B4/4k3/3q4/PPPPPPPP/RNBQ1BNR w KQkq - 0 1";
  // std::string fen_position = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1";
  Position position = Util::Initializers::fen_string_to_position(fen_position);
  Util::cli_display_position(&position);

  DOUBLE_SPACE

  auto start = std::chrono::high_resolution_clock::now();
  std::vector<uint32_t> moves = pesudolegal_possible_moves(position);
  auto stop = std::chrono::high_resolution_clock::now();
  auto duration = duration_cast<std::chrono::nanoseconds>(stop - start);

  std::cout << std::endl << "Generation took: " << duration.count();

  DOUBLE_SPACE;

  for (uint32_t move : moves)
  {
    Util::display_encoded_move(move);
  }
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
