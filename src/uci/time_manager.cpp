#include "time_manager.hpp"

#include <algorithm>

int TimeManager::allocate_time(const TimeControl& tc, bool white_to_move, int ply)
{
  // Fixed move time takes precedence
  if (tc.movetime > 0)
  {
    return tc.movetime - MOVE_OVERHEAD;
  }

  // Infinite search
  if (tc.infinite)
  {
    return 1000000000;  // Effectively infinite
  }

  // Fixed depth search - use generous time
  if (tc.depth > 0)
  {
    return 1000000000;  // Effectively infinite
  }

  // Get time and increment for side to move
  int time_remaining = white_to_move ? tc.wtime : tc.btime;
  int increment = white_to_move ? tc.winc : tc.binc;

  // Safety check
  if (time_remaining <= 0)
  {
    return MIN_MOVE_TIME;
  }

  // Subtract move overhead
  time_remaining = std::max(time_remaining - MOVE_OVERHEAD, MIN_MOVE_TIME);

  int allocated;

  if (tc.movestogo > 0)
  {
    // Time control with moves to go
    // Allocate time evenly with some buffer
    allocated = time_remaining / (tc.movestogo + 2);

    // Add increment
    allocated += increment * 3 / 4;
  }
  else
  {
    // Sudden death or increment-based
    // Estimate moves remaining based on game phase (more aggressive early)
    int estimated_moves_left = std::max(25 - ply / 3, 8);

    // Base allocation
    allocated = time_remaining / estimated_moves_left;

    // Add most of the increment
    allocated += increment * 3 / 4;
  }

  // Don't use more than half remaining time (except if very low)
  int max_time = static_cast<int>(time_remaining * MAX_TIME_FRACTION);
  allocated = std::min(allocated, max_time);

  // Ensure minimum time
  allocated = std::max(allocated, MIN_MOVE_TIME);

  return allocated;
}

bool TimeManager::should_extend(int elapsed_ms, int allocated_ms, bool pv_stable)
{
  // If PV is unstable (best move changed), consider extending
  if (!pv_stable && elapsed_ms < allocated_ms * 2)
  {
    return true;
  }

  return false;
}
