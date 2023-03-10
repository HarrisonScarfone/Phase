#ifndef UTIL_H
#define UTIL_H

#include <cstdint>
#include <string>
#include <vector>

#include "global.hpp"

namespace Util
{
std::vector<std::string> tokenize_string_by_whitespace(std::string);

std::string get_piece_as_string_from_square(Position *position, int square);

void cli_display_position(Position *position);
void print_bitboard(uint64_t bitboard);
void display_encoded_move(uint32_t move);
}  // namespace Util

#endif