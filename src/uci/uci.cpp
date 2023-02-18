#include "uci.hpp"

void UCI::handle_request()
{
  std::string move;
  std::string token;
  Gamestate::Bitboards bitboards;

  bool whiteToPlay = true;

  std::cout.setf(std::ios::unitbuf);

  while (getline(std::cin, token))
  {
    if (token == "uci")
    {
      initiate();
    }
    else if (token == "ucinewgame")
    {
    }
    else if (token == "setoption")
    {
      optionsHandler();
    }
    else if (token == "ucinewgame")
    {
      newGameHandler();
    }
    else if (token == "isready")
    {
      sendReady();
    }
    else if (token.substr(0, 8) == "position")
    {
      State state = handlePositionToken(token.substr(9));
      bitboards = state.bitboards;
      whiteToPlay = state.whiteToPlay;
    }

    else if (token.substr(0, 2) == "go")
    {
      gr.writeTurnStart();
      gr.writeBitboards(&bitboards);
      SearchReturn sr = Search::getMove(bitboards, whiteToPlay);
      gr.writeMove(&sr.selectedMove);
      std::cout << "bestmove " << Translator::engineToUCIMove(&sr.selectedMove) << "\n";
      gr.writeTurnEnd();
    }
  }
}

void UCI::initiate()
{
  std::cout << "id name testengine\n";
  std::cout << "id author Harrison Scarfone\n";

  gm.arrayToBitboards();

  std::vector<Gamestate::Bitboards> gameHistory;
  gameHistory.push_back(gm.getBitboards());

  std::cout << "uciok\n";
}

void UCI::sendReady() { std::cout << "readyok\n"; }

void UCI::optionsHandler() { return; }

void UCI::newGameHandler() { return; }

void UCI::inputPosition() { return; }

UCI::State UCI::handlePositionToken(std::string token) {}

std::vector<std::string> UCI::vectorizeToken(std::string tokenString) {}
