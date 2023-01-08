#ifndef INITIALIZERS_H
#define INITIALIZERS_H

#include <string>
#include "../game/types.hpp"

namespace Util
{
  namespace Initializers
  {
    Game::Position fen_string_to_position(std::string fen_string);
    Game::Position starting_position();

    void update_position_from_fen_token_0(Game::Position *position, std::string token);
    void update_position_from_fen_token_1(Game::Position *position, std::string token);
    void update_position_from_fen_token_2(Game::Position *position, std::string token);
    void update_position_from_fen_token_3(Game::Position *position, std::string token);
    void update_position_from_fen_token_4(Game::Position *position, std::string token);
    void update_position_from_fen_token_5(Game::Position *position, std::string token);
  }
}

#endif