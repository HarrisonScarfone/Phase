#ifndef SEARCH_H
#define SEARCH_H

#include <atomic>
#include <chrono>
#include <cstdint>
#include <thread>
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

// Search context for game history and draw detection
struct SearchContext
{
  std::vector<uint64_t> game_history;  // Position hashes from game start
  int root_ply = 0;                    // Ply count from root for mate distance
  int root_score = 0;                  // Score at root for draw contempt

  void clear()
  {
    game_history.clear();
    root_score = 0;
  }
};

// Global search context
extern SearchContext search_context;

// Set the search time limit in milliseconds
void set_search_time_limit(int time_ms);

// Set number of search threads
void set_thread_count(int threads);
int get_thread_count();

// Stop the current search
void stop_search();

// Main search functions
find_move_return_val find_move(Position* position);
find_move_return_val find_move_with_history(Position* position, const std::vector<uint64_t>& history);
uint32_t find_move_and_display_statistics(Position* position);

// Internal search function
int_fast32_t principal_variation_search(Position* position, int depth, int ply, int_fast32_t alpha, int_fast32_t beta,
                                        uint64_t* nodes);

#endif