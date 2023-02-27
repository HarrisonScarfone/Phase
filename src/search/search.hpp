#ifndef SEARCH_H
#define SEARCH_H

#include "../util/global.hpp"

// int minimax(Position* position, int depth, int_fast32_t alpha, int_fast32_t beta);
int_fast32_t negamax(Position* position, int depth, int_fast32_t alpha, int_fast32_t beta);
uint32_t find_move(Position* position, int depth);

#endif