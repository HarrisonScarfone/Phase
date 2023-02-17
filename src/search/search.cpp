#include "search.hpp"

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

  for (uint32_t move : moves)
  {
    Position new_position = make_move(position, move);
    current_score = alpha_beta_max(&new_position, -10000000, 10000000, depth - 1);

    if (current_score > best_score)
    {
      best_move = move;
      best_score = current_score;
    }
  }

  return best_move;
}

int alpha_beta_max(Position* position, int alpha, int beta, int depth)
{
  if (depth == 0)
  {
    return evaluate_position(position);
  }

  std::vector<uint32_t> moves = valid_moves_for_position(*position);

  for (uint32_t move : moves)
  {
    Position new_position = make_move(position, move);
    int score = alpha_beta_min(&new_position, alpha, beta, depth - 1);

    if (score >= beta)
    {
      return beta;
    }
    if (score > alpha)
    {
      alpha = score;
    }
  }

  return alpha;
}

int alpha_beta_min(Position* position, int alpha, int beta, int depth)
{
  if (depth == 0)
  {
    return -1 * evaluate_position(position);
  }

  std::vector<uint32_t> moves = valid_moves_for_position(*position);

  for (uint32_t move : moves)
  {
    Position new_position = make_move(position, move);
    int score = alpha_beta_max(&new_position, alpha, beta, depth - 1);

    if (score <= alpha)
    {
      return alpha;
    }
    if (score <= beta)
    {
      beta = score;
    }
  }

  return beta;
}
