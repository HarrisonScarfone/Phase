#include "evaluator.hpp"

#include "../game/moves.hpp"
#include "../util/constants.hpp"

// Piece values for midgame and endgame
constexpr int PAWN_MG = 100;
constexpr int KNIGHT_MG = 320;
constexpr int BISHOP_MG = 330;
constexpr int ROOK_MG = 500;
constexpr int QUEEN_MG = 950;

constexpr int PAWN_EG = 120;
constexpr int KNIGHT_EG = 300;
constexpr int BISHOP_EG = 320;
constexpr int ROOK_EG = 530;
constexpr int QUEEN_EG = 1000;

// Phase values for tapered evaluation
constexpr int KNIGHT_PHASE = 1;
constexpr int BISHOP_PHASE = 1;
constexpr int ROOK_PHASE = 2;
constexpr int QUEEN_PHASE = 4;
constexpr int TOTAL_PHASE = KNIGHT_PHASE * 4 + BISHOP_PHASE * 4 + ROOK_PHASE * 4 + QUEEN_PHASE * 2;

// Bonus/penalty values
constexpr int BISHOP_PAIR_BONUS = 30;
constexpr int ROOK_OPEN_FILE = 20;
constexpr int ROOK_SEMI_OPEN = 10;
constexpr int ROOK_ON_7TH = 25;
constexpr int DOUBLED_PAWN_PENALTY = 15;
constexpr int ISOLATED_PAWN_PENALTY = 15;
constexpr int PASSED_PAWN_BONUS_BASE = 20;
constexpr int CONNECTED_PAWN_BONUS = 10;
constexpr int KNIGHT_OUTPOST = 15;

// King safety
constexpr int KING_PAWN_SHIELD_BONUS = 10;
constexpr int KING_OPEN_FILE_PENALTY = 25;
constexpr int KING_ATTACK_WEIGHT = 2;

// Mobility weights (per square attacked)
constexpr int KNIGHT_MOBILITY = 4;
constexpr int BISHOP_MOBILITY = 3;
constexpr int ROOK_MOBILITY = 2;
constexpr int QUEEN_MOBILITY = 1;

// Endgame king PST (centralization)
// clang-format off
constexpr std::array<int, 64> king_eg_pst = {
    -30, -20, -10,   0,   0, -10, -20, -30,
    -20, -10,   0,  10,  10,   0, -10, -20,
    -10,   0,  10,  20,  20,  10,   0, -10,
      0,  10,  20,  30,  30,  20,  10,   0,
      0,  10,  20,  30,  30,  20,  10,   0,
    -10,   0,  10,  20,  20,  10,   0, -10,
    -20, -10,   0,  10,  10,   0, -10, -20,
    -30, -20, -10,   0,   0, -10, -20, -30
};

// Midgame king PST (stay castled)
constexpr std::array<int, 64> white_king_mg_pst = {
    -30, -40, -40, -50, -50, -40, -40, -30,
    -30, -40, -40, -50, -50, -40, -40, -30,
    -30, -40, -40, -50, -50, -40, -40, -30,
    -30, -40, -40, -50, -50, -40, -40, -30,
    -20, -30, -30, -40, -40, -30, -30, -20,
    -10, -20, -20, -20, -20, -20, -20, -10,
     10,  10,   0,   0,   0,   0,  10,  10,
     20,  30,  10,   0,   0,  10,  30,  20
};

constexpr std::array<int, 64> black_king_mg_pst = {
     20,  30,  10,   0,   0,  10,  30,  20,
     10,  10,   0,   0,   0,   0,  10,  10,
    -10, -20, -20, -20, -20, -20, -20, -10,
    -20, -30, -30, -40, -40, -30, -30, -20,
    -30, -40, -40, -50, -50, -40, -40, -30,
    -30, -40, -40, -50, -50, -40, -40, -30,
    -30, -40, -40, -50, -50, -40, -40, -30,
    -30, -40, -40, -50, -50, -40, -40, -30
};
// clang-format on

