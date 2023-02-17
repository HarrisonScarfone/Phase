#ifndef SEARCH_H
#define SEARCH_H

#include "../util/global.hpp"

int alpha_beta_min(Position* position, int alpha, int beta, int depth);
int alpha_beta_max(Position* position, int alpha, int beta, int depth);
uint32_t find_move(Position* position, int depth);

#endif