#pragma once
#include <array>

constexpr int PSQTBucketCount = 23;

constexpr int psqtIndexCount = (64 * 6 - 16) * PSQTBucketCount * 2; // PSQT but removing pawns from 1 and 8 rank

constexpr static std::array<int, 64> PSQTBucketLayout = {
    15, 16, 17, 18, 19, 20, 21, 22,
    15, 16, 17, 18, 19, 20, 21, 22,
    15, 16, 17, 18, 19, 20, 21, 22,
    15, 16, 17, 18, 19, 20, 21, 22,
    15, 8, 9, 10, 11, 12, 13, 14,
    15, 8, 9, 10, 11, 12, 13, 14,
    0, 8, 9, 10, 11, 12, 13, 14,
    0, 1, 2, 3, 4, 5, 6, 7, //
};

constexpr static std::array<int, 64> PawnBucketLayout = {
    0, 0, 0, 0, 0, 0, 0, 0,
    9, 9, 10, 10, 10, 10, 9, 9,
    7, 7, 8, 8, 8, 8, 7, 7,
    5, 5, 6, 6, 6, 6, 5, 5,
    3, 3, 4, 4, 4, 4, 3, 3,
    1, 1, 2, 2, 2, 2, 1, 1,
    1, 1, 2, 2, 2, 2, 1, 1,
    0, 0, 0, 0, 0, 0, 0, 0, //
};

// constexpr static std::array<int, 64> PawnBucketLayout = {
//     0, 0, 0, 0, 0, 0, 0, 0,
//     5, 5, 7, 7, 7, 7, 8, 8,
//     5, 5, 6, 6, 6, 6, 8, 8,
//     4, 4, 5, 5, 5, 5, 6, 6,
//     1, 1, 2, 2, 2, 2, 3, 3,
//     1, 1, 2, 2, 2, 2, 3, 3,
//     1, 1, 2, 2, 2, 2, 3, 3,
//     0, 0, 0, 0, 0, 0, 0, 0, //
// };
