#ifndef SEARCH_H
#define SEARCH_H

#include "../util/global.hpp"

int minimax(Position* position, int alpha, int beta, int depth);
uint32_t find_move(Position* position, int depth);

#endif