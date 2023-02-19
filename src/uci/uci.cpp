#include "uci.hpp"

#include <iostream>
#include <vector>

#include "../game/manager.hpp"
#include "../util/util.hpp"

UCI::UCI() : manager(){};

void UCI::start()
{
  std::string move;
  std::string request;

  bool whiteToPlay = true;

  std::cout.setf(std::ios::unitbuf);

  while (getline(std::cin, request))
  {
    if (request == "uci")
    {
      initiate();
    }
    else if (request == "ucinewgame")
    {
    }
    else if (request == "setoption")
    {
      // optionsHandler();
    }
    else if (request == "ucinewgame")
    {
      // newGameHandler();
    }
    else if (request == "isready")
    {
      send_ready();
    }
    else if (request.substr(0, 8) == "position")
    {
      update_position(request);
    }

    else if (request.substr(0, 2) == "go")
    {
      go();
    }
  }
}

void UCI::initiate()
{
  std::cout << "id name Phase\n";
  std::cout << "id author Harrison Scarfone\n";
  std::cout << "uciok\n";
}

void UCI::send_ready() { std::cout << "readyok\n"; }

void UCI::update_position(std::string request)
{
  std::vector<std::string> tokens = Util::tokenize_string_by_whitespace(request);
  manager.update_to_position(tokens);
}

void UCI::go()
{
  std::string next_move = manager.get_next_move();
  std::cout << "bestmove " << next_move << "\n";
}
