#include <gtest/gtest.h>

#include "../../src/game/moves.hpp"
#include "../../src/util/initializers.hpp"
#include "../../src/util/zobrist.hpp"

class ZobristTest : public ::testing::Test
{
protected:
  void SetUp() override { zobrist::init(); }
};

TEST_F(ZobristTest, HashConsistencyAfterMove)
{
  // Get starting position
  Position pos = Util::Initializers::starting_position();

  // Compute hash from scratch
  uint64_t computed_hash = zobrist::compute_hash(pos);
  EXPECT_EQ(pos.hash, computed_hash) << "Initial hash should match computed hash";

  // Get all legal moves
  std::vector<uint32_t> moves = valid_moves_for_position(pos);
  ASSERT_FALSE(moves.empty()) << "Starting position should have legal moves";

  // Make each move and verify hash consistency
  for (uint32_t move : moves)
  {
    Position new_pos = make_move(&pos, move);

    // Compute hash from scratch for new position
    uint64_t expected_hash = zobrist::compute_hash(new_pos);

    EXPECT_EQ(new_pos.hash, expected_hash)
        << "Incremental hash should match computed hash after move from "
        << square_names[decode_from_square(move)] << " to " << square_names[decode_to_square(move)];
  }
}

TEST_F(ZobristTest, HashConsistencyMultipleMoves)
{
  Position pos = Util::Initializers::starting_position();

  // Play a few moves: e4 e5 Nf3 Nc6 Bb5
  std::vector<std::string> move_strings = {"e2e4", "e7e5", "g1f3", "b8c6", "f1b5"};

  for (const std::string& move_str : move_strings)
  {
    int from_sq = string_to_int(move_str.substr(0, 2));
    int to_sq = string_to_int(move_str.substr(2, 2));

    std::vector<uint32_t> moves = valid_moves_for_position(pos);
    bool found = false;

    for (uint32_t move : moves)
    {
      if (decode_from_square(move) == from_sq && decode_to_square(move) == to_sq)
      {
        pos = make_move(&pos, move);
        found = true;
        break;
      }
    }

    ASSERT_TRUE(found) << "Move " << move_str << " should be legal";

    uint64_t expected_hash = zobrist::compute_hash(pos);
    EXPECT_EQ(pos.hash, expected_hash) << "Hash should be consistent after " << move_str;
  }
}

TEST_F(ZobristTest, DifferentPositionsDifferentHashes)
{
  Position pos1 = Util::Initializers::starting_position();

  // Make e4
  std::vector<uint32_t> moves = valid_moves_for_position(pos1);
  Position pos2 = pos1;

  for (uint32_t move : moves)
  {
    if (decode_from_square(move) == string_to_int("e2") && decode_to_square(move) == string_to_int("e4"))
    {
      pos2 = make_move(&pos1, move);
      break;
    }
  }

  EXPECT_NE(pos1.hash, pos2.hash) << "Different positions should have different hashes";
}

TEST_F(ZobristTest, FenPositionHash)
{
  // Test with a known FEN position
  std::string fen = "r1bqkbnr/pppp1ppp/2n5/4p3/4P3/5N2/PPPP1PPP/RNBQKB1R w KQkq - 2 3";
  Position pos = Util::Initializers::fen_string_to_position(fen);

  uint64_t computed_hash = zobrist::compute_hash(pos);
  EXPECT_EQ(pos.hash, computed_hash) << "FEN-parsed position hash should match computed hash";
}
