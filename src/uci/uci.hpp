#ifndef UCI_H
#define UCI_H

#include "../game/manager.hpp"

class UCI
{
 public:
  Manager manager;

  UCI();
  void start();
  void initiate();
  void send_ready();
  void go();
  void update_position(std::string token);
};

#endif