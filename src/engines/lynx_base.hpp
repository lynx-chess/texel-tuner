#pragma once
#include <array>

constexpr int PSQTBucketCount = 24;

constexpr int psqtIndexCount = (64 * 6 - 16) * PSQTBucketCount * 2; // PSQT but removing pawns from 1 and 8 rank

constexpr static std::array<int, 64> PSQTBucketLayout = {
    23, 16, 17, 18, 19, 20, 21, 22,
    23, 16, 17, 18, 19, 20, 21, 22,
    23, 16, 17, 18, 19, 20, 21, 22,
    23, 16, 17, 18, 19, 20, 21, 22,
    15, 8, 9, 10, 11, 12, 13, 14,
    15, 8, 9, 10, 11, 12, 13, 14,
    15, 8, 9, 10, 11, 12, 13, 14,
    0, 1, 2, 3, 4, 5, 6, 7, //
};
