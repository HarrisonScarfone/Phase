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

// define some depth limits
#define MAX_KILLER_HISTORY_DEPTH 100
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

std::array<std::array<int_fast32_t, 2>, MAX_KILLER_HISTORY_DEPTH> killer_moves = {};

int_fast32_t score_move(Position* position, uint32_t move, int depth)
{
  /*
  score is  -> captures (mvvlva ranked)
            -> checks
            -> killer moves (quiet only, early beta cutoff forcing)
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

    if (decode_check(move))
    {
      return 10000;
    }

    return 0;
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

uint32_t find_move_with_statistics(Position* position, int depth)
{
  uint64_t nodes = 0;
  auto start_time = std::chrono::high_resolution_clock::now();
  uint32_t best_move = find_move(position, depth, &nodes);
  auto end_time = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

  float nps;

  try
  {
    nps = nodes / ((duration.count() + 1.0) / 1000);
  }
  catch (const std::exception& e)
  {
    nps = 0.0;
  }

  std::cout << "info"
            << " nodes " << nodes << " nps " << nps << " time " << duration.count() << std::endl;

  return best_move;
}

uint32_t find_move(Position* position, int depth, uint64_t* nodes)
{
  uint32_t best_move;
  int_fast32_t best_score = MY_BEST_MOVE_START_VAL;
  int_fast32_t current_score;
  std::vector<uint32_t> moves = ordered_moves_for_search(position, depth);

  for (uint32_t move : moves)
  {
    Position new_position = make_move(position, move);
    current_score = -negamax(&new_position, depth - 1, -THEIR_BEST_MOVE_START_VAL, -best_score, nodes);
    *nodes += 1;

    if (current_score > best_score)
    {
      best_move = move;
      best_score = current_score;
    }
  }

  return best_move;
}

int_fast32_t negamax(Position* position, int depth, int_fast32_t alpha, int_fast32_t beta, uint64_t* nodes)
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

    // engine should only stalemate if we are getting pumped
    return -1000;
  }

  int_fast32_t score;
  bool first_move = true;

  for (uint32_t move : possible_moves)
  {
    Position new_position = make_move(position, move);

    if (first_move)
    {
      score = -negamax(&new_position, depth - 1, -beta, -alpha, nodes);
      *nodes += 1;
      first_move = false;
    }
    else
    {
      score = -negamax(&new_position, depth - 1, -alpha - 1, -alpha, nodes);
      *nodes += 1;

      if (alpha < score && score < beta)
      {
        score = -negamax(&new_position, depth - 1, -beta, -score, nodes);
        *nodes += 1;
      }
    }

    alpha = std::max(alpha, score);

    if (alpha >= beta)
    {
      killer_moves[1][depth] = killer_moves[0][depth];
      killer_moves[0][depth] = move;
      break;
    }
  }

  return alpha;
}