// Calculate game phase (0 = endgame, TOTAL_PHASE = opening)
static int calculate_phase(Position* position)
{
  int phase = 0;
  phase += bitcount(position->knights) * KNIGHT_PHASE;
  phase += bitcount(position->bishops) * BISHOP_PHASE;
  phase += bitcount(position->rooks) * ROOK_PHASE;
  phase += bitcount(position->queens) * QUEEN_PHASE;
  return std::min(phase, TOTAL_PHASE);
}

// Interpolate between midgame and endgame scores
static int tapered_eval(int mg_score, int eg_score, int phase)
{
  return (mg_score * phase + eg_score * (TOTAL_PHASE - phase)) / TOTAL_PHASE;
}

// Count attacks near a king
static int count_king_attackers(Position* position, int king_sq, bool attacking_white)
{
  int attacks = 0;
  uint64_t king_zone = king_moves[king_sq];
  uint64_t occupancy = all_occupied(position);

  if (attacking_white)
  {
    // White pieces attacking black king zone
    uint64_t knights = white_knights(position);
    while (knights)
    {
      int sq = bitscan(knights);
      if (knight_moves[sq] & king_zone)
        attacks++;
      knights &= knights - 1;
    }

    uint64_t bishops = white_bishops(position);
    while (bishops)
    {
      int sq = bitscan(bishops);
      if (bishop_attacks(occupancy, sq) & king_zone)
        attacks++;
      bishops &= bishops - 1;
    }

    uint64_t rooks = white_rooks(position);
    while (rooks)
    {
      int sq = bitscan(rooks);
      if (rook_attacks(occupancy, sq) & king_zone)
        attacks++;
      rooks &= rooks - 1;
    }

    uint64_t queens = white_queens(position);
    while (queens)
    {
      int sq = bitscan(queens);
      if (queen_attacks(occupancy, sq) & king_zone)
        attacks += 2;
      queens &= queens - 1;
    }
  }
  else
  {
    // Black pieces attacking white king zone
    uint64_t knights = black_knights(position);
    while (knights)
    {
      int sq = bitscan(knights);
      if (knight_moves[sq] & king_zone)
        attacks++;
      knights &= knights - 1;
    }

    uint64_t bishops = black_bishops(position);
    while (bishops)
    {
      int sq = bitscan(bishops);
      if (bishop_attacks(occupancy, sq) & king_zone)
        attacks++;
      bishops &= bishops - 1;
    }

    uint64_t rooks = black_rooks(position);
    while (rooks)
    {
      int sq = bitscan(rooks);
      if (rook_attacks(occupancy, sq) & king_zone)
        attacks++;
      rooks &= rooks - 1;
    }

    uint64_t queens = black_queens(position);
    while (queens)
    {
      int sq = bitscan(queens);
      if (queen_attacks(occupancy, sq) & king_zone)
        attacks += 2;
      queens &= queens - 1;
    }
  }

  return attacks;
}

