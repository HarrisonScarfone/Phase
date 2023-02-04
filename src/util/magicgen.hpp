#ifndef MAGICGEN_H
#define MAGICGEN_H

#include <string.h>

#include <array>
#include <cstdint>
#include <iostream>

#include "constants.hpp"
#include "global.hpp"

template <typename Generator>
constexpr auto piece_table_occupancy_generator(Generator&& f)
{
  std::array<uint64_t, 64> table{};
  uint64_t bitboard = 1;

  for (int i = 0; i < 64; i++)
  {
    table[i] = f(bitboard);
    bitboard <<= 1;
  }

  return table;
}

constexpr auto bishop_full_occupancy_for_bitboard(uint64_t bitboard)
{
  uint64_t bishop_occupancy_bitboard = 0;
  for (int i = 0; i < 30; i++)
  {
    if (bitboard & diagonal_masks[i])
    {
      bishop_occupancy_bitboard |= ~bitboard & diagonal_masks[i] & NOT_RANK_18_FILE_AH;
    }
  }

  return bishop_occupancy_bitboard;
}

constexpr auto rook_full_occupancy_for_bitboard(uint64_t bitboard)
{
  uint64_t rook_occupancy_bitboard = 0;
  uint64_t edge_mask = 0;

  if (bitboard & NOT_FILE_A)
  {
    edge_mask |= FILE_A;
  }
  if (bitboard & NOT_FILE_H)
  {
    edge_mask |= FILE_H;
  }
  if (bitboard & NOT_RANK_1)
  {
    edge_mask |= RANK_1;
  }
  if (bitboard & NOT_RANK_8)
  {
    edge_mask |= RANK_8;
  }

  for (int i = 0; i < 16; i++)
  {
    if (bitboard & rank_and_file_masks[i])
    {
      rook_occupancy_bitboard |= ~bitboard & rank_and_file_masks[i] & ~edge_mask;
    }
  }

  return rook_occupancy_bitboard;
}

constexpr auto bishop_full_occupancy = piece_table_occupancy_generator(bishop_full_occupancy_for_bitboard);
constexpr auto rook_full_occupancy = piece_table_occupancy_generator(rook_full_occupancy_for_bitboard);

template <typename Generator>
constexpr auto occupancy_bits(Generator&& f)
{
  std::array<int, 64> occupancy_bits_for_square{};

  for (int i = 0; i < 64; i++)
  {
    occupancy_bits_for_square[i] = f(i);
  }

  return occupancy_bits_for_square;
}

constexpr auto bishop_occupany_bits_for_square(int square) { return bitcount(bishop_full_occupancy[square]); }
constexpr auto rook_occupany_bits_for_square(int square) { return bitcount(rook_full_occupancy[square]); }

constexpr auto bishop_occupancy = occupancy_bits(bishop_occupany_bits_for_square);
constexpr auto rook_occupancy = occupancy_bits(rook_occupany_bits_for_square);

constexpr auto bishop_attack_mask_for_bitboard(uint64_t bitboard, uint64_t blocker_bitboard)
{
  uint64_t attack_mask = 0;
  uint64_t temp_bitboard = 0;

  // down the right
  for (int i = 1; i < 8; i++)
  {
    uint64_t new_location = bitboard << (9 * i);
    if (new_location & FILE_A)
    {
      break;
    }
    temp_bitboard = bitboard << (9 * i);
    attack_mask |= temp_bitboard;
    if (temp_bitboard & blocker_bitboard)
    {
      break;
    }
  }

  // down the left
  temp_bitboard = 0;
  for (int i = 1; i < 8; i++)
  {
    uint64_t new_location = bitboard << (7 * i);
    if (new_location & FILE_H)
    {
      break;
    }
    temp_bitboard = new_location;
    attack_mask |= temp_bitboard;
    if (temp_bitboard & blocker_bitboard)
    {
      break;
    }
  }

  // up to the right
  temp_bitboard = 0;
  for (int i = 1; i < 8; i++)
  {
    uint64_t new_location = bitboard >> (7 * i);
    if (new_location & FILE_A)
    {
      break;
    }
    temp_bitboard = new_location;
    attack_mask |= temp_bitboard;
    if (temp_bitboard & blocker_bitboard)
    {
      break;
    }
  }

  // down the right
  temp_bitboard = 0;
  for (int i = 1; i < 8; i++)
  {
    uint64_t new_location = bitboard >> (9 * i);
    if (new_location & FILE_H)
    {
      break;
    }
    temp_bitboard = new_location;
    attack_mask |= temp_bitboard;
    if (temp_bitboard & blocker_bitboard)
    {
      break;
    }
  }

  return attack_mask;
}

