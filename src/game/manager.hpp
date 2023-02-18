#ifndef MANAGER_H
#define MANAGER_H

#include <string>
#include <vector>

#include "../util/global.hpp"

class Manager
{
 private:
  Position position;
  std::vector<Position> game_history;

 public:
  Manager(){};
  void update_to_position(std::vector<std::string> tokens);
  void update_game_history();
  std::string get_next_move();
};

#endif