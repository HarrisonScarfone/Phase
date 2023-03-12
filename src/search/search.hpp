#ifndef SEARCH_H
#define SEARCH_H

#include <chrono>
#include <vector>

#include "../util/global.hpp"

struct find_move_return_val
{
  uint32_t best_move;
  uint64_t nodes;
  std::chrono::milliseconds miliseconds_of_search_time;
  int depth;
  int_fast32_t position_score;
  std::vector<uint32_t> principal_variation;
};

int minimax(Position* position, int depth, int_fast32_t alpha, int_fast32_t beta);
int_fast32_t principal_variation_search(Position* position, int depth, int_fast32_t alpha, int_fast32_t beta,
                                        uint64_t* nodes);
find_move_return_val find_move(Position* position);
uint32_t find_move_and_display_statistics(Position* position);

#endif