// Evaluate pawn structure
static int evaluate_pawns_for_side(uint64_t my_pawns, uint64_t their_pawns, bool white)
{
  int mg_score = 0;
  int eg_score = 0;
  uint64_t pawns = my_pawns;

  while (pawns)
  {
    int sq = bitscan(pawns);
    int file = file_of(sq);
    int rank = rank_of(sq);

    // Base pawn value and PST
    mg_score += PAWN_MG;
    eg_score += PAWN_EG;
    mg_score += white ? white_pawn_pst[sq] : black_pawn_pst[sq];
    eg_score += white ? white_pawn_pst[sq] : black_pawn_pst[sq];

    // File mask for this file
    uint64_t file_mask = FILE_A << file;

    // Doubled pawns
    if (bitcount(my_pawns & file_mask) > 1)
    {
      mg_score -= DOUBLED_PAWN_PENALTY;
      eg_score -= DOUBLED_PAWN_PENALTY;
    }

    // Isolated pawns
    uint64_t adjacent_files = 0;
    if (file > 0)
      adjacent_files |= FILE_A << (file - 1);
    if (file < 7)
      adjacent_files |= FILE_A << (file + 1);

    if (!(my_pawns & adjacent_files))
    {
      mg_score -= ISOLATED_PAWN_PENALTY;
      eg_score -= ISOLATED_PAWN_PENALTY;
    }
    else
    {
      // Connected pawns
      mg_score += CONNECTED_PAWN_BONUS;
    }

    // Passed pawns
    uint64_t passed_mask = file_mask | adjacent_files;
    if (white)
    {
      // For white, check no black pawns ahead
      uint64_t ahead_mask = 0;
      for (int r = 0; r < rank; r++)
      {
        ahead_mask |= 0xFFULL << (r * 8);
      }
      if (!(their_pawns & passed_mask & ahead_mask))
      {
        // Passed pawn bonus scales with advancement
        int advancement = 7 - rank;
        mg_score += PASSED_PAWN_BONUS_BASE + advancement * 5;
        eg_score += PASSED_PAWN_BONUS_BASE + advancement * 10;
      }
    }
    else
    {
      // For black, check no white pawns ahead
      uint64_t ahead_mask = 0;
      for (int r = rank + 1; r < 8; r++)
      {
        ahead_mask |= 0xFFULL << (r * 8);
      }
      if (!(their_pawns & passed_mask & ahead_mask))
      {
        int advancement = rank;
        mg_score += PASSED_PAWN_BONUS_BASE + advancement * 5;
        eg_score += PASSED_PAWN_BONUS_BASE + advancement * 10;
      }
    }

    pawns &= pawns - 1;
  }

  return mg_score;  // Simplified - full tapered would track both
}

