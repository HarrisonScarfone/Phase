#include "util/cli.hpp"
#include "util/initializers.hpp"

int main(int argc, char *argv[])
{
  Util::Initializers::fen_string_to_position("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
  return 0;
}
