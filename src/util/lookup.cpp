#include "lookup.hpp"

std::string Util::Lookup::uint64_t_to_coordinate(uint64_t square)
{
  return Util::Lookup::uint64_t_to_coordinate_map.find(square)->second;
}

uint64_t Util::Lookup::coordinate_to_uint64_t(std::string coordinate)
{
  return Util::Lookup::coordinate_to_uint64_t_map.find(coordinate)->second;
}