// Evaluate pieces for one side
static void evaluate_pieces(Position* position, bool white, int& mg_score, int& eg_score, int& mobility)
{
  uint64_t occupancy = all_occupied(position);
  uint64_t my_pieces = white ? position->white : position->black;
  uint64_t their_pieces = white ? position->black : position->white;
  uint64_t my_pawns = white ? white_pawns(position) : black_pawns(position);
  uint64_t their_pawns = white ? black_pawns(position) : white_pawns(position);

  // Knights
  uint64_t knights = white ? white_knights(position) : black_knights(position);
  while (knights)
  {
    int sq = bitscan(knights);
    mg_score += KNIGHT_MG + knight_pst[sq];
    eg_score += KNIGHT_EG + knight_pst[sq];

    // Mobility
    uint64_t attacks = knight_moves[sq] & ~my_pieces;
    mobility += bitcount(attacks) * KNIGHT_MOBILITY;

    // Outpost bonus (protected by pawn, can't be attacked by enemy pawn)
    // Simplified check
    if (white && rank_of(sq) <= 3)
    {
      uint64_t pawn_defenders = pawn_attacks[1][sq] & my_pawns;
      if (pawn_defenders)
        mg_score += KNIGHT_OUTPOST;
    }
    else if (!white && rank_of(sq) >= 4)
    {
      uint64_t pawn_defenders = pawn_attacks[0][sq] & my_pawns;
      if (pawn_defenders)
        mg_score += KNIGHT_OUTPOST;
    }

    knights &= knights - 1;
  }

  // Bishops
  uint64_t bishops = white ? white_bishops(position) : black_bishops(position);
  int bishop_count = bitcount(bishops);
  if (bishop_count >= 2)
  {
    mg_score += BISHOP_PAIR_BONUS;
    eg_score += BISHOP_PAIR_BONUS;
  }

  while (bishops)
  {
    int sq = bitscan(bishops);
    mg_score += BISHOP_MG + (white ? white_bishop_pst[sq] : black_bishop_pst[sq]);
    eg_score += BISHOP_EG + (white ? white_bishop_pst[sq] : black_bishop_pst[sq]);

    // Mobility
    uint64_t attacks = bishop_attacks(occupancy, sq) & ~my_pieces;
    mobility += bitcount(attacks) * BISHOP_MOBILITY;

    bishops &= bishops - 1;
  }

  // Rooks
  uint64_t rooks = white ? white_rooks(position) : black_rooks(position);
  while (rooks)
  {
    int sq = bitscan(rooks);
    mg_score += ROOK_MG + rook_pst[sq];
    eg_score += ROOK_EG + rook_pst[sq];

    // Mobility
    uint64_t attacks = rook_attacks(occupancy, sq) & ~my_pieces;
    mobility += bitcount(attacks) * ROOK_MOBILITY;

    // Open/semi-open file
    int file = file_of(sq);
    uint64_t file_mask = FILE_A << file;
    uint64_t all_pawns = my_pawns | their_pawns;

    if (!(all_pawns & file_mask))
    {
      mg_score += ROOK_OPEN_FILE;
    }
    else if (!(my_pawns & file_mask))
    {
      mg_score += ROOK_SEMI_OPEN;
    }

    // Rook on 7th rank
    if ((white && rank_of(sq) == 1) || (!white && rank_of(sq) == 6))
    {
      mg_score += ROOK_ON_7TH;
      eg_score += ROOK_ON_7TH;
    }

    rooks &= rooks - 1;
  }

  // Queens
  uint64_t queens = white ? white_queens(position) : black_queens(position);
  while (queens)
  {
    int sq = bitscan(queens);
    mg_score += QUEEN_MG + queen_pst[sq];
    eg_score += QUEEN_EG + queen_pst[sq];

    // Mobility (limited weight for queen)
    uint64_t attacks = queen_attacks(occupancy, sq) & ~my_pieces;
    mobility += bitcount(attacks) * QUEEN_MOBILITY;

    queens &= queens - 1;
  }

  // King
  int king_sq = bitscan(white ? white_kings(position) : black_kings(position));
  mg_score += white ? white_king_mg_pst[king_sq] : black_king_mg_pst[king_sq];
  eg_score += king_eg_pst[king_sq];

  // King safety (midgame only)
  int king_file = file_of(king_sq);
  uint64_t king_file_mask = FILE_A << king_file;

  // Penalty for open file near king
  if (!(my_pawns & king_file_mask))
  {
    mg_score -= KING_OPEN_FILE_PENALTY;
  }

  // Pawn shield bonus
  uint64_t shield_zone = king_moves[king_sq] & my_pawns;
  mg_score += bitcount(shield_zone) * KING_PAWN_SHIELD_BONUS;
}

int_fast32_t evaluate_position(Position* position)
{
  int phase = calculate_phase(position);

  int white_mg = 0, white_eg = 0, white_mobility = 0;
  int black_mg = 0, black_eg = 0, black_mobility = 0;

  // Evaluate pieces
  evaluate_pieces(position, true, white_mg, white_eg, white_mobility);
  evaluate_pieces(position, false, black_mg, black_eg, black_mobility);

  // Evaluate pawns
  white_mg += evaluate_pawns_for_side(white_pawns(position), black_pawns(position), true);
  black_mg += evaluate_pawns_for_side(black_pawns(position), white_pawns(position), false);

  // King safety (midgame - attacks on king zone)
  int white_king_sq = bitscan(white_kings(position));
  int black_king_sq = bitscan(black_kings(position));

  int attacks_on_white_king = count_king_attackers(position, white_king_sq, false);
  int attacks_on_black_king = count_king_attackers(position, black_king_sq, true);

  white_mg -= attacks_on_white_king * attacks_on_white_king * KING_ATTACK_WEIGHT;
  black_mg -= attacks_on_black_king * attacks_on_black_king * KING_ATTACK_WEIGHT;

  // Combine scores
  int mg_score = white_mg - black_mg + white_mobility - black_mobility;
  int eg_score = white_eg - black_eg;

  // Tapered evaluation
  int score = tapered_eval(mg_score, eg_score, phase);

  // Return from side to move's perspective
  return position->white_to_move ? score : -score;
}
