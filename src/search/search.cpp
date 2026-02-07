#include "search.hpp"

#include <algorithm>
#include <atomic>
#include <cmath>
#include <iostream>
#include <limits>
#include <ranges>

#include "../evaluator/evaluator.hpp"
#include "../game/moves.hpp"
#include "../util/zobrist.hpp"
#include "transposition.hpp"

// Score constants
constexpr int MATE_SCORE = 30000;
constexpr int INFINITY_SCORE = 100000;
constexpr int DRAW_SCORE = 0;

// Draw contempt - prefer draws when losing, avoid when winning
constexpr int CONTEMPT_THRESHOLD = 150;  // Score threshold for contempt
constexpr int CONTEMPT_VALUE = 20;       // Base contempt value

// Depth limits
constexpr int MAX_KILLER_HISTORY_DEPTH = 100;
constexpr int MAX_QUIESCENCE_DEPTH = 8;
constexpr int MAX_SEARCH_DEPTH = 64;

// Default time limit (overridden by time management)
constexpr int DEFAULT_SEARCH_TIME_MS = 10000;

// Mate score bounds (for TT storage)
constexpr int MATE_BOUND = MATE_SCORE - MAX_SEARCH_DEPTH;

// Legacy defines for compatibility during transition
#define CHECKMATE_FOR_LAST_PLAYER (-MATE_SCORE)
#define CHECKMATE_FOR_CURRENT_PLAYER MATE_SCORE
#define THEIR_BEST_MOVE_START_VAL INFINITY_SCORE
#define MY_BEST_MOVE_START_VAL (-INFINITY_SCORE)

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

// Thread-local search data
thread_local std::array<std::array<uint32_t, 2>, MAX_KILLER_HISTORY_DEPTH> killer_moves = {};
thread_local std::array<std::array<int_fast32_t, 64>, 64> history = {};
thread_local std::vector<uint64_t> search_stack;

// Global search context for game history and draw detection
SearchContext search_context;

// Thread count for Lazy SMP
static int num_threads = 1;
static std::atomic<uint64_t> total_nodes{0};

// Calculate draw score with contempt based on root evaluation
static int get_draw_score()
{
  // When winning, we don't want draws (negative score for draw)
  // When losing, we prefer draws (positive score for draw)
  int root_score = search_context.root_score;

  if (root_score > CONTEMPT_THRESHOLD)
  {
    return -CONTEMPT_VALUE;  // Avoid draws when winning
  }
  else if (root_score < -CONTEMPT_THRESHOLD)
  {
    return CONTEMPT_VALUE;  // Prefer draws when losing
  }

  return DRAW_SCORE;
}

// Check for repetition (2-fold in search, 3-fold with game history)
static bool is_repetition(uint64_t hash)
{
  // Check in search stack (2-fold repetition during search is a draw)
  int count = 0;
  for (auto it = search_stack.rbegin(); it != search_stack.rend(); ++it)
  {
    if (*it == hash)
    {
      count++;
      if (count >= 1)
        return true;  // Found same position once before in search = 2-fold
    }
  }

  // Check in game history (need 2 occurrences for 3-fold total)
  for (uint64_t h : search_context.game_history)
  {
    if (h == hash)
    {
      count++;
      if (count >= 2)
        return true;  // 3-fold repetition with game history
    }
  }

  return false;
}

// Score constants for move ordering
constexpr int TT_MOVE_SCORE = 100000;
constexpr int CAPTURE_BASE_SCORE = 20000;
constexpr int KILLER_1_SCORE = 15000;
constexpr int KILLER_2_SCORE = 14000;
constexpr int CHECK_SCORE = 10000;

int_fast32_t score_move(Position* position, uint32_t move, int depth, uint32_t tt_move)
{
  // TT move gets highest priority
  if (move == tt_move && tt_move != 0)
  {
    return TT_MOVE_SCORE;
  }

  if (!decode_capture(move))
  {
    // score 1st killer move
    if (depth < MAX_KILLER_HISTORY_DEPTH && killer_moves[depth][0] == move)
    {
      return KILLER_1_SCORE;
    }
    // score 2nd killer move
    else if (depth < MAX_KILLER_HISTORY_DEPTH && killer_moves[depth][1] == move)
    {
      return KILLER_2_SCORE;
    }
    else if (decode_check(move))
    {
      return CHECK_SCORE;
    }
    else
    {
      return history[decode_from_square(move)][decode_to_square(move)] / 1000;
    }
  }

  PieceAsInt attacker_piece_move = decode_moved_piece(move);
  PieceAsInt victim_piece_move = victim_on_square(position, static_cast<Square>(decode_to_square(move)));

  return mvv_lva_table[attacker_piece_move][victim_piece_move] + CAPTURE_BASE_SCORE;
}

