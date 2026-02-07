#ifndef TRANSPOSITION_H
#define TRANSPOSITION_H

#include <cstdint>
#include <vector>

// Transposition table entry flags
enum TTFlag : uint8_t
{
  TT_NONE = 0,
  TT_EXACT = 1,       // Exact score
  TT_LOWER_BOUND = 2, // Score is lower bound (fail-high)
  TT_UPPER_BOUND = 3  // Score is upper bound (fail-low)
};

// Transposition table entry
// Packed to minimize memory usage: 16 bytes per entry
struct TTEntry
{
  uint64_t key;       // Full Zobrist key for verification
  int16_t score;      // Evaluation score
  int16_t static_eval;// Static evaluation (for pruning)
  uint32_t best_move; // Best move found
  uint8_t depth;      // Search depth
  uint8_t flag;       // TTFlag
  uint8_t age;        // Generation counter for replacement
  uint8_t padding;    // Alignment padding
};

class TranspositionTable
{
public:
  TranspositionTable();
  ~TranspositionTable();

  // Resize the table to the given size in megabytes
  void resize(size_t size_mb);

  // Clear all entries
  void clear();

  // Increment the age counter (call at start of each search)
  void new_search();

  // Store an entry in the table
  // Returns true if the entry was stored
  void store(uint64_t key, int depth, int score, int static_eval, TTFlag flag, uint32_t best_move);

  // Probe the table for an entry
  // Returns nullptr if not found or key mismatch
  const TTEntry* probe(uint64_t key) const;

  // Get the best move from a position (returns 0 if not found)
  uint32_t probe_move(uint64_t key) const;

  // Get fill rate (percentage of entries used)
  int hashfull() const;

  // Get current size in MB
  size_t size_mb() const { return size_mb_; }

private:
  std::vector<TTEntry> table_;
  size_t size_mb_;
  size_t num_entries_;
  uint8_t generation_;
};

// Global transposition table
extern TranspositionTable TT;

#endif
