#include "search.hpp"

#include <algorithm>
#include <chrono>
#include <iostream>
#include <limits>
#include <ranges>
#include <vector>

#include "../evaluator/evaluator.hpp"
#include "../game/moves.hpp"

#define CHECKMATE_FOR_LAST_PLAYER -75000
#define THEIR_BEST_MOVE_START_VAL 100000
#define MY_BEST_MOVE_START_VAL -100000

// Limit quiesnce depth for now
#define MAX_QUIESCENCE_DEPTH 3

// clang-format off
// Attacker K->B->R->Q->K->P (left to right)
// Victim -> same (top to bottom)
const std::array<std::array<int, 6>, 6> mvv_lva_table = {{
  {24, 34, 44, 54, 64, 14,},
	{23, 33, 43, 53, 63, 13,},
	{22, 32, 42, 52, 62, 12,},
	{21, 31, 41, 51, 61, 11,},
	{20, 30, 40, 50, 60, 10,},
	{25, 35, 45, 55, 65, 15,},
}};
// clang-format on

bool compare_mvv_lva_move_value(Position* position, uint32_t move1, uint32_t move2)
{
  PieceAsInt attacker_piece_move1 = decode_moved_piece(move1);
  PieceAsInt victim_piece_move1 = victim_on_square(position, static_cast<Square>(decode_to_square(move1)));
  PieceAsInt attacker_piece_move2 = decode_moved_piece(move2);
  PieceAsInt victim_piece_move2 = victim_on_square(position, static_cast<Square>(decode_to_square(move2)));

  return mvv_lva_table[attacker_piece_move1][victim_piece_move1] >
         mvv_lva_table[attacker_piece_move2][victim_piece_move2];
}

std::vector<uint32_t> inline mvv_lva_ordered_moves(Position* position)
{
  std::vector<uint32_t> moves = valid_moves_for_position(*position);
  std::sort(moves.begin(), moves.end(),
            [position](uint32_t move1, uint32_t move2) { return compare_mvv_lva_move_value(position, move1, move2); });
  return moves;
}

// // limit this to depth 3 for now
// int_fast32_t inline quiescence_search(Position* position, int depth, int_fast32_t my_best_move_score,
//                                       int_fast32_t their_best_move_score, uint64_t* nodes)
// {
//   if (depth == 0)
//   {
//     return evaluate_position(position);
//   }

//   int_fast32_t position_score = evaluate_position(position);
//   *nodes += 1;

//   if (position_score >= their_best_move_score)
//   {
//     return their_best_move_score;
//   }

//   if (position_score > my_best_move_score)
//   {
//     my_best_move_score = position_score;
//   }

//   std::vector<uint32_t> possible_moves = valid_moves_for_position(*position);
//   auto last_capture_pointer = std::remove_if(possible_moves.begin(), possible_moves.end(),
//                                              [](uint32_t move) { return decode_capture(move) == 0; });
//   possible_moves.erase(last_capture_pointer, possible_moves.end());

//   int_fast32_t score = MY_BEST_MOVE_START_VAL;

//   for (uint32_t move : possible_moves)
//   {
//     Position new_position = make_move(position, move);
//     int_fast32_t current_score =
//         -quiescence_search(&new_position, depth - 1, -their_best_move_score, -my_best_move_score, nodes);

//     score = std::max(current_score, score);
//     my_best_move_score = std::max(score, my_best_move_score);

//     if (my_best_move_score >= their_best_move_score)
//     {
//       return my_best_move_score;
//     }
//   }

//   return my_best_move_score;
// }

uint32_t find_move_with_statistics(Position* position, int depth)
{
  uint64_t nodes = 0;
  auto start_time = std::chrono::high_resolution_clock::now();
  uint32_t best_move = find_move(position, depth, &nodes);
  auto end_time = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
  std::cout << "Time taken: " << duration.count() << "ms" << std::endl;
  std::cout << "Nodes: " << nodes << std::endl;
  // std::cout << "Nodes per second: " << nodes / (duration.count() / 1000) << std::endl;

  return best_move;
}

uint32_t find_move(Position* position, int depth, uint64_t* nodes)
{
  uint32_t best_move;
  int_fast32_t best_score = MY_BEST_MOVE_START_VAL;
  int_fast32_t current_score;
  std::vector<uint32_t> moves = mvv_lva_ordered_moves(position);
  // std::vector<uint32_t> moves = valid_moves_for_position(*position);

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

int_fast32_t negamax(Position* position, int depth, int_fast32_t my_best_move_score, int_fast32_t their_best_move_score,
                     uint64_t* nodes)
{
  if (depth == 0)
  {
    *nodes += 1;
    // return evaluate_position(position);
    // return quiescence_search(position, MAX_QUIESCENCE_DEPTH, my_best_move_score, their_best_move_score, nodes);
    return evaluate_position(position);
  }

  int_fast32_t score = MY_BEST_MOVE_START_VAL;
  std::vector<uint32_t> possible_moves = mvv_lva_ordered_moves(position);
  // std::vector<uint32_t> possible_moves = valid_moves_for_position(*position);

  if (possible_moves.empty())
  {
    return CHECKMATE_FOR_LAST_PLAYER;
  }

  for (uint32_t move : possible_moves)
  {
    Position new_position = make_move(position, move);
    int_fast32_t current_score = -negamax(&new_position, depth - 1, -their_best_move_score, -my_best_move_score, nodes);

    score = std::max(current_score, score);
    my_best_move_score = std::max(score, my_best_move_score);

    if (my_best_move_score >= their_best_move_score)
    {
      return my_best_move_score;
    }
  }

  return score;
}
