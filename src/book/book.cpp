#include "book.hpp"

#include <random>
#include <unordered_map>
#include <vector>

#include "../game/moves.hpp"
#include "../util/initializers.hpp"
#include "../util/zobrist.hpp"

namespace book {

static bool book_enabled = false;
static std::mt19937 rng;

struct BookMove
{
  std::string move;
  int weight;
};

// Map from position hash to possible book moves
static std::unordered_map<uint64_t, std::vector<BookMove>> book_entries;

// Helper to add a book line
static void add_line(const std::vector<std::pair<std::string, int>>& moves_with_weights)
{
  Position pos = Util::Initializers::starting_position();

  for (const auto& [move_str, weight] : moves_with_weights)
  {
    // Add this move as an option from current position
    book_entries[pos.hash].push_back({move_str, weight});

    // Parse and make the move to get next position
    int from_sq = string_to_int(move_str.substr(0, 2));
    int to_sq = string_to_int(move_str.substr(2, 2));
    PieceAsInt promo = NO_PIECE;
    if (move_str.length() > 4)
    {
      switch (move_str[4])
      {
        case 'q':
          promo = QUEEN;
          break;
        case 'r':
          promo = ROOK;
          break;
        case 'b':
          promo = BISHOP;
          break;
        case 'n':
          promo = KNIGHT;
          break;
      }
    }

    // Find and make the move
    auto legal_moves = valid_moves_for_position(pos);
    for (uint32_t m : legal_moves)
    {
      if (decode_from_square(m) == from_sq && decode_to_square(m) == to_sq && decode_promoted_to_piece(m) == promo)
      {
        pos = make_move(&pos, m);
        break;
      }
    }
  }
}

void init()
{
  rng.seed(std::random_device{}());
  book_entries.clear();

  // === WHITE OPENINGS ===

  // London System (highest priority) - d4, Nf3, Bf4 setup
  // 1. d4
  add_line({{"d2d4", 40}});

  // After 1. d4 d5 - continue London
  add_line({{"d2d4", 0}, {"d7d5", 0}, {"g1f3", 35}});
  add_line({{"d2d4", 0}, {"d7d5", 0}, {"g1f3", 0}, {"g8f6", 0}, {"c1f4", 40}});
  add_line({{"d2d4", 0}, {"d7d5", 0}, {"g1f3", 0}, {"g8f6", 0}, {"c1f4", 0}, {"e7e6", 0}, {"e2e3", 35}});
  add_line({{"d2d4", 0}, {"d7d5", 0}, {"g1f3", 0}, {"g8f6", 0}, {"c1f4", 0}, {"c7c5", 0}, {"e2e3", 35}});

  // After 1. d4 Nf6 - continue London
  add_line({{"d2d4", 0}, {"g8f6", 0}, {"g1f3", 35}});
  add_line({{"d2d4", 0}, {"g8f6", 0}, {"g1f3", 0}, {"e7e6", 0}, {"c1f4", 40}});
  add_line({{"d2d4", 0}, {"g8f6", 0}, {"g1f3", 0}, {"g7g6", 0}, {"c1f4", 35}});

  // Italian Game - e4, Nf3, Bc4
  // 1. e4
  add_line({{"e2e4", 35}});

  // After 1. e4 e5 - Italian/Scotch
  add_line({{"e2e4", 0}, {"e7e5", 0}, {"g1f3", 40}});
  add_line({{"e2e4", 0}, {"e7e5", 0}, {"g1f3", 0}, {"b8c6", 0}, {"f1c4", 35}});  // Italian
  add_line({{"e2e4", 0}, {"e7e5", 0}, {"g1f3", 0}, {"b8c6", 0}, {"d2d4", 30}});  // Scotch

  // Italian main line
  add_line({{"e2e4", 0}, {"e7e5", 0}, {"g1f3", 0}, {"b8c6", 0}, {"f1c4", 0}, {"f8c5", 0}, {"c2c3", 35}});
  add_line({{"e2e4", 0}, {"e7e5", 0}, {"g1f3", 0}, {"b8c6", 0}, {"f1c4", 0}, {"g8f6", 0}, {"d2d3", 30}});

  // Scotch main line
  add_line({{"e2e4", 0}, {"e7e5", 0}, {"g1f3", 0}, {"b8c6", 0}, {"d2d4", 0}, {"e5d4", 0}, {"f3d4", 35}});
  add_line({{"e2e4", 0}, {"e7e5", 0}, {"g1f3", 0}, {"b8c6", 0}, {"d2d4", 0}, {"e5d4", 0}, {"f3d4", 0}, {"g8f6", 0}, {"d4c6", 30}});
  add_line({{"e2e4", 0}, {"e7e5", 0}, {"g1f3", 0}, {"b8c6", 0}, {"d2d4", 0}, {"e5d4", 0}, {"f3d4", 0}, {"f8c5", 0}, {"c1e3", 30}});

  // Against Sicilian (1. e4 c5)
  add_line({{"e2e4", 0}, {"c7c5", 0}, {"g1f3", 30}});
  add_line({{"e2e4", 0}, {"c7c5", 0}, {"g1f3", 0}, {"d7d6", 0}, {"d2d4", 35}});
  add_line({{"e2e4", 0}, {"c7c5", 0}, {"g1f3", 0}, {"b8c6", 0}, {"d2d4", 35}});

  // Against French (1. e4 e6)
  add_line({{"e2e4", 0}, {"e7e6", 0}, {"d2d4", 35}});
  add_line({{"e2e4", 0}, {"e7e6", 0}, {"d2d4", 0}, {"d7d5", 0}, {"b1c3", 30}});

  // Against Caro-Kann (1. e4 c6)
  add_line({{"e2e4", 0}, {"c7c6", 0}, {"d2d4", 35}});
  add_line({{"e2e4", 0}, {"c7c6", 0}, {"d2d4", 0}, {"d7d5", 0}, {"b1c3", 30}});

  // === BLACK OPENINGS ===

  // Against 1. e4 - play e5 (leads to Italian/Scotch which we know)
  add_line({{"e2e4", 0}, {"e7e5", 35}});
  add_line({{"e2e4", 0}, {"e7e5", 0}, {"g1f3", 0}, {"b8c6", 35}});
  add_line({{"e2e4", 0}, {"e7e5", 0}, {"g1f3", 0}, {"b8c6", 0}, {"f1c4", 0}, {"g8f6", 30}});
  add_line({{"e2e4", 0}, {"e7e5", 0}, {"g1f3", 0}, {"b8c6", 0}, {"f1c4", 0}, {"f8c5", 30}});

  // Against 1. d4 - King's Indian setup
  add_line({{"d2d4", 0}, {"g8f6", 35}});
  add_line({{"d2d4", 0}, {"g8f6", 0}, {"c2c4", 0}, {"g7g6", 30}});
  add_line({{"d2d4", 0}, {"g8f6", 0}, {"g1f3", 0}, {"g7g6", 25}});
  add_line({{"d2d4", 0}, {"d7d5", 30}});

  book_enabled = false;
}

std::string probe(uint64_t hash)
{
  if (!book_enabled)
    return "";

  auto it = book_entries.find(hash);
  if (it == book_entries.end())
    return "";

  const auto& moves = it->second;
  if (moves.empty())
    return "";

  // Calculate total weight
  int total_weight = 0;
  for (const auto& m : moves)
  {
    if (m.weight > 0)
      total_weight += m.weight;
  }

  if (total_weight == 0)
    return "";

  // Pick weighted random move
  std::uniform_int_distribution<int> dist(0, total_weight - 1);
  int roll = dist(rng);

  int cumulative = 0;
  for (const auto& m : moves)
  {
    if (m.weight > 0)
    {
      cumulative += m.weight;
      if (roll < cumulative)
      {
        return m.move;
      }
    }
  }

  return moves[0].move;  // Fallback
}

bool is_enabled() { return book_enabled; }

void set_enabled(bool enabled) { book_enabled = enabled; }

}  // namespace book