bool compare_move_pair(Position* position, uint32_t move1, uint32_t move2, int depth, uint32_t tt_move)
{
  int_fast32_t score1 = score_move(position, move1, depth, tt_move);
  int_fast32_t score2 = score_move(position, move2, depth, tt_move);

  return score1 > score2;
}

std::vector<uint32_t> inline ordered_moves_for_search(Position* position, int depth, uint32_t tt_move = 0)
{
  std::vector<uint32_t> moves = valid_moves_for_position(*position);
  std::sort(moves.begin(), moves.end(),
            [position, depth, tt_move](uint32_t move1, uint32_t move2)
            { return compare_move_pair(position, move1, move2, depth, tt_move); });
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

// Global search state
static std::chrono::high_resolution_clock::time_point search_start_time;
static int search_time_limit_ms = DEFAULT_SEARCH_TIME_MS;
static std::atomic<bool> search_stopped{false};

// Public functions to control search
void set_search_time_limit(int time_ms) { search_time_limit_ms = time_ms; }

void stop_search() { search_stopped.store(true, std::memory_order_relaxed); }

void set_thread_count(int threads) { num_threads = std::max(1, std::min(threads, 64)); }

int get_thread_count() { return num_threads; }

// Check if we should stop searching
static bool should_stop()
{
  if (search_stopped.load(std::memory_order_relaxed))
    return true;

  auto now = std::chrono::high_resolution_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - search_start_time);
  return elapsed.count() > search_time_limit_ms;
}

find_move_return_val find_move_with_history(Position* position, const std::vector<uint64_t>& history)
{
  search_context.game_history = history;
  return find_move(position);
}

// Aspiration window parameters
constexpr int ASP_WINDOW_INITIAL = 25;  // Initial window size (centipawns)
constexpr int ASP_WINDOW_MIN_DEPTH = 4;  // Start using aspiration windows at this depth

// Shared results for Lazy SMP
static std::atomic<uint32_t> shared_best_move{0};
static std::atomic<int_fast32_t> shared_best_score{0};
static std::atomic<int> shared_best_depth{0};

