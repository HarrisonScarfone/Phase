#include "search.hpp"

#include <algorithm>
#include <limits>
#include <vector>

#include "../evaluator/evaluator.hpp"
#include "../game/moves.hpp"

uint32_t find_move(Position* position, int depth)
{
  uint32_t best_move;
  int_fast32_t best_score = -1000000000;
  int_fast32_t current_score;
  std::vector<uint32_t> moves = valid_moves_for_position(*position);

  int alpha = 10000000 * (position->white_to_move ? -1 : 1);
  int beta = 10000000 * (position->white_to_move ? 1 : -1);

  for (uint32_t move : moves)
  {
    Position new_position = make_move(position, move);
    current_score = minimax(&new_position, depth - 1, alpha, beta);

    if (current_score > best_score)
    {
      best_move = move;
      best_score = current_score;
    }
  }

  return best_move;
}

int minimax(Position* position, int depth, int alpha, int beta)
{
  if (depth == 0) return evaluate_position(position);

  std::vector<uint32_t> possible_moves = valid_moves_for_position(*position);

  if (position->white_to_move)
  {
    int max_score = -10000000;
    for (uint32_t move : possible_moves)
    {
      Position new_position = make_move(position, move);
      int score = minimax(&new_position, depth - 1, alpha, beta);
      max_score = std::max(max_score, score);
      alpha = std::max(alpha, score);
      if (beta <= alpha) break;
    }
    return max_score;
  }
  else
  {
    int min_score = 10000000;
    for (uint32_t move : possible_moves)
    {
      Position new_position = make_move(position, move);
      int score = minimax(&new_position, depth - 1, alpha, beta);
      min_score = std::min(min_score, score);
      beta = std::min(beta, score);
      if (beta <= alpha) break;
    }
    return min_score;
  }
}

// int alpha_beta_max(Position* position, int alpha, int beta, int depth)
// {
//   if (depth == 0)
//   {
//     return evaluate_position(position);
//   }

//   std::vector<uint32_t> moves = valid_moves_for_position(*position);
//   int best_score = -10000000;

//   for (uint32_t move : moves)
//   {
//     Position new_position = make_move(position, move);
//     int score = alpha_beta_min(&new_position, alpha, beta, depth - 1);

//     if (score >= beta)
//     {
//       return score;
//     }
//     if (score > best_score)
//     {
//       best_score = score;
//     }
//     if (score > alpha)
//     {
//       alpha = score;
//     }
//   }

//   return best_score;
// }

// int alpha_beta_min(Position* position, int alpha, int beta, int depth)
// {
//   if (depth == 0)
//   {
//     return evaluate_position(position);
//   }

//   std::vector<uint32_t> moves = valid_moves_for_position(*position);
//   int best_score = -10000000;

//   for (uint32_t move : moves)
//   {
//     Position new_position = make_move(position, move);
//     int score = alpha_beta_max(&new_position, alpha, beta, depth - 1);

//     if (score <= alpha)
//     {
//       return score;
//     }
//     if (score < best_score)
//     {
//       best_score = score;
//     }
//     if (score < beta)
//     {
//       beta = score;
//     }
//   }

//   return best_score;
// }
