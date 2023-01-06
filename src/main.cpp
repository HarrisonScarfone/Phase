#include "util/initializers.hpp"
#include "util/util.hpp"

#include <iostream>

int main(int argc, char *argv[])
{
  Game::Position position = Util::Initializers::fen_string_to_position("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
  Util::cli_display_position(&position);
  Util::display_64_bit_int_in_binary(position.kings);
  return 0;
}
