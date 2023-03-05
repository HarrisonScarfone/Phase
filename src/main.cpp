#include <iostream>

#include "search/search.hpp"
#include "uci/uci.hpp"
#include "util/global.hpp"
#include "util/initializers.hpp"
#include "util/util.hpp"

int main()
{
  UCI uci = UCI();
  uci.start();

  // std::string fen = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1";
  // Position position = Util::Initializers::fen_string_to_position(fen);

  // uint32_t move = find_move_with_statistics(&position, 1);
}
