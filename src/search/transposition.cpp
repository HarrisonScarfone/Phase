#include "transposition.hpp"

#include <algorithm>
#include <cstring>

// Global transposition table instance
TranspositionTable TT;

// Default TT size in MB
constexpr size_t DEFAULT_TT_SIZE_MB = 64;

TranspositionTable::TranspositionTable() : size_mb_(0), num_entries_(0), generation_(0) { resize(DEFAULT_TT_SIZE_MB); }

TranspositionTable::~TranspositionTable() = default;

void TranspositionTable::resize(size_t size_mb)
{
  size_mb_ = size_mb;
  num_entries_ = (size_mb * 1024 * 1024) / sizeof(TTEntry);

  // Round down to power of 2 for fast indexing
  size_t power = 1;
  while (power * 2 <= num_entries_)
  {
    power *= 2;
  }
  num_entries_ = power;

  table_.resize(num_entries_);
  clear();
}

void TranspositionTable::clear()
{
  std::fill(table_.begin(), table_.end(), TTEntry{});
  generation_ = 0;
}

void TranspositionTable::new_search() { generation_ = (generation_ + 1) & 0xFF; }

void TranspositionTable::store(uint64_t key, int depth, int score, int static_eval, TTFlag flag, uint32_t best_move)
{
  size_t index = key & (num_entries_ - 1);
  TTEntry& entry = table_[index];

  // Replacement strategy:
  // 1. Always replace if empty
  // 2. Always replace if same position
  // 3. Replace if new entry is deeper or entry is from old search
  bool should_replace = (entry.key == 0) || (entry.key == key) ||
                        (depth >= entry.depth) || (entry.age != generation_);

  if (!should_replace)
  {
    // At minimum, update the best move if we have one and the entry doesn't
    if (best_move != 0 && entry.best_move == 0 && entry.key == key)
    {
      entry.best_move = best_move;
    }
    return;
  }

  // Don't overwrite a deeper entry from the same search with a shallower one
  // unless we have an exact score
  if (entry.key == key && entry.age == generation_ && entry.depth > depth && flag != TT_EXACT)
  {
    // Still update best move if we found one
    if (best_move != 0)
    {
      entry.best_move = best_move;
    }
    return;
  }

  entry.key = key;
  entry.score = static_cast<int16_t>(score);
  entry.static_eval = static_cast<int16_t>(static_eval);
  entry.best_move = best_move;
  entry.depth = static_cast<uint8_t>(depth);
  entry.flag = static_cast<uint8_t>(flag);
  entry.age = generation_;
}

const TTEntry* TranspositionTable::probe(uint64_t key) const
{
  size_t index = key & (num_entries_ - 1);
  const TTEntry& entry = table_[index];

  if (entry.key == key)
  {
    return &entry;
  }

  return nullptr;
}

uint32_t TranspositionTable::probe_move(uint64_t key) const
{
  const TTEntry* entry = probe(key);
  return entry ? entry->best_move : 0;
}

int TranspositionTable::hashfull() const
{
  // Sample first 1000 entries to estimate fill rate
  int used = 0;
  int sample_size = std::min(static_cast<size_t>(1000), num_entries_);

  for (int i = 0; i < sample_size; i++)
  {
    if (table_[i].key != 0 && table_[i].age == generation_)
    {
      used++;
    }
  }

  return (used * 1000) / sample_size;
}