// Worker thread search function
static void search_worker(Position position, int thread_id)
{
  // Initialize thread-local data
  search_stack.clear();
  search_stack.reserve(MAX_SEARCH_DEPTH + 10);

  // Age history (each thread has its own)
  for (auto& row : history)
  {
    for (auto& val : row)
    {
      val /= 2;
    }
  }
  for (auto& km : killer_moves)
    km.fill(0);

  uint32_t last_best_move = 0, current_best_move = 0;
  int_fast32_t last_best_score = 0, current_best_score;
  uint64_t nodes = 0;

  // Get TT move hint for root
  uint32_t tt_move = TT.probe_move(position.hash);

  // Start at different depths to diversify search (Lazy SMP technique)
  int start_depth = 1 + (thread_id % 2);

  for (int depth = start_depth; depth <= MAX_SEARCH_DEPTH; depth++)
  {
    if (should_stop())
      break;

    int_fast32_t alpha = MY_BEST_MOVE_START_VAL;
    int_fast32_t beta = THEIR_BEST_MOVE_START_VAL;

    if (depth >= ASP_WINDOW_MIN_DEPTH && last_best_score > -MATE_BOUND && last_best_score < MATE_BOUND)
    {
      alpha = last_best_score - ASP_WINDOW_INITIAL;
      beta = last_best_score + ASP_WINDOW_INITIAL;
    }

    int asp_delta = ASP_WINDOW_INITIAL;

    while (true)
    {
      if (should_stop())
        break;

      current_best_score = MY_BEST_MOVE_START_VAL;
      int_fast32_t current_score = 0;
      uint32_t depth_best_move = 0;

      std::vector<uint32_t> moves = ordered_moves_for_search(&position, depth, tt_move);

      if (moves.empty())
        break;

      for (uint32_t move : moves)
      {
        if (should_stop())
          break;

        Position new_position = make_move(&position, move);

        current_score = -principal_variation_search(&new_position, depth - 1, 1, -beta, -std::max(alpha, current_best_score), &nodes);

        if (current_score > current_best_score)
        {
          current_best_move = move;
          current_best_score = current_score;
          depth_best_move = move;
        }
      }

      if (should_stop())
        break;

      // Aspiration window re-search
      if (depth >= ASP_WINDOW_MIN_DEPTH && last_best_score > -MATE_BOUND && last_best_score < MATE_BOUND)
      {
        if (current_best_score <= alpha)
        {
          asp_delta *= 2;
          alpha = std::max(static_cast<int_fast32_t>(-INFINITY_SCORE), last_best_score - asp_delta);
          continue;
        }
        else if (current_best_score >= beta)
        {
          asp_delta *= 2;
          beta = std::min(static_cast<int_fast32_t>(INFINITY_SCORE), last_best_score + asp_delta);
          continue;
        }
      }

      tt_move = depth_best_move;
      break;
    }

    if (should_stop())
      break;

    // Update shared best if this thread found a better result
    int current_shared_depth = shared_best_depth.load(std::memory_order_relaxed);
    if (depth > current_shared_depth)
    {
      shared_best_move.store(current_best_move, std::memory_order_relaxed);
      shared_best_score.store(current_best_score, std::memory_order_relaxed);
      shared_best_depth.store(depth, std::memory_order_relaxed);
    }

    // Accumulate nodes
    total_nodes.fetch_add(nodes, std::memory_order_relaxed);

    // Output info from thread 0 only
    if (thread_id == 0)
    {
      auto now = std::chrono::high_resolution_clock::now();
      auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - search_start_time);
      uint64_t total = total_nodes.load(std::memory_order_relaxed);
      float nps = total / ((elapsed.count() + 1.0f) / 1000.0f);

      std::cout << "info depth " << depth << " seldepth " << depth << " score ";
      if (current_best_score >= MATE_BOUND)
      {
        int mate_in = (MATE_SCORE - current_best_score + 1) / 2;
        std::cout << "mate " << mate_in;
      }
      else if (current_best_score <= -MATE_BOUND)
      {
        int mate_in = -(MATE_SCORE + current_best_score + 1) / 2;
        std::cout << "mate " << mate_in;
      }
      else
      {
        std::cout << "cp " << current_best_score;
      }
      std::cout << " nodes " << total << " nps " << static_cast<int>(nps) << " time " << elapsed.count() << " hashfull "
                << TT.hashfull() << " pv " << uint_move_to_engine_string_move(current_best_move) << std::endl;
    }

    nodes = 0;
    last_best_move = current_best_move;
    last_best_score = current_best_score;

    // Check for mate
    if (current_best_score >= MATE_BOUND || current_best_score <= -MATE_BOUND)
      break;
  }

  // Add remaining nodes
  total_nodes.fetch_add(nodes, std::memory_order_relaxed);
}

find_move_return_val find_move(Position* position)
{
  // Reset search state
  search_start_time = std::chrono::high_resolution_clock::now();
  search_stopped.store(false, std::memory_order_relaxed);
  total_nodes.store(0, std::memory_order_relaxed);
  shared_best_move.store(0, std::memory_order_relaxed);
  shared_best_score.store(0, std::memory_order_relaxed);
  shared_best_depth.store(0, std::memory_order_relaxed);

  // Start new TT generation
  TT.new_search();

  // Launch worker threads
  std::vector<std::thread> threads;
  for (int i = 1; i < num_threads; i++)
  {
    threads.emplace_back(search_worker, *position, i);
  }

  // Main thread also searches (thread 0)
  search_worker(*position, 0);

  // Wait for all threads to finish
  for (auto& t : threads)
  {
    t.join();
  }

  // Get results
  uint32_t best_move = shared_best_move.load(std::memory_order_relaxed);
  int_fast32_t best_score = shared_best_score.load(std::memory_order_relaxed);
  int best_depth = shared_best_depth.load(std::memory_order_relaxed);
  uint64_t nodes = total_nodes.load(std::memory_order_relaxed);

  auto end_time = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - search_start_time);

  // Output final info
  float nps = nodes / ((duration.count() + 1.0f) / 1000.0f);
  std::cout << "info depth " << best_depth << " seldepth " << best_depth << " score ";
  if (best_score >= MATE_BOUND)
  {
    int mate_in = (MATE_SCORE - best_score + 1) / 2;
    std::cout << "mate " << mate_in;
  }
  else if (best_score <= -MATE_BOUND)
  {
    int mate_in = -(MATE_SCORE + best_score + 1) / 2;
    std::cout << "mate " << mate_in;
  }
  else
  {
    std::cout << "cp " << best_score;
  }
  std::cout << " nodes " << nodes << " nps " << static_cast<int>(nps) << " time " << duration.count() << " hashfull "
            << TT.hashfull();
  if (best_move != 0)
  {
    std::cout << " pv " << uint_move_to_engine_string_move(best_move);
  }
  std::cout << std::endl;

  // Update root score for next search
  search_context.root_score = best_score;

  return {best_move, nodes, duration, best_depth, best_score};
}

