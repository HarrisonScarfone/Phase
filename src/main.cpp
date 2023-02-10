// #include <chrono>
// #include <iostream>
// #include <vector>

// #include "../src/game/moves.hpp"
// #include "../src/util/initializers.hpp"

// #define DOUBLE_SPACE std::cout << std::endl << std::endl;

// int main(int argc, char* argv[])
// {
//   Position position = Util::Initializers::starting_position();

//   std::vector<uint32_t> moves = valid_moves_for_position(position);

//   for (uint32_t move : moves)
//   {
//     Position new_position = make_move(&position, move);
//     std::vector<uint32_t> new_moves = valid_moves_for_position(new_position);

//     Util::cli_display_position(&new_position);

//     for (uint32_t new_move : new_moves)
//     {
//       Util::display_encoded_move(new_move);
//     }

//     for (uint32_t new_move : new_moves)
//     {
//       Position newer_position = make_move(&new_position, new_move);
//       Util::cli_display_position(&newer_position);
//     }
//   }

//   DOUBLE_SPACE;
// }

#include <chrono>
#include <iostream>

#include "../src/game/moves.hpp"
#include "../src/util/initializers.hpp"

#define DOUBLE_SPACE std::cout << std::endl << std::endl;

struct DetailedPerftResults
{
  uint64_t captures = 0;
  uint64_t enpassants = 0;
  uint64_t castles = 0;
  uint64_t promotions = 0;
  uint64_t double_pushes = 0;
};

void display_perft_results(auto duration, int depth, uint64_t total_nodes, DetailedPerftResults detailed_perft_results)
{
  std::cout << "Perft of depth : " << depth << " took: " << duration.count() * (0.000000001) << " seconds."
            << std::endl;
  std::cout << "Found " << total_nodes << " total nodes." << std::endl << std::endl;

  if (total_nodes > 0)
  {
    std::cout << "Averaged " << (duration.count() / total_nodes) << " ns per move." << std::endl;
    std::cout << "Generating and classifying at " << 1.0 / ((duration.count() / total_nodes) * (0.000000001))
              << " valid moves/second." << std::endl;
    DOUBLE_SPACE
    std::cout << "Captures: " << detailed_perft_results.captures << std::endl;
    std::cout << "Enpassants: " << detailed_perft_results.enpassants << std::endl;
    std::cout << "Castles: " << detailed_perft_results.castles << std::endl;
    std::cout << "Promotions: " << detailed_perft_results.promotions << std::endl;
    std::cout << "Double pushes: " << detailed_perft_results.double_pushes << std::endl;
  }
  else
  {
    std::cout << std::endl << "No valid moves generated." << std::endl << std::endl;
  }
}

uint64_t perft(Position position, int depth, DetailedPerftResults* detailed_perft_results)
{
  if (depth == 0)
  {
    return 1ull;
  }

  Position new_position;
  std::vector<uint32_t> moves;
  int moves_from_iteration;
  uint64_t nodes = 0;

  moves = valid_moves_for_position(position);

  for (uint32_t move : moves)
  {
    new_position = make_move(&position, move);

    detailed_perft_results->captures += decode_capture(move);
    detailed_perft_results->enpassants += decode_enpassant(move);
    detailed_perft_results->castles += decode_castling(move);
    detailed_perft_results->promotions += decode_promoted_to_piece(move) == NO_PIECE ? 0 : 1;
    detailed_perft_results->double_pushes += decode_double_push(move);

    nodes += perft(new_position, depth - 1, detailed_perft_results);
  }

  return nodes;
}

int main(int argc, char* argv[])
{
  // Position position = Util::Initializers::starting_position();
  std::string fen = "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1";
  Position position = Util::Initializers::fen_string_to_position(fen);

  Util::cli_display_position(&position);

  int move_count = 0;

  int depth = 3;

  DetailedPerftResults detailed_perft_results;

  auto start = std::chrono::high_resolution_clock::now();
  uint64_t found_nodes = perft(position, depth, &detailed_perft_results);
  auto stop = std::chrono::high_resolution_clock::now();
  auto duration = duration_cast<std::chrono::nanoseconds>(stop - start);

  DOUBLE_SPACE;

  display_perft_results(duration, depth, found_nodes, detailed_perft_results);
}