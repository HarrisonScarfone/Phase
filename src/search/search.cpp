#include "search.hpp"

#include <algorithm>
#include <limits>
#include <vector>

#include "../evaluator/evaluator.hpp"
#include "../game/moves.hpp"

uint32_t find_move(Position* position, int depth)
{
  uint32_t best_move;
  int_fast32_t best_score = INT_FAST32_MIN;
  int_fast32_t current_score;
  std::vector<uint32_t> moves = valid_moves_for_position(*position);

  int_fast32_t alpha = -100000;
  int_fast32_t beta = 100000;

  for (uint32_t move : moves)
  {
    Position new_position = make_move(position, move);
    current_score = -negamax(&new_position, depth - 1, -beta, -alpha);

    if (current_score > best_score)
    {
      best_move = move;
      best_score = current_score;
    }
  }

  return best_move;
}

int_fast32_t negamax(Position* position, int depth, int_fast32_t alpha, int_fast32_t beta)
{
  if (depth == 0)
  {
    return evaluate_position(position);
  }

  int_fast32_t score = -100000;
  std::vector<uint32_t> possible_moves = valid_moves_for_position(*position);

  if (possible_moves.empty())
  {
    return -75000;
  }

  for (uint32_t move : possible_moves)
  {
    Position new_position = make_move(position, move);
    int_fast32_t current_score = -negamax(&new_position, depth - 1, -beta, -alpha);

    if (abs(current_score) == 75000)
    {
      current_score *= (decode_check(move) > 0 ? -1 : 1);
    }

    score = std::max(current_score, score);
    alpha = std::max(score, alpha);

    if (alpha >= beta)
    {
      break;
    }
  }

  return score;
}
