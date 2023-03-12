#include "search.hpp"

#include <algorithm>
#include <iostream>
#include <limits>
#include <ranges>

#include "../evaluator/evaluator.hpp"
#include "../game/moves.hpp"

#define CHECKMATE_FOR_LAST_PLAYER -75000
#define CHECKMATE_FOR_CURRENT_PLAYER 75000
#define THEIR_BEST_MOVE_START_VAL 100000
#define MY_BEST_MOVE_START_VAL -100000

// define some depth limits
#define MAX_KILLER_HISTORY_DEPTH 100
#define MAX_QUIESCENCE_DEPTH 3

// max search depth of 20 for now
#define MAX_SEARCH_DEPTH 7

// stop searching if we have been going for 10s
#define MAX_SEARCH_TIME_IN_MILISECONDS 10000

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

std::array<std::array<int_fast32_t, 2>, MAX_KILLER_HISTORY_DEPTH> killer_moves = {};
std::array<std::array<int_fast32_t, 64>, 64> history;

int_fast32_t score_move(Position* position, uint32_t move, int depth)
{
  /*
  score is  -> captures (mvvlva ranked)
            -> checks
            -> killer moves (quiet only, early beta cutoff forcing)
            -> history moves (played frequently recently)
  */

  if (!decode_capture(move))
  {
    // score 1st killer move
    if (killer_moves[0][depth] == move)
    {
      return 15000;
    }
    // score 2nd killer move
    else if (killer_moves[1][depth] == move)
    {
      return 14000;
    }
    else if (decode_check(move))
    {
      return 10000;
    }
    else
    {
      return history[decode_from_square(move)][decode_to_square(move)] / 1000;
    }
  }

  PieceAsInt attacker_piece_move = decode_moved_piece(move);
  PieceAsInt victim_piece_move = victim_on_square(position, static_cast<Square>(decode_to_square(move)));

  return mvv_lva_table[attacker_piece_move][victim_piece_move] + 20000;
}

bool compare_move_pair(Position* position, uint32_t move1, uint32_t move2, int depth)
{
  int_fast32_t score1 = score_move(position, move1, depth);
  int_fast32_t score2 = score_move(position, move2, depth);

  return score1 > score2;
}

std::vector<uint32_t> inline ordered_moves_for_search(Position* position, int depth)
{
  std::vector<uint32_t> moves = valid_moves_for_position(*position);
  std::sort(moves.begin(), moves.end(),
            [position, depth](uint32_t move1, uint32_t move2)
            { return compare_move_pair(position, move1, move2, depth); });
  return moves;
}

uint32_t find_move_and_display_statistics(Position* position)
{
  find_move_return_val fmrv = find_move(position);
  float nps;

  try
  {
    nps = fmrv.nodes / ((fmrv.miliseconds_of_search_time.count() + 1.0) / 1000);
  }
  catch (const std::exception& e)
  {
    nps = 0.0;
  }

  std::cout << "info"
            << " nodes " << fmrv.nodes << " nps " << nps << " time " << fmrv.miliseconds_of_search_time.count()
            << " eval " << fmrv.position_score << std::endl;

  return fmrv.best_move;
}

find_move_return_val find_move(Position* position)
{
  uint32_t last_best_move, current_best_move;
  int_fast32_t last_best_score, current_best_score;
  uint64_t nodes = 0;
  int depth;

  auto start_time = std::chrono::high_resolution_clock::now();

  for (depth = 1; depth <= MAX_SEARCH_DEPTH; depth++)
  {
    std::cout << "info depth " << depth << std::endl;
    current_best_score = MY_BEST_MOVE_START_VAL;
    int_fast32_t current_score = 0;
    std::vector<uint32_t> moves = ordered_moves_for_search(position, depth);

    for (uint32_t move : moves)
    {
      Position new_position = make_move(position, move);
      current_score = -principal_variation_search(&new_position, depth - 1, -THEIR_BEST_MOVE_START_VAL,
                                                  -MY_BEST_MOVE_START_VAL, &nodes);

      nodes += 1;

      if (current_score > current_best_score)
      {
        current_best_move = move;
        current_best_score = current_score;
      }

      auto end_time = std::chrono::high_resolution_clock::now();
      auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

      if (duration.count() > MAX_SEARCH_TIME_IN_MILISECONDS)
      {
        // this is a time cutoff, so return the best move in the last iteration
        return {last_best_move, nodes, duration, depth, last_best_score};
      }
    }

    // no need to deepen if we've found a checkmate
    if (current_best_score == CHECKMATE_FOR_LAST_PLAYER)
    {
      break;
    }

    last_best_move = current_best_move;
    last_best_score = current_best_score;
  }

  auto end_time = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
  // depth cutoff or checkmate return, we can use the current best move and score
  return {current_best_move, nodes, duration, depth, current_best_score};
}

int_fast32_t principal_variation_search(Position* position, int depth, int_fast32_t alpha, int_fast32_t beta,
                                        uint64_t* nodes)
{
  if (depth == 0)
  {
    *nodes += 1;
    return evaluate_position(position);
  }

  std::vector<uint32_t> possible_moves = ordered_moves_for_search(position, depth);
  if (possible_moves.empty())
  {
    // look for a mate
    if (position->white_to_move)
    {
      int king_position = bitboard_to_square(white_kings(position));
      if (is_square_attacked(true, king_position, position))
      {
        return CHECKMATE_FOR_LAST_PLAYER;
      }
    }
    else
    {
      int king_position = bitboard_to_square(black_kings(position));
      if (is_square_attacked(false, king_position, position))
      {
        return CHECKMATE_FOR_LAST_PLAYER;
      }
    }

    /*
      Engine should return stalemate if we have the option to stalemate
      while we think we are losing (only positive selection available)
    */
    return 1;
  }

  int_fast32_t score;
  bool first_move = true;

  for (uint32_t move : possible_moves)
  {
    Position new_position = make_move(position, move);

    if (first_move)
    {
      score = -principal_variation_search(&new_position, depth - 1, -beta, -alpha, nodes);
      *nodes += 1;
      first_move = false;
    }
    else
    {
      score = -principal_variation_search(&new_position, depth - 1, -alpha - 1, -alpha, nodes);
      *nodes += 1;

      if (alpha < score && score < beta)
      {
        score = -principal_variation_search(&new_position, depth - 1, -beta, -score, nodes);
        *nodes += 1;
      }
    }

    alpha = std::max(alpha, score);

    if (alpha >= beta)
    {
      killer_moves[1][depth] = killer_moves[0][depth];
      killer_moves[0][depth] = move;
      history[decode_from_square(move)][decode_to_square(move)] += depth * depth;
      break;
    }
    else
    {
      history[decode_from_square(move)][decode_to_square(move)] -= depth * depth;
    }
  }

  return alpha;
}
