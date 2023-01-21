#include "util.hpp"

#include <bitset>
#include <iostream>
#include <map>

#include "global.hpp"

// clang-format off
const int index64[64] = {
  0,  1,  48, 2,  57, 49, 28, 3,  61, 58, 50, 42, 38, 29, 17, 4,  62, 55, 59, 36, 53, 51,
  43, 22, 45, 39, 33, 30, 24, 18, 12, 5,  63, 47, 56, 27, 60, 41, 37, 16, 54, 35, 52, 21,
  44, 32, 23, 11, 46, 26, 40, 15, 34, 20, 31, 10, 25, 14, 19, 9,  13, 8,  7,  6};
// clang-format on

// A forward bitscan
int Util::bitscan(uint64_t bitboard)
{
  const uint64_t debruijn64 = static_cast<uint64_t>(0x03f79d71b4cb0a89);
  return index64[((bitboard & -bitboard) * debruijn64) >> 58];
}

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

void Util::print_bitboard(uint64_t bitboard)
{
  for (int i = 0; i < 8; i++)
  {
    std::cout << 8 - i << " ";

    for (int j = 0; j < 8; j++)
    {
      Square sq = static_cast<Square>(i * 8 + j);
      std::cout << (bitboard & square_to_bitboard(sq) ? "X " : "- ");
    }

    std::cout << std::endl;
  }

  std::cout << "  a b c d e f g h" << std::endl;
}

void Util::cli_display_position(Position *position)
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
      std::cout << "| " << Util::get_piece_as_char_from_square(position, (i * 8) + j) << " ";
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
  std::cout << "\nEn passant capture available on:\t" << bitboard_to_string(position->enPassantTarget);
  std::cout << "\nHalf move clock:\t\t\t" << position->half_move_clock;
  std::cout << "\nFull move clock:\t\t\t" << position->full_move_clock;
  std::cout << "\n\n";
}

char Util::get_piece_as_char_from_square(Position *position, int square)
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
