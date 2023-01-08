#ifndef UTIL_H
#define UTIL_H

#include <string>
#include <vector>

#include "../game/types.hpp"

namespace Util
{
  std::vector<std::string> tokenize_string_by_whitespace(std::string);

  uint64_t coordinate_to_uint64_t(std::string coordinate);
  std::string uint64_t_to_coordinate(uint64_t square);

  char get_piece_as_char_from_square(Game::Position *position, int square);

  void display_64_bit_int_in_binary(uint64_t number);
  void cli_display_position(Game::Position *position);
}

#endif