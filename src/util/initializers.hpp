#ifndef INITIALIZERS_H
#define INITIALIZERS_H

#include <string>

#include "global.hpp"

namespace Util
{
namespace Initializers
{
Position fen_string_to_position(std::string fen_string);
Position starting_position();

void update_position_from_fen_token_0(Position *position, std::string token);
void update_position_from_fen_token_1(Position *position, std::string token);
void update_position_from_fen_token_2(Position *position, std::string token);
void update_position_from_fen_token_3(Position *position, std::string token);
void update_position_from_fen_token_4(Position *position, std::string token);
void update_position_from_fen_token_5(Position *position, std::string token);
}  // namespace Initializers
}  // namespace Util

#endif