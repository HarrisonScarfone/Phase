#include "manager.hpp"

#include <iostream>

#include "../search/search.hpp"
#include "../util/initializers.hpp"
#include "moves.hpp"

void Manager::update_to_position(std::vector<std::string> tokens)
{
  std::vector<uint32_t> possible_moves;
  int from_square, to_square, move_to_square, move_from_square;
  PieceAsInt promotion_peice, move_promotion_piece;

  tokens.erase(tokens.begin());

  if (tokens.at(0) == "startpos")
  {
    position = Util::Initializers::starting_position();
  }
  else if (tokens.at(0) == "fen")
  {
    position = Util::Initializers::fen_string_to_position(tokens.at(1));
    tokens.erase(tokens.begin());
  }

  tokens.erase(tokens.begin());
  tokens.erase(tokens.begin());

  for (std::string token : tokens)
  {
    from_square = string_to_int(token.substr(0, 2));
    to_square = string_to_int(token.substr(2, 2));

    if (token.length() > 4)
    {
      switch (token[4])
      {
        case 'q':
          promotion_peice = QUEEN;
          break;
        case 'r':
          promotion_peice = ROOK;
          break;
        case 'b':
          promotion_peice = BISHOP;
          break;
        case 'n':
          promotion_peice = KNIGHT;
          break;
      }
    }
    else
    {
      promotion_peice = NO_PIECE;
    }

    possible_moves = valid_moves_for_position(position);

    for (uint32_t possible_move : possible_moves)
    {
      move_from_square = decode_from_square(possible_move);
      move_to_square = decode_to_square(possible_move);
      move_promotion_piece = decode_promoted_to_piece(possible_move);

      if (move_from_square == from_square && move_to_square == to_square && promotion_peice == move_promotion_piece)
      {
        position = make_move(&position, possible_move);
        break;
      }
    }
  }
}

std::string Manager::get_next_move()
{
  uint32_t move = find_move(&position, 5);
  std::string move_string = "";
  move_string += int_to_string(decode_from_square(move));
  move_string += int_to_string(decode_to_square(move));

  switch (decode_promoted_to_piece(move))
  {
    case QUEEN:
      move_string += "q";
      break;
    case ROOK:
      move_string += "r";
      break;
    case BISHOP:
      move_string += "b";
      break;
    case KNIGHT:
      move_string += "n";
      break;
  }

  return move_string;
}