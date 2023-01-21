#ifndef UTIL_H
#define UTIL_H

#include <cstdint>
#include <string>
#include <vector>

#include "global.hpp"

namespace Util
{
int bitscan(uint64_t square);
std::vector<std::string> tokenize_string_by_whitespace(std::string);

char get_piece_as_char_from_square(Position *position, int square);

void cli_display_position(Position *position);
void print_bitboard(uint64_t bitboard);
}  // namespace Util

#endif