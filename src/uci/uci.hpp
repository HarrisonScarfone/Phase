#ifndef UCI_H
#define UCI_H

#include <atomic>
#include <string>
#include <thread>

#include "../game/manager.hpp"
#include "time_manager.hpp"

class UCI
{
public:
  Manager manager;

  UCI();
  ~UCI();
  void start();
  void initiate();
  void send_ready();
  void go(const std::string& request);
  void update_position(const std::string& request);
  void new_game();

  // Parse time control from "go" command
  TimeControl parse_go_command(const std::string& request);

  // Handle UCI setoption command
  void set_option(const std::string& request);

private:
  std::thread search_thread;
  std::atomic<bool> searching{false};

  void wait_for_search();
};

#endif
