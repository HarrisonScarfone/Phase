#include "util.hpp"

#include <iostream>
#include <bitset>

std::vector<std::string> Util::tokenize_string_by_whitespace(std::string input_string)
{
  std::string string_copy = input_string;
  std::vector<std::string> tokens;
  std::string space_delimiter = " ";

  size_t position = 0;
  while ((position = string_copy.find(space_delimiter)) != std::string::npos)
  {
    tokens.push_back(string_copy.substr(0, position));
    string_copy.erase(0, position + space_delimiter.length());
  }

  return tokens;
}

void Util::display_64_bit_int_in_binary(uint64_t number)
{
  std::cout << std::bitset<64>(number) << ' ';
}

void Util::cli_display_position(Game::Position *position)
{
  char file = 'A';
  int rank = 8;

  std::cout << "\n\n";

  for (int i = 0; i < 8; i++)
  {

    std::cout << "  ---------------------------------\n";
    std::cout << rank << " ";
    for (int j = 0; j < 8; j++)
    {
      std::cout << "| "
                << Util::get_piece_as_char_from_square(position, (i * 8) + j)
                << " ";
    }

    --rank;
    std::cout << "|\n";
  }

  std::cout << "  ---------------------------------\n";
  std::cout << "    ";

  for (int i = 0; i < 8; i++)
  {
    std::cout << file << "   ";
    file++;
  }

  std::cout << "\n\n";
}

char Util::get_piece_as_char_from_square(Game::Position *position, int square)
{
  uint64_t bitboard_square = static_cast<uint64_t>(1) << square;
  char piece;

  bool is_white;

  if (position->white & bitboard_square)
  {
    is_white = true;
  }
  else if (position->black & bitboard_square)
  {
    is_white = false;
  }
  else
  {
    return ' ';
  }

  if (position->kings & bitboard_square)
  {
    piece = 'k';
  }
  else if (position->queens & bitboard_square)
  {
    piece = 'q';
  }
  else if (position->rooks & bitboard_square)
  {
    piece = 'r';
  }
  else if (position->bishops & bitboard_square)
  {
    piece = 'b';
  }
  else if (position->knights & bitboard_square)
  {
    piece = 'n';
  }
  else if (position->pawns & bitboard_square)
  {
    piece = 'p';
  }
  else
  {
    return ' ';
  }

  return is_white ? toupper(piece) : piece;
}