#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <cstdint>

// Rank and file constants
constexpr uint64_t FILE_A = 0x0101010101010101ull;
constexpr uint64_t NOT_FILE_A = ~FILE_A;
constexpr uint64_t FILE_B = FILE_A << 1;
constexpr uint64_t NOT_FILE_B = ~FILE_B;
constexpr uint64_t FILE_G = FILE_A << 6;
constexpr uint64_t NOT_FILE_G = ~FILE_G;
constexpr uint64_t FILE_H = FILE_A << 7;
constexpr uint64_t NOT_FILE_H = ~FILE_H;
constexpr uint64_t FILE_AB = FILE_A | FILE_B;
constexpr uint64_t NOT_FILE_AB = ~FILE_AB;
constexpr uint64_t FILE_GH = FILE_G | FILE_H;
constexpr uint64_t NOT_FILE_GH = ~FILE_GH;
constexpr uint64_t RANK_1 = 0xffull;
constexpr uint64_t RANK_2 = 0xff00000000000000;
constexpr uint64_t RANK_7 = 0x00000000000000ff;
constexpr uint64_t NOT_RANK_1 = ~RANK_1;
constexpr uint64_t RANK_8 = RANK_1 << 56;
constexpr uint64_t NOT_RANK_8 = ~RANK_8;
constexpr uint64_t NOT_RANK_1_8 = ~(RANK_1 | RANK_8);

#endif