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

  tokens.push_back(string_copy);
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

  std::cout << "To move:\t\t\t\t" << (position->white_to_move ? "white" : "black");

  std::cout << "\nWhite can castle kingside:\t\t" << position->white_can_castle_kingside;
  std::cout << "\nWhite can castle queenside:\t\t" << position->white_can_castle_queenside;
  std::cout << "\nBlack can castle kingside:\t\t" << position->black_can_castle_kingside;
  std::cout << "\nBlack can castle queenside:\t\t" << position->black_can_castle_queenside;
  std::cout << "\nEn passant capture available on:\t" << Util::uint64_t_to_coordinate(position->enPassantTarget);
  std::cout << "\nHalf move clock:\t\t\t" << position->half_move_clock;
  std::cout << "\nFull move clock:\t\t\t" << position->full_move_clock;
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

uint64_t Util::coordinate_to_uint64_t(std::string coordinate)
{
  // Integers as char start at 0 = 48
  // lower case char start 'a' = 97
  uint64_t square = 1;
  square <<= (8 * (8 - (coordinate.at(1) - 48))) + (tolower(coordinate.at(0)) - 97 - 1);
  return square;
}

std::string Util::uint64_t_to_coordinate(uint64_t square)
{
  if (square == 0)
  {
    return "-";
  }

  int square_as_int = 0;
  char file;
  char rank;

  while (square > 0)
  {
    square_as_int++;
    square >>= 1;
  }

  // Integers as char start at 0 = 48
  // lower case char start 'a' = 97
  file = (square_as_int % 8) + 97;
  rank = (8 - (square_as_int / 8)) + 48;

  return std::string() + file + rank;
}
