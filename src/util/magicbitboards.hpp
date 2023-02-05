#ifndef MAGICBITBOARDS_H
#define MAGICBITBOARDS_H

#include <array>
#include <cstdint>
#include <iostream>
#include <string>

#include "magicgen.hpp"
#include "util.hpp"

template <typename Generator>
constexpr auto pawn_table_generator(Generator&& f)
{
  std::array<std::array<uint64_t, 64>, 2> pawn_attack_table{};
  uint64_t bitboard = 1;

  for (int i = 0; i < 64; i++)
  {
    pawn_attack_table[1][i] = f(1, bitboard);
    pawn_attack_table[0][i] = f(0, bitboard);
    bitboard <<= 1;
  }

  return pawn_attack_table;
}

constexpr auto pawn_attacks_for_bitboard(bool white, uint64_t bitboard)
{
  if (white)
  {
    return ((bitboard & NOT_FILE_H & NOT_RANK_18) >> 7) | ((bitboard & NOT_FILE_A & NOT_RANK_18) >> 9);
  }
  else
  {
    return ((bitboard & NOT_FILE_A & NOT_RANK_18) << 7) | ((bitboard & NOT_FILE_H & NOT_RANK_18) << 9);
  }
}

constexpr auto pawn_moves_for_bitboard(bool white, uint64_t bitboard)
{
  if (white)
  {
    return ((bitboard & NOT_RANK_18 & NOT_RANK_7) >> 8) | ((bitboard & RANK_2) >> 16);
  }
  else
  {
    return ((bitboard & NOT_RANK_18 & NOT_RANK_2) << 8) | ((bitboard & RANK_7) << 16);
  }
}

constexpr auto pawn_attacks = pawn_table_generator(pawn_attacks_for_bitboard);
constexpr auto pawn_moves = pawn_table_generator(pawn_moves_for_bitboard);

template <typename Generator>
constexpr auto piece_table_generator(Generator&& f)
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

constexpr auto knight_moves_for_bitboard(uint64_t bitboard)
{
  // starting from the knight position, right and up then ctrclkwise about the knight square
  return ((bitboard & NOT_FILE_GH & NOT_RANK_8) >> 6) | ((bitboard & NOT_RANK_78 & NOT_FILE_H) >> 15) |
         ((bitboard & NOT_RANK_78 & NOT_FILE_A) >> 17) | ((bitboard & NOT_FILE_AB & NOT_RANK_8) >> 10) |
         ((bitboard & NOT_FILE_AB & NOT_RANK_1) << 6) | ((bitboard & NOT_RANK_12 & NOT_FILE_A) << 15) |
         ((bitboard & NOT_RANK_12 & NOT_FILE_H) << 17) | ((bitboard & NOT_FILE_GH & NOT_RANK_1) << 10);
}

constexpr auto king_moves_for_bitboard(uint64_t bitboard)
{
  // starting from kings square, the square to the right then ctrclkwise about the king
  return ((bitboard & NOT_FILE_H) << 1) | ((bitboard & NOT_FILE_H & NOT_RANK_8) >> 7) | ((bitboard & NOT_RANK_8) >> 8) |
         ((bitboard & NOT_FILE_A & NOT_RANK_8) >> 9) | ((bitboard & NOT_FILE_A) >> 1) |
         ((bitboard & NOT_FILE_A & NOT_RANK_1) << 7) | ((bitboard & NOT_RANK_1) << 8) |
         ((bitboard & NOT_FILE_H & NOT_RANK_1) << 9);
}

constexpr auto knight_moves = piece_table_generator(knight_moves_for_bitboard);
constexpr auto king_moves = piece_table_generator(king_moves_for_bitboard);

struct MagicBB
{
  uint64_t mask;
  uint64_t magic;
};

// Easier to store the magic numbers then generate them at compile time
// See `generate_magic_numbers` in `magicgen.hpp`
constexpr std::array<uint64_t, 64> rook_magic_numbers = {
    0x8a80104000800020ULL, 0x140002000100040ULL,  0x2801880a0017001ULL,  0x100081001000420ULL,  0x200020010080420ULL,
    0x3001c0002010008ULL,  0x8480008002000100ULL, 0x2080088004402900ULL, 0x800098204000ULL,     0x2024401000200040ULL,
    0x100802000801000ULL,  0x120800800801000ULL,  0x208808088000400ULL,  0x2802200800400ULL,    0x2200800100020080ULL,
    0x801000060821100ULL,  0x80044006422000ULL,   0x100808020004000ULL,  0x12108a0010204200ULL, 0x140848010000802ULL,
    0x481828014002800ULL,  0x8094004002004100ULL, 0x4010040010010802ULL, 0x20008806104ULL,      0x100400080208000ULL,
    0x2040002120081000ULL, 0x21200680100081ULL,   0x20100080080080ULL,   0x2000a00200410ULL,    0x20080800400ULL,
    0x80088400100102ULL,   0x80004600042881ULL,   0x4040008040800020ULL, 0x440003000200801ULL,  0x4200011004500ULL,
    0x188020010100100ULL,  0x14800401802800ULL,   0x2080040080800200ULL, 0x124080204001001ULL,  0x200046502000484ULL,
    0x480400080088020ULL,  0x1000422010034000ULL, 0x30200100110040ULL,   0x100021010009ULL,     0x2002080100110004ULL,
    0x202008004008002ULL,  0x20020004010100ULL,   0x2048440040820001ULL, 0x101002200408200ULL,  0x40802000401080ULL,
    0x4008142004410100ULL, 0x2060820c0120200ULL,  0x1001004080100ULL,    0x20c020080040080ULL,  0x2935610830022400ULL,
    0x44440041009200ULL,   0x280001040802101ULL,  0x2100190040002085ULL, 0x80c0084100102001ULL, 0x4024081001000421ULL,
    0x20030a0244872ULL,    0x12001008414402ULL,   0x2006104900a0804ULL,  0x1004081002402ULL};

