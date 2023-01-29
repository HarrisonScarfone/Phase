#include "magicgen.hpp"

namespace RandGen
{
uint32_t RANDOM_STATE = 1804289383;
}

void display_bishop_magic_numbers() { generate_all_magic_numbers(generate_bishop_magic_numbers); }
void display_rook_magic_numbers() { generate_all_magic_numbers(generate_rook_magic_numbers); }
