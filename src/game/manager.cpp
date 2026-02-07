#include "manager.hpp"

#include <algorithm>
#include <iostream>

#include "../search/search.hpp"
#include "../util/initializers.hpp"
#include "moves.hpp"

void Manager::new_game()
{
  position_hashes.clear();
  irreversible_indices.clear();
}

void Manager::add_to_history(const Position& pos)
{
  // Track if this is an irreversible position (half_move_clock was reset)
  if (pos.half_move_clock == 0)
  {
    irreversible_indices.push_back(static_cast<int>(position_hashes.size()));
  }
  position_hashes.push_back(pos.hash);
}

bool Manager::is_repetition(uint64_t hash, int count) const
{
  int occurrences = 0;

  // Only check back to last irreversible move
  int start_index = 0;
  if (!irreversible_indices.empty())
  {
    start_index = irreversible_indices.back();
  }

  for (int i = static_cast<int>(position_hashes.size()) - 1; i >= start_index; i--)
  {
    if (position_hashes[i] == hash)
    {
      occurrences++;
      if (occurrences >= count)
      {
        return true;
      }
    }
  }

  return false;
}

bool Manager::is_draw_by_fifty_moves() const { return position.half_move_clock >= 100; }

bool Manager::is_draw() const { return is_draw_by_fifty_moves() || is_repetition(position.hash, 3); }

void Manager::update_to_position(std::vector<std::string> tokens)
{
  std::vector<uint32_t> possible_moves;
  int from_square, to_square, move_to_square, move_from_square;
  PieceAsInt promotion_peice, move_promotion_piece;

  tokens.erase(tokens.begin());

  // Clear history when setting up a new position
  position_hashes.clear();
  irreversible_indices.clear();

  if (tokens.at(0) == "startpos")
  {
    position = Util::Initializers::starting_position();
  }
  else if (tokens.at(0) == "fen")
  {
    // FEN string is in tokens[1] through tokens[6]
    std::string fen = tokens.at(1);
    for (size_t i = 2; i <= 6 && i < tokens.size(); i++)
    {
      if (tokens.at(i) == "moves")
        break;
      fen += " " + tokens.at(i);
    }
    position = Util::Initializers::fen_string_to_position(fen);

    // Skip past FEN tokens
    while (!tokens.empty() && tokens.at(0) != "moves")
    {
      tokens.erase(tokens.begin());
    }
  }
  else
  {
    tokens.erase(tokens.begin());
  }

  // Add initial position to history
  add_to_history(position);

  // Skip "moves" token if present
  if (!tokens.empty() && tokens.at(0) == "moves")
  {
    tokens.erase(tokens.begin());
  }
  else if (tokens.size() >= 2)
  {
    tokens.erase(tokens.begin());
    if (!tokens.empty() && tokens.at(0) == "moves")
    {
      tokens.erase(tokens.begin());
    }
  }

  for (const std::string& token : tokens)
  {
    if (token.empty() || token == "moves")
      continue;

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
        default:
          promotion_peice = NO_PIECE;
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
        add_to_history(position);
        break;
      }
    }
  }
}

std::string Manager::get_next_move()
{
  uint32_t move = find_move_and_display_statistics(&position);
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