#ifndef MOVES_H
#define MOVES_H

#include <cstdint>

#include "../util/global.hpp"
#include "../util/magicbitboards.hpp"

bool is_square_attacked(bool white, int square, Position position);
uint64_t attacked_squares(bool white, Position position);

#endif