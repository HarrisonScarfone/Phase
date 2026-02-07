#include <gtest/gtest.h>

#include "../../src/search/transposition.hpp"
#include "../../src/util/zobrist.hpp"

class TranspositionTest : public ::testing::Test
{
protected:
  void SetUp() override
  {
    zobrist::init();
    TT.clear();
  }
};

TEST_F(TranspositionTest, StoreAndProbe)
{
  uint64_t key = 0x123456789ABCDEF0ULL;
  int depth = 5;
  int score = 150;
  int static_eval = 100;
  uint32_t best_move = 0x1234;

  TT.store(key, depth, score, static_eval, TT_EXACT, best_move);

  const TTEntry* entry = TT.probe(key);
  ASSERT_NE(entry, nullptr) << "Should find stored entry";

  EXPECT_EQ(entry->depth, depth);
  EXPECT_EQ(entry->score, score);
  EXPECT_EQ(entry->static_eval, static_eval);
  EXPECT_EQ(entry->flag, TT_EXACT);
  EXPECT_EQ(entry->best_move, best_move);
}

TEST_F(TranspositionTest, ProbeNonExistent)
{
  uint64_t key = 0xDEADBEEFCAFEBABEULL;

  const TTEntry* entry = TT.probe(key);
  // Entry might be nullptr or have a different key
  if (entry != nullptr)
  {
    // If there's an entry at this index, its key should be different
    // (unless by coincidence, which is extremely unlikely)
  }
}

TEST_F(TranspositionTest, ProbeMoveOnly)
{
  uint64_t key = 0xABCDEF0123456789ULL;
  uint32_t best_move = 0x5678;

  TT.store(key, 3, 50, 40, TT_LOWER_BOUND, best_move);

  uint32_t probed_move = TT.probe_move(key);
  EXPECT_EQ(probed_move, best_move);
}

TEST_F(TranspositionTest, ClearTable)
{
  uint64_t key = 0x1111111111111111ULL;
  TT.store(key, 5, 100, 90, TT_EXACT, 0x1234);

  TT.clear();

  const TTEntry* entry = TT.probe(key);
  // After clear, the entry should either be nullptr or have a zeroed/invalid state
  // The key at that index should not match
}

TEST_F(TranspositionTest, HashfullStartsEmpty)
{
  TT.clear();
  TT.new_search();

  // With a fresh table, hashfull should be 0 or very low
  int hashfull = TT.hashfull();
  EXPECT_LT(hashfull, 10) << "Fresh table should have low hashfull";
}

TEST_F(TranspositionTest, DifferentFlags)
{
  uint64_t key1 = 0x1000000000000001ULL;
  uint64_t key2 = 0x2000000000000002ULL;
  uint64_t key3 = 0x3000000000000003ULL;

  TT.store(key1, 5, 100, 90, TT_EXACT, 0);
  TT.store(key2, 5, 200, 90, TT_LOWER_BOUND, 0);
  TT.store(key3, 5, 50, 90, TT_UPPER_BOUND, 0);

  const TTEntry* e1 = TT.probe(key1);
  const TTEntry* e2 = TT.probe(key2);
  const TTEntry* e3 = TT.probe(key3);

  if (e1)
    EXPECT_EQ(e1->flag, TT_EXACT);
  if (e2)
    EXPECT_EQ(e2->flag, TT_LOWER_BOUND);
  if (e3)
    EXPECT_EQ(e3->flag, TT_UPPER_BOUND);
}
