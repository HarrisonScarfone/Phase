#include "util/initializers.hpp"
#include "util/util.hpp"

#include <iostream>

int main(int argc, char *argv[])
{
  Game::Position position = Util::Initializers::starting_position();
  Util::cli_display_position(&position);
  return 0;
}
