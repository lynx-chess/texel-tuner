#pragma once
#include <array>

constexpr int PSQTBucketCount = 23;

constexpr int psqtIndexCount = (64 * 6 - 16) * PSQTBucketCount * 2; // PSQT but removing pawns from 1 and 8 rank

/// [friend | enemy] x [P (squares 8-55) | N | B | R | Q | K] x [bucket] x [square]
constexpr int PSQTIndex(int friendEnemy, int piece, int bucket, int square)
{
    return (friendEnemy * (psqtIndexCount / 2)) +
           (piece == 0
                ? (48 * bucket) + square - 8
                : (48 * PSQTBucketCount) + (64 * PSQTBucketCount * (piece - 1)) + (64 * bucket) + square);
}

static_assert(PSQTIndex(0, 0, 0, 8) == 0);
static_assert(PSQTIndex(0, 0, PSQTBucketCount - 1, 55) + 1 == PSQTIndex(0, 1, 0, 0));
static_assert(PSQTIndex(0, 5, PSQTBucketCount - 1, 63) + 1 == PSQTIndex(1, 0, 0, 8));
static_assert(PSQTIndex(1, 5, PSQTBucketCount - 1, 63) + 1 == psqtIndexCount);

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
