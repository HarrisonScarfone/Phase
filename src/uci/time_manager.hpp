#ifndef TIME_MANAGER_H
#define TIME_MANAGER_H

#include <cstdint>

// Time control parameters from UCI "go" command
struct TimeControl
{
  int wtime = 0;      // White time remaining (ms)
  int btime = 0;      // Black time remaining (ms)
  int winc = 0;       // White increment per move (ms)
  int binc = 0;       // Black increment per move (ms)
  int movestogo = 0;  // Moves until next time control (0 = sudden death)
  int depth = 0;      // Search to fixed depth (0 = unlimited)
  int movetime = 0;   // Search for fixed time (ms, 0 = use time control)
  int64_t nodes = 0;  // Search fixed number of nodes (0 = unlimited)
  bool infinite = false;  // Infinite search (until "stop")

  void clear()
  {
    wtime = btime = winc = binc = movestogo = depth = movetime = 0;
    nodes = 0;
    infinite = false;
  }
};

class TimeManager
{
public:
  // Calculate time to allocate for this move
  // Returns time in milliseconds
  static int allocate_time(const TimeControl& tc, bool white_to_move, int ply);

  // Check if we should extend search time (e.g., for important positions)
  static bool should_extend(int elapsed_ms, int allocated_ms, bool pv_stable);

private:
  // Safety margin to avoid flagging (ms)
  static constexpr int MOVE_OVERHEAD = 50;

  // Minimum time per move (ms)
  static constexpr int MIN_MOVE_TIME = 10;

  // Maximum fraction of remaining time to use
  static constexpr float MAX_TIME_FRACTION = 0.7f;
};

#endif
