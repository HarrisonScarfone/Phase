#ifndef MANAGER_H
#define MANAGER_H

#include <cstdint>
#include <string>
#include <vector>

#include "../util/global.hpp"

class Manager
{
private:
  Position position;
  std::vector<uint64_t> position_hashes;  // History of Zobrist hashes for repetition detection
  std::vector<int> irreversible_indices;  // Indices where half_move_clock was reset

public:
  Manager() {};

  void update_to_position(std::vector<std::string> tokens);
  std::string get_next_move();
  void new_game();

  // Repetition and draw detection
  bool is_repetition(uint64_t hash, int count = 2) const;
  bool is_draw_by_fifty_moves() const;
  bool is_draw() const;

  // Get current position and history for search
  const Position& get_position() const { return position; }
  const std::vector<uint64_t>& get_position_hashes() const { return position_hashes; }

  // Add a position to history (after making a move)
  void add_to_history(const Position& pos);
};

#endif