// Adjust mate scores for TT storage (distance to mate)
static int score_to_tt(int score, int ply)
{
  if (score >= MATE_BOUND)
    return score + ply;
  if (score <= -MATE_BOUND)
    return score - ply;
  return score;
}

static int score_from_tt(int score, int ply)
{
  if (score >= MATE_BOUND)
    return score - ply;
  if (score <= -MATE_BOUND)
    return score + ply;
  return score;
}

// Generate capture moves only (for quiescence search)
static std::vector<uint32_t> generate_captures(Position* position)
{
  std::vector<uint32_t> all_moves = valid_moves_for_position(*position);
  std::vector<uint32_t> captures;
  captures.reserve(32);

  for (uint32_t move : all_moves)
  {
    if (decode_capture(move))
    {
      captures.push_back(move);
    }
  }

  // Sort captures by MVV-LVA
  std::sort(captures.begin(), captures.end(),
            [position](uint32_t m1, uint32_t m2)
            {
              PieceAsInt v1 = victim_on_square(position, static_cast<Square>(decode_to_square(m1)));
              PieceAsInt v2 = victim_on_square(position, static_cast<Square>(decode_to_square(m2)));
              PieceAsInt a1 = decode_moved_piece(m1);
              PieceAsInt a2 = decode_moved_piece(m2);
              return mvv_lva_table[a1][v1] > mvv_lva_table[a2][v2];
            });

  return captures;
}

