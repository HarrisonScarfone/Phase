#include "../../../src/util/tables.hpp"

#include <gtest/gtest.h>

TEST(PawnAttacks, WhitePawnOnA2) { EXPECT_EQ(pawn_attacks[1][48], (1ULL << 41)); }

TEST(PawnAttacks, BlackPawnOnH7) { EXPECT_EQ(pawn_attacks[0][15], (1ULL << 22)); }

TEST(PawnAttacks, WhitePawnOnA1) { EXPECT_EQ(pawn_attacks[1][0], 0ull); }

TEST(PawnAttacks, BlackPawnOnH8) { EXPECT_EQ(pawn_attacks[0][63], 0ull); }

TEST(PawnAttacks, WhitePawnOnB2) { EXPECT_EQ(pawn_attacks[1][49], (1ull << 40) | (1ull << 42)); }

TEST(PawnAttacks, BlackPawnOnG7) { EXPECT_EQ(pawn_attacks[0][14], (1ull << 23) | (1ull << 21)); }

TEST(PawnAttacks, FirstRank)
{
  EXPECT_EQ(pawn_attacks[1][0], 0ull);
  EXPECT_EQ(pawn_attacks[0][7], 0ull);
}

TEST(PawnAttacks, LastRank)
{
  EXPECT_EQ(pawn_attacks[1][63], 0ull);
  EXPECT_EQ(pawn_attacks[0][56], 0ull);
}
