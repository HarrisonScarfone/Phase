#include "search.hpp"

#include <algorithm>
#include <chrono>
#include <iostream>
#include <limits>
#include <vector>

#include "../evaluator/evaluator.hpp"
#include "../game/moves.hpp"

#define CHECKMATE_FOR_LAST_PLAYER -75000
#define THEIR_BEST_MOVE_START_VAL 100000
#define MY_BEST_MOVE_START_VAL -100000

uint32_t find_move_with_statistics(Position* position, int depth)
{
  uint64_t nodes = 0;
  auto start_time = std::chrono::high_resolution_clock::now();
  uint32_t best_move = find_move(position, depth, &nodes);
  auto end_time = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
  std::cout << "Time taken: " << duration.count() << "ms" << std::endl;
  std::cout << "Nodes: " << nodes << std::endl;
  std::cout << "Nodes per second: " << nodes / (duration.count() / 1000) << std::endl;

  return best_move;
}

uint32_t find_move(Position* position, int depth, uint64_t* nodes)
{
  uint32_t best_move;
  int_fast32_t best_score = MY_BEST_MOVE_START_VAL;
  int_fast32_t current_score;
  std::vector<uint32_t> moves = valid_moves_for_position(*position);

  for (uint32_t move : moves)
  {
    Position new_position = make_move(position, move);
    current_score = -negamax(&new_position, depth - 1, -THEIR_BEST_MOVE_START_VAL, -MY_BEST_MOVE_START_VAL, nodes);
    *nodes += 1;

    if (current_score > best_score)
    {
      best_move = move;
      best_score = current_score;
    }
  }

  return best_move;
}

int_fast32_t negamax(Position* position, int depth, int_fast32_t my_best_move, int_fast32_t their_best_move,
                     uint64_t* nodes)
{
  if (depth == 0)
  {
    *nodes += 1;
    return evaluate_position(position);
  }

  int_fast32_t score = MY_BEST_MOVE_START_VAL;
  std::vector<uint32_t> possible_moves = valid_moves_for_position(*position);

  if (possible_moves.empty())
  {
    return CHECKMATE_FOR_LAST_PLAYER;
  }

  for (uint32_t move : possible_moves)
  {
    Position new_position = make_move(position, move);
    int_fast32_t current_score = -negamax(&new_position, depth - 1, -their_best_move, -my_best_move, nodes);

    score = std::max(current_score, score);
    my_best_move = std::max(score, my_best_move);

    if (my_best_move >= their_best_move)
    {
      break;
    }
  }

  return score;
}

// uint32_t find_move(Position* position, int depth)
// {
//   uint32_t best_move;
//   int_fast32_t best_score = MY_BEST_MOVE_START_VAL;
//   int_fast32_t current_score;
//   int_fast32_t look_ahead_score;
//   std::vector<uint32_t> moves = valid_moves_for_position(*position);

//   std::vector<std::tuple<uint32_t, int_fast32_t, Position>> scored_moves;
//   for (uint32_t move : moves)
//   {
//     Position new_position = make_move(position, move);
//     look_ahead_score = evaluate_position(&new_position);
//     scored_moves.emplace_back(move, look_ahead_score, new_position);
//   }

//   std::sort(scored_moves.begin(), scored_moves.end(), [](auto& a, auto& b) { return std::get<1>(a) > std::get<1>(b);
//   });

//   for (std::tuple<uint32_t, int_fast32_t, Position> scored_move : scored_moves)
//   {
//     current_score = -negamax(&std::get<2>(scored_move), depth - 1, -THEIR_BEST_MOVE_START_VAL,
//     -MY_BEST_MOVE_START_VAL);

//     if (current_score > best_score)
//     {
//       best_move = std::get<0>(scored_move);
//       best_score = current_score;
//     }
//   }

//   return best_move;
// }

// int_fast32_t negamax(Position* position, int depth, int_fast32_t my_best_move, int_fast32_t their_best_move)
// {
//   if (depth == 0)
//   {
//     return evaluate_position(position);
//   }

//   int_fast32_t score = MY_BEST_MOVE_START_VAL;
//   int_fast32_t look_ahead_score;
//   std::vector<uint32_t> possible_moves = valid_moves_for_position(*position);

//   if (possible_moves.empty())
//   {
//     return CHECKMATE_FOR_LAST_PLAYER;
//   }

//   std::vector<std::pair<int_fast32_t, Position>> scored_moves;
//   for (uint32_t move : possible_moves)
//   {
//     Position new_position = make_move(position, move);
//     look_ahead_score = evaluate_position(&new_position);
//     scored_moves.emplace_back(look_ahead_score, new_position);
//   }

//   std::sort(scored_moves.begin(), scored_moves.end(), [](auto& a, auto& b) { return std::get<0>(a) > std::get<0>(b);
//   });

//   for (std::pair<int_fast32_t, Position> scored_move : scored_moves)
//   {
//     int_fast32_t current_score = -negamax(&std::get<1>(scored_move), depth - 1, -their_best_move, -my_best_move);

//     score = std::max(current_score, score);
//     my_best_move = std::max(score, my_best_move);

//     if (my_best_move >= their_best_move)
//     {
//       break;
//     }
//   }

//   return score;
// }