// Quiescence search - search captures until position is "quiet"
static int_fast32_t quiescence_search(Position* position, int ply, int_fast32_t alpha, int_fast32_t beta,
                                      uint64_t* nodes)
{
  // Check for time limit periodically
  if ((*nodes & 4095) == 0 && should_stop())
  {
    return 0;
  }

  *nodes += 1;

  // Stand-pat: evaluate current position
  int_fast32_t stand_pat = evaluate_position(position);

  // Beta cutoff - position is already too good
  if (stand_pat >= beta)
  {
    return beta;
  }

  // Delta pruning - if we're way behind, even a queen capture won't help
  constexpr int DELTA_MARGIN = 1100;  // Slightly more than queen value
  if (stand_pat + DELTA_MARGIN < alpha)
  {
    return alpha;
  }

  // Update alpha if stand-pat is better
  if (stand_pat > alpha)
  {
    alpha = stand_pat;
  }

  // Generate and search captures
  std::vector<uint32_t> captures = generate_captures(position);

  for (uint32_t move : captures)
  {
    Position new_position = make_move(position, move);

    int_fast32_t score = -quiescence_search(&new_position, ply + 1, -beta, -alpha, nodes);

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

// Check if position is in check
static bool is_in_check(Position* position)
{
  int king_sq;
  if (position->white_to_move)
  {
    king_sq = bitscan(white_kings(position));
    return is_square_attacked(true, king_sq, position);
  }
  else
  {
    king_sq = bitscan(black_kings(position));
    return is_square_attacked(false, king_sq, position);
  }
}

// Null move pruning parameters
constexpr int NULL_MOVE_R = 3;  // Reduction depth
constexpr int NULL_MOVE_MIN_DEPTH = 3;  // Minimum depth for null move

// LMR parameters
constexpr int LMR_MIN_DEPTH = 3;
constexpr int LMR_FULL_DEPTH_MOVES = 4;  // First N moves searched at full depth

// LMR reduction table (initialized lazily)
static int lmr_reductions[64][64] = {};
static bool lmr_initialized = false;

static void init_lmr()
{
  if (lmr_initialized) return;
  for (int d = 1; d < 64; d++)
  {
    for (int m = 1; m < 64; m++)
    {
      lmr_reductions[d][m] = static_cast<int>(0.5 + log(d) * log(m) / 2.0);
    }
  }
  lmr_initialized = true;
}

int_fast32_t principal_variation_search(Position* position, int depth, int ply, int_fast32_t alpha, int_fast32_t beta,
                                        uint64_t* nodes)
{
  // Initialize LMR table on first call
  init_lmr();

  // Check for time limit periodically (every 4096 nodes)
  if ((*nodes & 4095) == 0 && should_stop())
  {
    return 0;
  }

  // Check for draw by repetition
  if (is_repetition(position->hash))
  {
    return get_draw_score();
  }

  // Check for 50-move draw
  if (position->half_move_clock >= 100)
  {
    return get_draw_score();
  }

  // Push current position to search stack for repetition detection
  search_stack.push_back(position->hash);

  bool is_pv = (beta - alpha) > 1;
  int_fast32_t original_alpha = alpha;
  uint32_t best_move = 0;
  int_fast32_t best_score = MY_BEST_MOVE_START_VAL;

  // Check if we're in check (needed for pruning decisions)
  bool in_check = is_in_check(position);

  // Probe transposition table
  uint32_t tt_move = 0;
  const TTEntry* tt_entry = TT.probe(position->hash);
  if (tt_entry)
  {
    tt_move = tt_entry->best_move;

    // Use TT score for cutoff if depth is sufficient and not in PV
    if (tt_entry->depth >= depth && !is_pv)
    {
      int tt_score = score_from_tt(tt_entry->score, ply);

      if (tt_entry->flag == TT_EXACT)
      {
        search_stack.pop_back();
        return tt_score;
      }
      else if (tt_entry->flag == TT_LOWER_BOUND && tt_score >= beta)
      {
        search_stack.pop_back();
        return tt_score;
      }
      else if (tt_entry->flag == TT_UPPER_BOUND && tt_score <= alpha)
      {
        search_stack.pop_back();
        return tt_score;
      }
    }
  }

  // Leaf node - enter quiescence search
  if (depth <= 0)
  {
    search_stack.pop_back();
    return quiescence_search(position, ply, alpha, beta, nodes);
  }

  // Null move pruning
  // Don't do null move if: in check, PV node, or low material (zugzwang risk)
  if (!is_pv && !in_check && depth >= NULL_MOVE_MIN_DEPTH && ply > 0)
  {
    // Check for sufficient material (at least one non-pawn piece)
    uint64_t our_pieces = position->white_to_move ? position->white : position->black;
    uint64_t non_pawn_material = our_pieces & ~position->pawns & ~position->kings;

    if (non_pawn_material)
    {
      // Make null move (just flip side to move)
      Position null_position = *position;
      null_position.white_to_move = !position->white_to_move;
      null_position.enPassantTarget = 0;
      null_position.hash ^= zobrist::side_key;
      if (position->enPassantTarget)
      {
        null_position.hash ^= zobrist::ep_file_keys[file_of(bitscan(position->enPassantTarget))];
      }

      *nodes += 1;

      // Search with reduced depth
      int R = NULL_MOVE_R + (depth > 6 ? 1 : 0);  // Adaptive reduction
      int_fast32_t null_score = -principal_variation_search(&null_position, depth - 1 - R, ply + 1, -beta, -beta + 1, nodes);

      // Null move cutoff
      if (null_score >= beta)
      {
        // Don't return mate scores from null move search
        if (null_score >= MATE_BOUND)
          null_score = beta;
        search_stack.pop_back();
        return null_score;
      }
    }
  }

  // Generate and order moves
  std::vector<uint32_t> possible_moves = ordered_moves_for_search(position, depth, tt_move);

  // No legal moves - checkmate or stalemate
  if (possible_moves.empty())
  {
    search_stack.pop_back();
    if (in_check)
    {
      // Checkmate - return mate score adjusted for ply (prefer shorter mates)
      return -MATE_SCORE + ply;
    }
    else
    {
      // Stalemate - use draw contempt
      return get_draw_score();
    }
  }

  int_fast32_t score;
  int moves_searched = 0;

  // Get static eval for futility pruning
  int_fast32_t static_eval = evaluate_position(position);

  // Futility pruning margins
  constexpr int FUTILITY_MARGIN_1 = 200;  // Depth 1
  constexpr int FUTILITY_MARGIN_2 = 400;  // Depth 2
  constexpr int FUTILITY_MARGIN_3 = 600;  // Depth 3

  for (uint32_t move : possible_moves)
  {
    bool is_capture = decode_capture(move);
    bool gives_check = decode_check(move);
    bool is_promotion = decode_promoted_to_piece(move) != NO_PIECE;

    // Futility pruning - skip hopeless quiet moves at low depths
    if (!is_pv && !in_check && !is_capture && !is_promotion && !gives_check && moves_searched > 0)
    {
      int futility_margin = 0;
      if (depth == 1)
        futility_margin = FUTILITY_MARGIN_1;
      else if (depth == 2)
        futility_margin = FUTILITY_MARGIN_2;
      else if (depth == 3)
        futility_margin = FUTILITY_MARGIN_3;

      if (futility_margin > 0 && static_eval + futility_margin <= alpha)
      {
        continue;  // Skip this move - it's futile
      }
    }

    Position new_position = make_move(position, move);
    *nodes += 1;

    int new_depth = depth - 1;

    // Check extension
    if (gives_check)
    {
      new_depth += 1;
    }

    moves_searched++;

    if (moves_searched == 1)
    {
      // First move - full window search
      score = -principal_variation_search(&new_position, new_depth, ply + 1, -beta, -alpha, nodes);
    }
    else
    {
      // Late Move Reductions (LMR)
      int reduction = 0;

      if (depth >= LMR_MIN_DEPTH && moves_searched > LMR_FULL_DEPTH_MOVES && !in_check && !is_capture && !is_promotion &&
          !gives_check)
      {
        // Calculate reduction based on depth and move number
        reduction = lmr_reductions[std::min(depth, 63)][std::min(moves_searched, 63)];

        // Reduce less for killer moves
        if (depth < MAX_KILLER_HISTORY_DEPTH &&
            (killer_moves[depth][0] == move || killer_moves[depth][1] == move))
        {
          reduction = std::max(0, reduction - 1);
        }

        // Don't reduce into negative depth
        reduction = std::min(reduction, new_depth - 1);
      }

      // Zero-window search with possible reduction
      score = -principal_variation_search(&new_position, new_depth - reduction, ply + 1, -alpha - 1, -alpha, nodes);

      // Re-search at full depth if reduced search raises alpha
      if (reduction > 0 && score > alpha)
      {
        score = -principal_variation_search(&new_position, new_depth, ply + 1, -alpha - 1, -alpha, nodes);
      }

      // Re-search with full window if score falls within window (PVS)
      if (score > alpha && score < beta)
      {
        score = -principal_variation_search(&new_position, new_depth, ply + 1, -beta, -alpha, nodes);
      }
    }

    if (score > best_score)
    {
      best_score = score;
      best_move = move;
    }

    if (score > alpha)
    {
      alpha = score;
    }

    if (alpha >= beta)
    {
      // Beta cutoff - update killer moves and history for quiet moves
      if (!decode_capture(move))
      {
        if (depth < MAX_KILLER_HISTORY_DEPTH)
        {
          killer_moves[depth][1] = killer_moves[depth][0];
          killer_moves[depth][0] = move;
        }
        history[decode_from_square(move)][decode_to_square(move)] += depth * depth;
      }
      break;
    }
    else if (!decode_capture(move))
    {
      // Penalize moves that don't cause cutoff
      history[decode_from_square(move)][decode_to_square(move)] -= depth * depth;
    }
  }

  // Store in transposition table
  TTFlag flag;
  if (best_score <= original_alpha)
  {
    flag = TT_UPPER_BOUND;
  }
  else if (best_score >= beta)
  {
    flag = TT_LOWER_BOUND;
  }
  else
  {
    flag = TT_EXACT;
  }

  TT.store(position->hash, depth, score_to_tt(best_score, ply), static_eval, flag, best_move);

  // Pop from search stack before returning
  search_stack.pop_back();
  return best_score;
}