constexpr std::array<uint64_t, 64> bishop_magic_numbers = {
    0x40040844404084ULL,   0x2004208a004208ULL,   0x10190041080202ULL,   0x108060845042010ULL,  0x581104180800210ULL,
    0x2112080446200010ULL, 0x1080820820060210ULL, 0x3c0808410220200ULL,  0x4050404440404ULL,    0x21001420088ULL,
    0x24d0080801082102ULL, 0x1020a0a020400ULL,    0x40308200402ULL,      0x4011002100800ULL,    0x401484104104005ULL,
    0x801010402020200ULL,  0x400210c3880100ULL,   0x404022024108200ULL,  0x810018200204102ULL,  0x4002801a02003ULL,
    0x85040820080400ULL,   0x810102c808880400ULL, 0xe900410884800ULL,    0x8002020480840102ULL, 0x220200865090201ULL,
    0x2010100a02021202ULL, 0x152048408022401ULL,  0x20080002081110ULL,   0x4001001021004000ULL, 0x800040400a011002ULL,
    0xe4004081011002ULL,   0x1c004001012080ULL,   0x8004200962a00220ULL, 0x8422100208500202ULL, 0x2000402200300c08ULL,
    0x8646020080080080ULL, 0x80020a0200100808ULL, 0x2010004880111000ULL, 0x623000a080011400ULL, 0x42008c0340209202ULL,
    0x209188240001000ULL,  0x400408a884001800ULL, 0x110400a6080400ULL,   0x1840060a44020800ULL, 0x90080104000041ULL,
    0x201011000808101ULL,  0x1a2208080504f080ULL, 0x8012020600211212ULL, 0x500861011240000ULL,  0x180806108200800ULL,
    0x4000020e01040044ULL, 0x300000261044000aULL, 0x802241102020002ULL,  0x20906061210001ULL,   0x5a84841004010310ULL,
    0x4010801011c04ULL,    0xa010109502200ULL,    0x4a02012000ULL,       0x500201010098b028ULL, 0x8040002811040900ULL,
    0x28000010020204ULL,   0x6000020202d0240ULL,  0x8918844842082200ULL, 0x4010011029020020ULL};

template <std::size_t Length, bool rooks>
constexpr auto attack_table()
{
  std::array<std::array<uint64_t, Length>, 64> attack_table{};
  for (int square = 0; square < 64; square++)
  {
    std::array<uint64_t, Length> attack_table_for_square{};
    uint64_t bitboard = 1ull << square;
    for (int i = 0; i < Length; i++)
    {
      uint64_t occupancy = construct_occupancies(i, rooks ? rook_occupancy[square] : bishop_occupancy[square],
                                                 rooks ? rook_full_occupancy[square] : bishop_full_occupancy[square]);

      uint64_t magic_index = occupancy;
      magic_index *= rooks ? rook_magic_numbers[square] : bishop_magic_numbers[square];
      magic_index >>= 64 - (rooks ? rook_occupancy[square] : bishop_occupancy[square]);

      attack_table_for_square[magic_index] = rooks ? rook_attack_mask_for_bitboard(bitboard, occupancy)
                                                   : bishop_attack_mask_for_bitboard(bitboard, occupancy);
    }
    attack_table[square] = attack_table_for_square;
  }

  return attack_table;
}

const auto magic_bishop_attacks = attack_table<512, false>();
const auto magic_rook_attacks = attack_table<4096, true>();

template <bool rooks>
constexpr std::array<MagicBB, 64> generate_magic_struct_tables()
{
  std::array<MagicBB, 64> table{};
  for (int i = 0; i < 64; i++)
  {
    MagicBB magicBB;
    magicBB.magic = rooks ? rook_magic_numbers[i] : bishop_magic_numbers[i];
    magicBB.mask = rooks ? rook_full_occupancy[i] : bishop_full_occupancy[i];

    table[i] = magicBB;
  }

  return table;
}

const auto magic_bishop_table = generate_magic_struct_tables<false>();
const auto magic_rook_table = generate_magic_struct_tables<true>();

uint64_t bishop_attacks(uint64_t occupancy, int square);
uint64_t rook_attacks(uint64_t occupancy, int square);
uint64_t queen_attacks(uint64_t occupancy, int square);

#endif
