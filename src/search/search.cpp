#include "search.hpp"

#include <vector>

#include "../evaluator/evaluator.hpp"
#include "../game/moves.hpp"

int negamax(Position position, int depth)
{
  if (depth == 0)
  {
    return evaluate_position(&position);
  }

  int_fast32_t max = INT_FAST32_MIN;
  std::vector<uint32_t> moves = valid_moves_for_position(position);

  for (uint32_t move : moves)
  {
    Position new_position = make_move(&position, move);
    int score = -1 * negamax(new_position, depth - 1);

    if (score > max)
    {
      max = score;
    }
  }

  return max;
}

uint32_t find_move(Position* position, int depth)
{
  uint32_t best_move;
  int_fast32_t best_score = INT_FAST32_MIN;
  int_fast32_t current_score;
  std::vector<uint32_t> moves = valid_moves_for_position(*position);

  for (uint32_t move : moves)
  {
    Position new_position = make_move(position, move);
    current_score = negamax(new_position, depth - 1);

    if (current_score > best_score)
    {
      best_move = move;
      best_score = current_score;
    }
  }

  return best_move;
}
