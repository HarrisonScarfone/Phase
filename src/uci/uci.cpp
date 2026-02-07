#include "uci.hpp"

#include <iostream>
#include <random>
#include <sstream>
#include <vector>

#include "../book/book.hpp"
#include "../game/manager.hpp"
#include "../search/search.hpp"
#include "../search/transposition.hpp"
#include "../util/util.hpp"
#include "time_manager.hpp"

UCI::UCI() : manager() {};

UCI::~UCI() { wait_for_search(); }

void UCI::wait_for_search()
{
  if (search_thread.joinable())
  {
    search_thread.join();
  }
  searching.store(false, std::memory_order_relaxed);
}

void UCI::start()
{
  std::string request;

  std::cout.setf(std::ios::unitbuf);

  while (getline(std::cin, request))
  {
    if (request == "uci")
    {
      initiate();
    }
    else if (request == "ucinewgame")
    {
      wait_for_search();
      new_game();
    }
    else if (request.substr(0, 9) == "setoption")
    {
      wait_for_search();
      set_option(request);
    }
    else if (request == "isready")
    {
      wait_for_search();
      send_ready();
    }
    else if (request.substr(0, 8) == "position")
    {
      wait_for_search();
      update_position(request);
    }
    else if (request.substr(0, 2) == "go")
    {
      wait_for_search();  // Wait for any previous search
      go(request);
    }
    else if (request == "stop")
    {
      stop_search();
      wait_for_search();
    }
    else if (request == "quit")
    {
      stop_search();
      wait_for_search();
      break;
    }
  }
}

void UCI::initiate()
{
  std::cout << "id name Phase\n";
  std::cout << "id author Harrison Scarfone\n";

  // Advertise options
  std::cout << "option name Hash type spin default 64 min 1 max 4096\n";
  std::cout << "option name Threads type spin default 1 min 1 max 64\n";
  std::cout << "option name OwnBook type check default false\n";

  std::cout << "uciok\n";
}

void UCI::send_ready() { std::cout << "readyok\n"; }

void UCI::new_game()
{
  manager.new_game();
  TT.clear();
  search_context.clear();
}

void UCI::update_position(const std::string& request)
{
  std::vector<std::string> tokens = Util::tokenize_string_by_whitespace(request);
  manager.update_to_position(tokens);
}

void UCI::set_option(const std::string& request)
{
  // Parse: "setoption name <name> value <value>"
  std::istringstream iss(request);
  std::string token, name, value;

  // Skip "setoption"
  iss >> token;

  // Parse "name <name>"
  while (iss >> token)
  {
    if (token == "name")
    {
      while (iss >> token && token != "value")
      {
        if (!name.empty())
          name += " ";
        name += token;
      }
    }
    if (token == "value")
    {
      while (iss >> token)
      {
        if (!value.empty())
          value += " ";
        value += token;
      }
    }
  }

  // Handle known options
  if (name == "Hash")
  {
    try
    {
      int mb = std::stoi(value);
      mb = std::max(1, std::min(mb, 4096));  // Clamp to valid range
      TT.resize(static_cast<size_t>(mb));
    }
    catch (...)
    {
      // Invalid value, ignore
    }
  }
  else if (name == "Threads")
  {
    try
    {
      int threads = std::stoi(value);
      set_thread_count(threads);
    }
    catch (...)
    {
      // Invalid value, ignore
    }
  }
  else if (name == "OwnBook")
  {
    book::set_enabled(value == "true");
  }
}

TimeControl UCI::parse_go_command(const std::string& request)
{
  TimeControl tc;
  tc.clear();

  std::istringstream iss(request);
  std::string token;

  // Skip "go"
  iss >> token;

  while (iss >> token)
  {
    if (token == "wtime")
    {
      iss >> tc.wtime;
    }
    else if (token == "btime")
    {
      iss >> tc.btime;
    }
    else if (token == "winc")
    {
      iss >> tc.winc;
    }
    else if (token == "binc")
    {
      iss >> tc.binc;
    }
    else if (token == "movestogo")
    {
      iss >> tc.movestogo;
    }
    else if (token == "depth")
    {
      iss >> tc.depth;
    }
    else if (token == "movetime")
    {
      iss >> tc.movetime;
    }
    else if (token == "nodes")
    {
      iss >> tc.nodes;
    }
    else if (token == "infinite")
    {
      tc.infinite = true;
    }
  }

  return tc;
}

void UCI::go(const std::string& request)
{
  // Parse time control from go command
  TimeControl tc = parse_go_command(request);

  // Get current position info
  const Position& pos = manager.get_position();
  bool white_to_move = pos.white_to_move;
  int ply = pos.full_move_clock * 2 + (white_to_move ? 0 : 1);

  // Check opening book first (unless analyzing with infinite/fixed depth)
  if (!tc.infinite && tc.depth == 0)
  {
    std::string book_move = book::probe(pos.hash);
    if (!book_move.empty())
    {
      std::cout << "bestmove " << book_move << "\n";
      return;
    }

    // If book is disabled but at starting position, play e4 (60%) or d4 (40%)
    if (!book::is_enabled() && pos.full_move_clock == 1 && white_to_move)
    {
      static std::mt19937 rng(std::random_device{}());
      static std::uniform_int_distribution<int> dist(0, 99);
      std::cout << "bestmove " << (dist(rng) < 60 ? "e2e4" : "d2d4") << "\n";
      return;
    }
  }

  // Calculate time allocation
  int time_ms;
  if (tc.depth > 0 || tc.infinite)
  {
    // For fixed depth or infinite, use very long time
    time_ms = 1000000000;
  }
  else
  {
    time_ms = TimeManager::allocate_time(tc, white_to_move, ply);
  }

  // Set the search time limit
  set_search_time_limit(time_ms);

  // Set up search context with game history
  search_context.game_history = manager.get_position_hashes();

  // Copy position for the search thread
  Position position = manager.get_position();

  // Run search on separate thread so UCI can process "stop"
  searching.store(true, std::memory_order_relaxed);
  search_thread = std::thread(
      [this, position]() mutable
      {
        find_move_return_val result = find_move(&position);

        // Output best move
        std::string move_string = uint_move_to_engine_string_move(result.best_move);
        std::cout << "bestmove " << move_string << "\n";

        searching.store(false, std::memory_order_relaxed);
      });
}
