#include "manager.hpp"

#include <iostream>

#include "../evaluator/evaluator.hpp"
#include "../search/search.hpp"
#include "../util/global.hpp"
#include "../util/initializers.hpp"
#include "../util/util.hpp"
#include "moves.hpp"

void play_game()
{
  Position position = Util::Initializers::starting_position();

  while (true)
  {
    Util::cli_display_position(&position);
    uint32_t best_move = find_move(&position, 5);
    std::cout << std::endl << "Score: " << evaluate_position(&position) << std::endl << std::endl;
    Util::display_encoded_move(best_move);
    std::cout << std::endl;
    position = make_move(&position, best_move);
    std::cout << std::endl << "Enter to continue..." << std::endl << std::endl;
    std::getchar();
  }
}