#include "util.hpp"

#include <bitset>
#include <iostream>
#include <map>

#include "../game/moves.hpp"
#include "global.hpp"

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
      std::cout << "| " << Util::get_piece_as_string_from_square(position, (i * 8) + j) << " ";
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

std::string Util::get_piece_as_string_from_square(Position *position, int square)
{
  uint64_t bitboard_square = static_cast<uint64_t>(1) << square;
  std::string piece;

  bool white;

  if (position->white & bitboard_square)
  {
    white = true;
  }
  else if (position->black & bitboard_square)
  {
    white = false;
  }
  else
  {
    return " ";
  }

  if (position->kings & bitboard_square)
  {
    return white ? "♔" : "♚";
  }
  else if (position->queens & bitboard_square)
  {
    return white ? "♕" : "♛";
  }
  else if (position->rooks & bitboard_square)
  {
    return white ? "♖" : "♜";
  }
  else if (position->bishops & bitboard_square)
  {
    return white ? "♗" : "♝";
  }
  else if (position->knights & bitboard_square)
  {
    return white ? "♘" : "♞";
  }
  else if (position->pawns & bitboard_square)
  {
    return white ? "♙" : "♟︎";
  }
  else
  {
    return " ";
  }
}

void Util::display_encoded_move(uint32_t move)
{
  std::cout << std::endl << move_mask_names[0] << ": " << int_to_square_string(decode_from_square(move)) << std::endl;
  std::cout << move_mask_names[1] << ": " << int_to_square_string(decode_to_square(move)) << std::endl;
  std::cout << move_mask_names[2] << ": " << decode_whites_turn(move) << std::endl;
  std::cout << move_mask_names[3] << ": " << piece_names[decode_moved_piece(move)] << std::endl;
  std::cout << move_mask_names[4] << ": " << piece_names[decode_promoted_to_piece(move)] << std::endl;
  std::cout << move_mask_names[5] << ": " << decode_capture(move) << std::endl;
  std::cout << move_mask_names[6] << ": " << decode_double_push(move) << std::endl;
  std::cout << move_mask_names[7] << ": " << decode_enpassant(move) << std::endl;
  std::cout << move_mask_names[8] << ": " << decode_castling(move) << std::endl << std::endl;
}
