#include <math.h>

#include <array>
#include <iostream>

#include "game/moves.hpp"
#include "util/initializers.hpp"
#include "util/magicbitboards.hpp"
#include "util/util.hpp"

#define DOUBLE_SPACE std::cout << std::endl << std::endl;

int main(int argc, char *argv[])
{
  // Position position = Util::Initializers::starting_position();
  std::string fen_position = "rnbqkbn1/pppppppP/ppp5/8/8/ppp1P3/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
  // std::string fen_position = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1";
  Position position = Util::Initializers::fen_string_to_position(fen_position);
  Util::cli_display_position(&position);

  DOUBLE_SPACE

  std::vector<uint32_t> moves = white_quiet_pawn_moves(position);

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
