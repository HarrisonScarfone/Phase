#include "../../../src/util/tables.hpp"

#include <gtest/gtest.h>

// Pawn attack table tests
// ------------------------------------------------------------------------------------------------
TEST(MoveTablesTest, PawnAttacks)
{
  // white on h2
  EXPECT_EQ(pawn_attacks[1][48], (1ULL << 41));
  // black on h7
  EXPECT_EQ(pawn_attacks[0][15], (1ULL << 22));
  // white on a1
  EXPECT_EQ(pawn_attacks[1][0], 0ull);
  // black on h8
  EXPECT_EQ(pawn_attacks[0][63], 0ull);
  // white on b2
  EXPECT_EQ(pawn_attacks[1][49], (1ull << 40) | (1ull << 42));
  // black on g7
  EXPECT_EQ(pawn_attacks[0][14], (1ull << 23) | (1ull << 21));
  // white on d6
  EXPECT_EQ(pawn_attacks[1][19], (5ull << 10));
  // black on d3
  EXPECT_EQ(pawn_attacks[0][43], (5ull << 50));
}

// ------------------------------------------------------------------------------------------------
// Pawn move table tests
// ------------------------------------------------------------------------------------------------
TEST(MoveTablesTest, WhitePawnMoves)
{
  constexpr auto white_pawn_moves = pawn_moves[1];
  // on a2
  EXPECT_EQ(white_pawn_moves[48], 1ull << 40 | 1ull << 32);
  // on h2
  EXPECT_EQ(white_pawn_moves[55], 1ull << 47 | 1ull << 39);
  // on d7
  EXPECT_EQ(white_pawn_moves[11], 0);
  // on b6
  EXPECT_EQ(white_pawn_moves[17], 1ull << 9);
}

TEST(MoveTablesTest, BlackPawnMoves)
{
  constexpr auto black_pawn_moves = pawn_moves[0];
  // on a7
  EXPECT_EQ(black_pawn_moves[8], 1ull << 16 | 1ull << 24);
  // on h7
  EXPECT_EQ(black_pawn_moves[15], 1ull << 23 | 1ull << 31);
  // on b2
  EXPECT_EQ(black_pawn_moves[41], 1ull << 49);
  // on b3
  EXPECT_EQ(black_pawn_moves[49], 0);
}
// ------------------------------------------------------------------------------------------------
// Knight move table tests
// ------------------------------------------------------------------------------------------------
TEST(MoveTablesTest, KnightMoves)
{
  // on a8
  EXPECT_EQ(knight_moves[0], (1ull << 10 | 1ull << 17));
  // on a7
  EXPECT_EQ(knight_moves[14], (1ull << 4 | 1ull << 20 | 1ull << 29 | 1ull << 31));
  // on b2
  EXPECT_EQ(knight_moves[49], (1ull << 32 | 1ull << 34 | 1ull << 43 | 1ull << 59));
  // on e5
  EXPECT_EQ(knight_moves[28],
            (1ull << 11 | 1ull << 13 | 1ull << 18 | 1ull << 22 | 1ull << 34 | 1ull << 38 | 1ull << 43 | 1ull << 45));
}
// ------------------------------------------------------------------------------------------------
// King move table tests
// ------------------------------------------------------------------------------------------------
TEST(MoveTablesTest, KingMoves)
{  // on a8
  EXPECT_EQ(king_moves[0], (1ull << 1 | 3ull << 8));
  // on a7
  EXPECT_EQ(king_moves[7], (1ull << 6 | 3ull << 14));
  // on b2
  EXPECT_EQ(king_moves[31], (3ull << 22 | 1ull << 30 | 3ull << 38));
  // on e5
  EXPECT_EQ(king_moves[28], (7ull << 19 | 5ull << 27 | 7ull << 35));
}