constexpr auto rook_attack_mask_for_bitboard(uint64_t bitboard, uint64_t blocker_bitboard)
{
  uint64_t attack_mask = 0;
  uint64_t temp_bitboard = 0;

  int file = bitscan(bitboard) % 8;

  // to the right
  for (int i = 1; i < 8 - file; i++)
  {
    temp_bitboard = bitboard << i;
    attack_mask |= temp_bitboard;
    if (temp_bitboard & blocker_bitboard)
    {
      break;
    }
  }

  // to the left
  temp_bitboard = 0;
  for (int i = 1; i < 1 + file; i++)
  {
    temp_bitboard = bitboard >> i;
    attack_mask |= temp_bitboard;
    if (temp_bitboard & blocker_bitboard)
    {
      break;
    }
  }

  // up
  temp_bitboard = 0;
  for (int i = 1; i < 8; i++)
  {
    temp_bitboard = bitboard >> (8 * i);
    attack_mask |= temp_bitboard;
    if (temp_bitboard & blocker_bitboard)
    {
      break;
    }
  }

  // down
  temp_bitboard = 0;
  for (int i = 1; i < 8; i++)
  {
    temp_bitboard = bitboard << (8 * i);
    attack_mask |= temp_bitboard;
    if (temp_bitboard & blocker_bitboard)
    {
      break;
    }
  }

  return attack_mask;
}

constexpr uint64_t construct_occupancies(int occupancy_index, int occupancy_map_bits, uint64_t occupancy_map)
{
  uint64_t bitboard = 0;

  for (int i = 0; i < occupancy_map_bits; i++)
  {
    int square = bitscan(occupancy_map);
    occupancy_map ^= 1ull << square;

    if (occupancy_index & (1 << i))
    {
      bitboard |= (1ull << square);
    }
  }

  return bitboard;
}

namespace RandGen
{
extern uint32_t RANDOM_STATE;

constexpr uint32_t generate_random_32_bit_number()
{
  uint32_t random_number = RANDOM_STATE;

  // XORSHIFT 32 random
  random_number ^= random_number << 13;
  random_number ^= random_number >> 17;
  random_number ^= random_number << 5;

  RANDOM_STATE = random_number;
  return random_number;
}

constexpr uint64_t generate_random_64_bit_number()
{
  uint64_t n1 = static_cast<uint64_t>(generate_random_32_bit_number() & 0xFFFF);
  uint64_t n2 = static_cast<uint64_t>(generate_random_32_bit_number() & 0xFFFF);
  uint64_t n3 = static_cast<uint64_t>(generate_random_32_bit_number() & 0xFFFF);
  uint64_t n4 = static_cast<uint64_t>(generate_random_32_bit_number() & 0xFFFF);

  return n1 | (n2 << 16) | (n3 << 32) | (n4 << 48);
}
}  // namespace RandGen

constexpr uint64_t generate_magic_number_candidate()
{
  return RandGen::generate_random_64_bit_number() & RandGen::generate_random_64_bit_number() &
         RandGen::generate_random_64_bit_number();
}

constexpr uint64_t generate_magic_number(int square, int occupancy_bits, bool rooks)
{
  int array_entries = 1 << occupancy_bits;
  uint64_t seen_attacks[static_cast<uint16_t>(1) << occupancy_bits] = {0ull};
  uint64_t attacks[rooks ? 4096 : 512] = {0ull};
  uint64_t occupancies[rooks ? 4096 : 512] = {0ull};

  uint64_t bitboard = 1ull << square;
  uint64_t occupancy_mask = rooks ? rook_full_occupancy[square] : bishop_full_occupancy[square];

  for (int i = 0; i < array_entries; i++)
  {
    occupancies[i] = construct_occupancies(i, occupancy_bits, occupancy_mask);
    attacks[i] = rooks ? rook_attack_mask_for_bitboard(bitboard, occupancies[i])
                       : bishop_attack_mask_for_bitboard(bitboard, occupancies[i]);
  }

  for (int i = 0; i < 1ull << 30; i++)
  {
    uint64_t magic_candidate = generate_magic_number_candidate();

    if (bitcount((occupancy_mask * magic_candidate) & 0xFF00000000000000ull) < 6)
    {
      continue;
    }

    bool fail = false;
    memset(seen_attacks, 0, (1 << occupancy_bits) * sizeof(uint64_t));

    for (int index = 0; index < array_entries; index++)
    {
      if (fail) break;

      int magic_index = int((occupancies[index] * magic_candidate) >> (64 - occupancy_bits));

      if (seen_attacks[magic_index] == 0ull)
      {
        seen_attacks[magic_index] = attacks[index];
      }
      else if (seen_attacks[magic_index] != attacks[index])
      {
        fail = true;
      }
    }

    if (!fail)
    {
      return magic_candidate;
    }
  }

  throw new std::invalid_argument("Unable to successfully generate a valid magic number.");
}

template <typename Generator>
constexpr void generate_all_magic_numbers(Generator&& f)
{
  for (int i = 0; i < 64; i++)
  {
    std::cout << "0x" << std::hex << f(i) << "," << std::endl;
  }
}

constexpr auto generate_bishop_magic_numbers(int square)
{
  return generate_magic_number(square, bishop_occupancy[square], false);
}

constexpr auto generate_rook_magic_numbers(int square)
{
  return generate_magic_number(square, rook_occupancy[square], true);
}
// call to generate array numbers, just leave this for runtime as we only need to generate
// the numbers once and can save in a const array after
void display_bishop_magic_numbers();
void display_rook_magic_numbers();
void display_all_magic_numbers();

#endif
