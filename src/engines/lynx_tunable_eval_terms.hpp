
#pragma once
#include <array>

constexpr static int PSQTBucketCount = 11;

constexpr static std::array<std::array<int, 12>, PSQTBucketCount> PieceValue = {
    {
        {
            +79, +301, +314, +414, +906,   // 0
            +131, +358, +333, +634, +1182, // 0
        },
        {
            +94, +329, +340, +454, +1030,  // 0
            +129, +397, +358, +699, +1296, // 0
        },
        {
            +98, +347, +366, +474, +1078,  // 0
            +128, +418, +369, +735, +1352, // 0
        },
        {
            +103, +357, +370, +459, +984,  // 0
            +125, +412, +359, +722, +1360, // 0
        },
        {
            +105, +353, +368, +479, +1062, // 0
            +123, +428, +372, +750, +1388, // 0
        },
        {
            +114, +365, +379, +483, +1057, // 0
            +117, +420, +369, +744, +1395, // 0
        },
        {
            +104, +384, +397, +546, +1245, // 0
            +120, +452, +399, +805, +1512, // 0
        },
        {
            +96, +355, +370, +498, +1121,  // 0
            +117, +409, +365, +738, +1392, // 0
        },
    }};

constexpr static std::array<std::array<int, 64>, PSQTBucketCount> MiddleGamePawnTable = {
    {
        {
            0, 0, 0, 0, 0, 0, 0, 0,
            -62, 106, 84, -28, -4, -57, -35, -52,
            -18, 46, 69, -2, 16, -21, -50, -46,
            -29, 78, 26, 13, -1, -0, -24, -23,
            6, 69, 30, 48, 25, 12, -32, -60,
            18, 70, 60, -22, -15, -3, -69, -60,
            -19, 132, 51, -36, -53, 5, -78, -65,
            0, 0, 0, 0, 0, 0, 0, 0 //

        },
        {
            0, 0, 0, 0, 0, 0, 0, 0,
            15, 63, 47, 27, -17, -38, -58, -69,
            20, 24, 35, 9, 0, -6, -47, -52,
            13, 17, 30, 25, 17, 4, -29, -51,
            35, 24, 41, 37, 15, -6, -26, -54,
            53, 53, 33, 9, -12, -18, -52, -77,
            33, 78, 40, 10, -31, -32, -64, -72,
            0, 0, 0, 0, 0, 0, 0, 0 //

        },
        {
            0, 0, 0, 0, 0, 0, 0, 0,
            45, 57, 26, 21, -27, -14, -52, -74,
            44, 28, 15, 12, 9, -2, -39, -64,
            25, 13, 13, 29, 24, 6, -32, -53,
            32, 23, 14, 33, 20, 4, -31, -59,
            54, 38, 16, 7, 0, -11, -45, -80,
            51, 67, 8, 15, -16, -18, -56, -80,
            0, 0, 0, 0, 0, 0, 0, 0 //

        },
        {
            0, 0, 0, 0, 0, 0, 0, 0,
            -27, 3, 28, -17, -5, 12, -26, -61,
            -23, 2, 34, 18, 40, 17, -16, -43,
            -24, -2, 41, 25, 48, 22, -20, -45,
            -20, 7, 32, 25, 54, 15, -20, -32,
            -22, 5, 29, 10, 43, 17, -16, -58,
            -31, 9, 13, -9, 41, 9, -25, -57,
            0, 0, 0, 0, 0, 0, 0, 0 //

        },
        {
            0, 0, 0, 0, 0, 0, 0, 0,
            -21, -4, -2, 3, -17, 25, 18, -23,
            -15, -15, 9, 27, 7, 24, 7, -24,
            -21, -6, 17, 35, 10, 31, 2, -29,
            -22, -6, 13, 32, 14, 31, 2, -23,
            -16, -16, 11, 12, -2, 21, 2, -27,
            -22, -8, -2, -8, -24, 16, 10, -25,
            0, 0, 0, 0, 0, 0, 0, 0 //

        },
        {
            0, 0, 0, 0, 0, 0, 0, 0,
            -43, -31, -5, -20, -0, 49, 26, -15,
            -52, -37, -3, 26, 28, 31, 30, -11,
            -38, -24, 0, 14, 38, 27, 25, -7,
            -38, -26, -6, 14, 38, 27, 27, -2,
            -39, -35, -2, 13, 30, 31, 21, -11,
            -34, -33, -14, -26, 25, 31, 23, -19,
            0, 0, 0, 0, 0, 0, 0, 0 //

        },
        {
            0, 0, 0, 0, 0, 0, 0, 0,
            -31, -36, -29, -16, -4, 32, 47, 17,
            -37, -37, -18, -4, 10, 22, 34, 35,
            -30, -21, -4, 8, 23, 29, 7, 19,
            -30, -16, -5, 11, 25, 27, 7, 18,
            -36, -30, -16, -8, 5, 20, 27, 29,
            -33, -32, -34, -14, -5, 26, 38, 9,
            0, 0, 0, 0, 0, 0, 0, 0 //

        },
        {
            0, 0, 0, 0, 0, 0, 0, 0,
            -28, -39, -12, -26, -9, 29, 72, -11,
            -36, -42, -14, 5, 11, 34, 40, 7,
            -25, -30, -8, 7, 38, 40, 13, -5,
            -29, -22, -8, 12, 35, 36, 16, -10,
            -36, -32, -15, -2, 16, 31, 30, 1,
            -28, -34, -21, -8, -10, 31, 55, -14,
            0, 0, 0, 0, 0, 0, 0, 0 //

        },
    }};

constexpr static std::array<std::array<int, 64>, PSQTBucketCount> EndGamePawnTable = {
    {
        {
            0, 0, 0, 0, 0, 0, 0, 0,
            7, -15, -2, 12, -30, 4, -13, 22,
            -10, 9, 5, -10, -29, -19, -4, 26,
            28, 17, 13, 1, -20, -13, -2, 12,
            16, 21, 5, -15, -15, -8, -2, 20,
            -12, -4, -18, -13, 1, -15, 18, 10,
            5, -10, 4, -1, -0, -9, 13, 20,
            0, 0, 0, 0, 0, 0, 0, 0 //

        },
        {
            0, 0, 0, 0, 0, 0, 0, 0,
            2, -24, -10, -17, -8, 4, 16, 27,
            10, -7, -9, -18, -13, -13, 3, 10,
            38, 6, 2, -14, -13, -13, 13, 17,
            34, 5, -4, -17, -13, -4, 6, 20,
            14, -27, -9, -13, -9, -8, 4, 8,
            1, -29, -6, 6, -10, 8, 25, 32,
            0, 0, 0, 0, 0, 0, 0, 0 //

        },
        {
            0, 0, 0, 0, 0, 0, 0, 0,
            -9, -18, -10, -22, 8, 1, 17, 32,
            -5, 0, -10, -18, -14, -12, 3, 12,
            22, 20, -14, -22, -10, -9, 13, 16,
            18, 17, -14, -15, -13, -6, 12, 18,
            -0, -11, -17, -12, -10, -8, 0, 14,
            -4, -21, 3, 4, 18, 2, 17, 39,
            0, 0, 0, 0, 0, 0, 0, 0 //

        },
        {
            0, 0, 0, 0, 0, 0, 0, 0,
            20, 2, -16, -8, 1, -2, 6, 28,
            14, 3, -13, -32, -18, -12, -5, 10,
            28, 18, -4, -31, -14, -9, 11, 18,
            29, 13, -5, -23, -15, -8, 7, 11,
            18, -1, -12, -24, -18, -13, -6, 7,
            22, 1, -10, 4, -7, 1, 10, 24,
            0, 0, 0, 0, 0, 0, 0, 0 //

        },
        {
            0, 0, 0, 0, 0, 0, 0, 0,
            21, 8, 5, -8, 7, -11, -4, 7,
            8, 3, -6, -17, -13, -11, -6, -0,
            27, 16, -4, -19, -13, -9, 8, 9,
            22, 17, -3, -15, -13, -8, 7, 5,
            11, 5, -8, -16, -8, -12, -8, -0,
            21, 12, 0, -10, 7, -6, -3, 6,
            0, 0, 0, 0, 0, 0, 0, 0 //

        },
        {
            0, 0, 0, 0, 0, 0, 0, 0,
            27, 14, -0, -1, -1, -10, -10, 2,
            20, 11, -2, -19, -17, -14, -13, -4,
            33, 19, 3, -13, -13, -19, 3, 3,
            29, 21, 4, -10, -12, -16, 1, -1,
            15, 9, -6, -19, -18, -13, -14, -5,
            25, 21, 6, -9, -1, -2, -9, 4,
            0, 0, 0, 0, 0, 0, 0, 0 //

        },
        {
            0, 0, 0, 0, 0, 0, 0, 0,
            20, 20, 7, -14, 17, 8, -18, -22,
            16, 11, 1, -3, 2, -5, -17, -20,
            29, 18, -0, -16, -18, -14, -4, -7,
            27, 16, -1, -13, -16, -14, -7, -9,
            16, 9, -0, -4, 3, -3, -16, -20,
            24, 17, 15, -12, 25, 10, -16, -20,
            0, 0, 0, 0, 0, 0, 0, 0 //

        },
        {
            0, 0, 0, 0, 0, 0, 0, 0,
            13, 15, 5, 8, 17, 14, -6, -27,
            13, 5, 0, -14, 2, 1, -12, -21,
            26, 17, 1, -19, -20, -12, 4, -8,
            23, 14, -7, -17, -15, -7, 4, -10,
            10, 6, -1, -6, -4, 4, -7, -21,
            16, 11, 11, -16, 15, 20, -1, -21,
            0, 0, 0, 0, 0, 0, 0, 0 //

        },
    }};

constexpr static std::array<std::array<int, 64>, PSQTBucketCount> MiddleGameKnightTable = {
    {
        {
            127, -37, -34, 16, -118, -89, -118, -213,
            -19, -52, -7, 64, 37, -26, -7, 1,
            -15, -19, 61, 86, 55, 14, 51, 48,
            60, 52, 52, 103, 39, 33, 98, -21,
            43, 47, 56, 52, 59, 38, 23, -50,
            55, -4, 62, 55, 46, 25, -36, -84,
            44, 28, 56, 1, 31, -55, 63, -139,
            3, -47, -125, -90, -27, -141, -125, -87 //
        },
        {
            -49, -52, -18, -27, -28, -62, -46, -43,
            -49, -63, 30, 5, 8, 76, 21, -38,
            -27, 12, 38, 49, 68, 21, 11, -6,
            3, 39, 46, 91, 69, 48, 45, 8,
            -4, 42, 50, 50, 63, 42, 49, 27,
            -22, -3, 38, 56, 75, 5, 3, -43,
            -30, -36, 23, -2, 12, 13, -41, -59,
            -128, -103, -54, -32, -26, -52, -33, -61 //
        },
        {
            -110, -76, -33, -5, -55, -26, -25, -110,
            -49, -72, 2, 22, 10, 39, -26, -31,
            -50, 19, 41, 55, 58, 15, 3, -22,
            12, 36, 58, 68, 72, 53, 52, 9,
            -3, 29, 63, 45, 60, 54, 39, 28,
            -24, 1, 38, 68, 41, 14, 3, -21,
            -21, 9, 38, 14, 10, 19, -36, -27,
            -157, -77, 21, -29, -21, -21, -30, -66 //
        },
        {
            -222, -17, -116, -8, -16, -29, -23, -77,
            -47, -11, 2, 26, 24, 20, 1, -31,
            -43, 21, 47, 115, 37, 37, 19, -14,
            -16, 23, 60, 88, 88, 72, 62, 23,
            17, 5, 69, 71, 88, 51, 49, 21,
            -16, -4, 29, 64, 84, 39, 53, -11,
            -68, -43, -27, 38, 17, 29, -16, 21,
            -275, -10, 35, -25, -37, -52, -38, -254 //
        },
        {
            -191, -4, -56, -36, -25, -26, 1, -156,
            -48, -17, 1, 21, 33, 25, -26, -3,
            -23, 7, 21, 56, 49, 37, 22, -4,
            -6, 23, 41, 57, 52, 50, 30, 16,
            -12, 25, 39, 39, 47, 45, 34, 5,
            -18, 13, 20, 37, 73, 28, 21, -13,
            -33, -21, 21, 22, 27, 42, 4, -18,
            -200, -4, -32, -23, 28, -8, -7, -103 //
        },
        {
            -198, -53, -97, -57, -2, -14, -15, -88,
            -47, -70, 20, 11, 12, 48, -38, -7,
            -71, -35, 10, 52, 75, 81, 42, -26,
            -24, 11, 42, 52, 82, 88, 89, 46,
            -17, -6, 43, 68, 87, 99, 80, 34,
            -66, -1, 12, 70, 69, 75, 48, 20,
            -19, -19, -13, 14, 24, 36, -6, -12,
            -231, -46, -64, -29, 11, -36, 11, -155 //
        },
        {
            -120, -22, -47, -23, -8, 1, -11, -68,
            -42, -24, -9, 14, 13, 33, 4, -3,
            -33, -5, 5, 44, 49, 29, 25, -10,
            -16, 18, 33, 43, 43, 46, 35, 8,
            -11, 17, 34, 31, 43, 46, 37, 5,
            -36, -6, 5, 35, 40, 22, 16, -8,
            -45, -19, -7, 13, 14, 25, 6, 1,
            -95, -23, -45, -16, -5, 5, -26, -57 //
        },
        {
            -176, -54, -70, -32, -13, -22, -5, -11,
            -57, -44, -18, 6, 13, 25, -5, 23,
            -60, -22, 6, 45, 56, 36, 36, 13,
            -36, 5, 33, 56, 68, 70, 70, 39,
            -21, 23, 37, 42, 58, 63, 69, 34,
            -51, -21, 4, 49, 62, 35, 33, 15,
            -95, -46, -20, 9, 15, 14, -6, 8,
            -157, -40, -61, -24, -23, 5, -2, 20 //
        },
    }};

constexpr static std::array<std::array<int, 64>, PSQTBucketCount> EndGameKnightTable = {
    {
        {
            -147, -7, 1, -70, 34, -35, 5, -6,
            -15, -5, 49, -37, -2, 27, 1, -55,
            16, -20, 27, 35, 23, 23, -12, -25,
            -35, 0, 60, 34, 51, 60, -27, -21,
            -27, -1, 32, 39, 62, 15, 35, 15,
            -44, 26, 2, 22, 16, 13, -22, 19,
            -33, 8, -29, 16, -20, -13, -78, 29,
            -50, -26, 62, 21, 22, 61, 12, -111 //
        },
        {
            -95, -4, -14, -23, 4, -23, -6, -65,
            -3, 11, -4, 20, 5, -36, -8, -7,
            11, -9, 8, 28, 26, 10, -3, -18,
            -2, 8, 35, 31, 38, 48, 23, -18,
            16, 3, 40, 40, 41, 35, -7, -19,
            10, 20, -8, 21, 15, 12, 1, 8,
            -2, 23, -17, 19, -11, -19, 20, 9,
            -59, 43, 17, 3, -5, -0, -45, -171 //
        },
        {
            -95, 22, 12, -11, 12, -47, -11, -84,
            -13, 28, -2, -2, 9, -8, -6, -18,
            21, 3, 8, 25, 28, 16, 2, -5,
            8, 7, 31, 30, 35, 40, 5, -10,
            15, 21, 22, 37, 49, 26, 11, -16,
            -14, 24, -3, 19, 22, 20, 3, -6,
            -29, -4, -4, 15, 11, -15, 18, -12,
            -76, -6, -19, 2, -19, -17, -5, -102 //
        },
        {
            -1, -14, 17, -8, -9, -20, -10, -54,
            -16, 15, 9, -0, -14, 11, 3, -26,
            -7, 7, 4, 0, 18, 4, -6, -10,
            11, 12, 23, 13, 24, 23, -5, -5,
            -3, 27, 28, 21, 18, 21, -1, -15,
            5, 17, 12, 15, 15, -4, -22, 7,
            -1, 13, 12, 1, -6, -4, -3, -25,
            -13, -49, -14, -10, 1, -10, 2, -22 //
        },
        {
            -51, -37, 4, 16, 9, -32, -34, -73,
            1, 1, 11, -7, 0, 14, 0, -30,
            -13, 7, 20, 21, 17, 0, 2, -15,
            19, 13, 38, 33, 37, 29, 8, 10,
            10, 14, 34, 40, 35, 32, 23, -3,
            -19, 9, 15, 22, 12, 4, 7, 2,
            -8, 7, -4, -3, -7, -3, -5, -40,
            -63, -35, 8, 8, -13, -6, -48, -43 //
        },
        {
            -27, -26, -2, 17, -2, -7, -39, -53,
            -17, 17, 1, 14, 1, -7, -4, -14,
            8, 22, 30, 25, 11, -11, 1, 12,
            10, 12, 31, 37, 26, 15, -0, -22,
            10, 22, 34, 27, 22, 11, 6, -3,
            -3, 6, 17, 25, 15, -8, -6, -15,
            -30, -1, 1, 18, -6, -4, -7, -33,
            -67, -17, -11, -6, -9, -5, -55, 13 //
        },
        {
            -56, -40, -7, -11, -5, -28, -37, -84,
            -3, 5, 13, 6, 10, -20, -9, -14,
            -16, 10, 29, 29, 21, 5, 1, -13,
            14, 16, 40, 42, 48, 34, 19, -2,
            9, 22, 40, 47, 48, 33, 22, 6,
            -10, 13, 18, 41, 23, 6, -3, -12,
            -14, 14, 5, 7, 5, -15, -12, -16,
            -61, -34, -2, -11, -7, -33, -19, -110 //
        },
        {
            -36, -52, -15, -10, -13, -12, -40, -104,
            -22, 2, 6, 14, 15, 12, -9, -14,
            -11, -1, 29, 36, 36, 19, -10, -12,
            -1, 8, 39, 45, 46, 39, 21, -11,
            -18, -4, 35, 48, 54, 50, 33, 19,
            -14, 3, 22, 28, 29, 22, -6, -22,
            -5, 7, 8, 8, 14, 7, -15, -9,
            -56, -39, 1, -8, 8, -35, -24, -132 //
        },
    }};

constexpr static std::array<std::array<int, 64>, PSQTBucketCount> MiddleGameBishopTable = {
    {
        {
            48, -14, 17, 30, 0, -70, 68, 16,
            -82, 47, -8, 10, -9, -2, 13, -27,
            39, 46, 0, 26, -18, -25, 12, -20,
            109, 85, 2, 68, 33, -35, -63, -20,
            -101, 39, 16, 28, 27, -21, 20, -14,
            1, 7, -1, 21, -37, -27, -70, -1,
            67, 41, 27, 50, -22, 6, -19, -71,
            -6, 26, -1, -26, -6, -63, -23, -143 //
        },
        {
            9, -275, 2, -0, -6, -20, -28, -5,
            -29, 60, 45, -9, -1, 7, -17, -27,
            14, 12, 19, 25, 2, -10, -13, -18,
            -7, -5, 21, 41, 36, -9, -3, -5,
            -9, 41, 10, 56, 15, -10, -2, 16,
            -29, 46, 37, 18, 11, -1, -19, -14,
            -5, 94, 56, 17, -11, 2, -15, -28,
            35, 53, 21, -40, -6, -11, 1, -130 //
        },
        {
            -38, -14, 23, -25, -26, 1, -36, -30,
            3, 55, 28, -2, 0, 15, -3, 7,
            -3, 7, 31, 24, -2, -1, -0, 15,
            1, 6, 12, 34, 34, -26, -5, -16,
            -15, 6, 1, 24, 13, -6, -1, -6,
            -10, 16, 27, 7, -0, -11, -13, 7,
            -26, 55, 41, 13, -13, -12, -25, -21,
            -53, -22, 42, -21, -1, -6, -27, -33 //
        },
        {
            -61, 15, -21, -21, -12, -4, -20, -69,
            -67, -1, -16, 31, 25, -1, 13, -29,
            -47, -4, 10, 42, 44, 9, 6, 7,
            -23, -1, 38, 42, 53, 19, 26, 20,
            -11, 10, 29, 33, 31, 13, 18, -5,
            -6, -6, 9, 36, 16, 9, -10, 29,
            -38, 12, -7, 49, 13, -18, -14, 20,
            8, -35, -15, -66, -64, -4, 30, -66 //
        },
        {
            -10, -8, -10, -23, 18, -12, -66, -14,
            13, 13, 5, -8, 11, -2, 25, -2,
            -8, -1, -3, 11, -3, 8, -7, 6,
            3, 0, 14, 15, 18, -21, -2, -3,
            -17, 1, -2, 18, -1, -18, -5, 7,
            7, 10, -2, 7, 4, 10, -9, 5,
            8, 20, 11, -3, 9, 8, 24, -10,
            23, 25, -0, -29, -11, -15, -19, -14 //
        },
        {
            -48, -52, -9, -27, -3, -17, -36, -39,
            -40, -12, 8, -11, 4, 3, 24, -23,
            -49, 3, 13, 29, 19, 50, 12, 18,
            -16, -18, 15, 37, 63, 24, 16, -4,
            -30, -1, 0, 43, 41, 24, 4, 19,
            -29, 12, 23, 15, 32, 37, 1, 22,
            2, 7, 3, -3, -3, 10, 26, -11,
            -65, -3, -5, -19, -29, -3, -51, -6 //
        },
        {
            -9, 25, -9, -10, -15, 17, -22, 22,
            7, -5, 4, -22, -0, 7, 45, -9,
            -9, 1, -14, -2, -19, 3, -2, 24,
            -12, -11, -11, 20, 15, -15, 2, -2,
            -15, -5, -18, 13, 6, -12, -9, 3,
            0, -7, -4, -10, -4, -3, 5, 20,
            9, 6, 6, -10, -7, 14, 36, 13,
            14, 19, 4, -27, -19, 13, -29, 7 //
        },
        {
            -38, -2, -10, -46, 2, -11, 19, -11,
            -6, -11, -13, -19, -8, 5, 42, 25,
            -23, -10, -13, -5, -8, 21, 8, 50,
            -25, -21, -5, 33, 42, 4, 8, 3,
            -35, -25, -8, 21, 24, 5, 3, 5,
            -10, -15, 10, -7, 2, 1, 16, 29,
            -5, 2, -10, -4, -9, 2, 30, 28,
            -21, -5, 6, -47, -5, -5, 37, 12 //
        },
    }};

constexpr static std::array<std::array<int, 64>, PSQTBucketCount> EndGameBishopTable = {
    {
        {
            -54, -11, -22, 5, 43, -7, -43, -39,
            5, -36, -16, -27, -21, -3, -29, 3,
            -2, 6, 0, 1, -5, 19, -7, 28,
            -35, -30, 14, -12, -24, 14, 23, 24,
            48, -33, -3, 13, 10, 19, 2, 23,
            -1, -17, -2, -27, -1, 22, 18, -22,
            -30, -17, 5, -34, 40, -22, 30, 43,
            31, 6, 41, 24, -7, -4, 6, 78 //
        },
        {
            14, 54, 4, -12, 2, 4, 22, 18,
            17, -4, -21, 4, 1, -34, 3, -4,
            13, 2, -9, -12, -1, 8, 10, 12,
            22, 2, -14, 4, -10, 4, 4, 24,
            13, 1, -7, -17, -3, -9, 4, 2,
            37, -29, -12, -11, -3, -4, 6, 4,
            -19, -42, -23, -14, -8, -30, 6, 0,
            10, -19, -1, 18, -6, 1, -22, 51 //
        },
        {
            2, 12, -15, 8, 7, 3, 10, 22,
            24, -24, -22, -4, -5, -14, 3, -15,
            24, 0, -17, -15, -8, -12, -5, -4,
            11, -7, -10, -8, -18, 6, 9, 14,
            6, -3, -14, -4, -5, -7, -6, 12,
            46, -21, -8, -11, 3, 5, 2, -14,
            11, -20, -29, -14, 4, -2, 1, 14,
            22, 37, -2, 19, 2, 3, 11, 10 //
        },
        {
            38, 9, 12, -2, 3, 11, 1, 10,
            33, -4, 11, -23, -15, -1, -8, 0,
            29, 13, -9, -22, -24, 6, -9, 14,
            29, -6, -11, -15, -17, -8, -13, 6,
            11, 2, -16, -16, -17, -12, -10, 10,
            23, 2, -3, -31, -15, -10, -10, -9,
            12, 3, -12, -26, -23, 5, 4, 5,
            3, 10, 15, 15, 19, -1, 10, 23 //
        },
        {
            -0, 46, -1, 15, -10, -3, 12, -7,
            5, -7, -1, -7, -7, -6, -10, 3,
            14, 8, 2, -8, -8, -3, 6, 13,
            10, -1, -8, -6, -3, 1, -0, 15,
            4, 2, -10, -3, -13, 4, 5, 5,
            15, -5, -3, -14, -2, -12, 5, 8,
            -4, -8, -7, -1, -12, -4, -7, 5,
            11, -13, -3, 8, -5, -8, 5, 2 //
        },
        {
            16, 54, -2, 13, -3, 16, -2, -1,
            15, -2, -9, -1, -5, -23, -9, -5,
            26, 10, -4, -14, -14, -20, -1, 8,
            9, 6, -8, -9, -13, -16, 1, 6,
            20, 4, -3, -13, -16, -1, -6, 8,
            13, -5, -12, -8, -16, -25, 9, 9,
            -9, -7, -7, 6, -6, -14, -15, 15,
            24, 1, 14, 6, 15, 10, 4, -11 //
        },
        {
            -10, -9, -5, -1, 6, -8, 15, -40,
            -7, -3, -7, 2, 1, -16, -2, -20,
            14, 11, 8, 3, 15, 4, -2, 16,
            15, 2, 12, 9, 5, 2, -6, -6,
            7, 2, 13, 17, 2, 3, 0, 2,
            11, 7, 1, 1, 7, -2, -9, 19,
            -9, -5, -21, 0, 2, -8, 1, -22,
            -2, -17, -3, 5, 18, -6, 16, -27 //
        },
        {
            20, 5, -9, 12, -19, 11, -26, -17,
            4, -11, -1, 7, 3, -2, -12, -3,
            5, -1, 14, 1, 30, 1, 7, 6,
            17, 1, 11, 5, 1, 3, 5, 7,
            -9, 8, 5, 12, 2, 2, -1, 14,
            -5, 6, -1, 4, 8, 11, 4, 10,
            -2, -13, -18, -3, 1, -3, -15, -22,
            4, -7, -17, -8, 10, -7, -31, -17 //
        },
    }};

constexpr static std::array<std::array<int, 64>, PSQTBucketCount> MiddleGameRookTable = {
    {
        {
            -140, 20, 14, 5, 0, 21, -8, -7,
            93, 36, 36, -20, -25, 9, 15, -24,
            -42, -74, -10, -2, -79, -34, 58, -47,
            -46, 85, 12, -11, -51, 25, 46, -17,
            -89, 26, 41, 47, -3, -23, -14, 19,
            39, -4, 29, 12, -8, 37, -15, -21,
            -23, 27, 51, 5, -17, 19, 26, -7,
            -146, 23, 29, 33, 20, 19, 40, -11 //
        },
        {
            -83, -50, -5, 14, 1, 7, 20, 7,
            -34, 13, 40, -3, -11, 13, -2, -22,
            -2, 32, -28, -0, -3, -29, 8, -7,
            -25, 21, 4, 1, 11, -11, 7, 1,
            -10, 16, -11, 4, -14, -27, -11, -4,
            -30, 23, 46, -1, 8, 17, 6, 4,
            56, 92, 4, 4, 15, -17, 2, -20,
            -66, -12, 3, 20, 3, 2, 15, -1 //
        },
        {
            -71, -34, -45, 12, 8, 9, 26, -7,
            15, 11, 5, -3, 24, -1, 10, -17,
            -5, -40, -28, 9, -8, -21, 15, 0,
            -6, -11, 40, -1, 1, 2, 13, -7,
            -6, 7, 12, 3, -14, -2, 1, -7,
            49, -8, 79, -4, -19, 22, 35, -15,
            -6, 6, 12, 3, -0, 9, 4, -29,
            -56, -38, 9, 19, 14, 5, 23, 0 //
        },
        {
            -41, -37, -23, -43, 1, 14, 23, 7,
            -33, -22, -4, 6, -10, 26, -13, -18,
            -3, 7, -45, 40, -5, -2, 7, -9,
            -29, -2, -30, 21, -16, 40, 41, 35,
            8, -10, 30, 34, 13, 33, -32, 29,
            -28, 1, 5, 42, -12, 11, 20, -7,
            -5, -16, -24, 12, -19, 8, 28, 43,
            -17, -46, -24, -16, 9, 25, 23, -1 //
        },
        {
            34, -2, -2, -7, -10, -36, 3, 19,
            -20, -5, -5, -13, 32, -14, 21, -12,
            -9, 1, -25, -14, 64, -2, -1, 2,
            -35, -20, -26, -34, 47, 5, 21, 11,
            -17, -17, -35, -28, 48, 23, 5, -10,
            -18, 1, -20, -6, 37, 3, 24, 8,
            -12, -32, -8, 6, 5, -20, 7, -3,
            12, 1, 1, 5, 18, -11, 15, 49 //
        },
        {
            -25, -22, -22, -12, -2, -30, -6, -14,
            -62, -44, 9, 5, 6, 11, 10, 27,
            -43, -8, -19, -12, 1, 36, 21, 54,
            -54, -9, -21, 4, 30, 26, 39, 23,
            -57, -27, -14, 11, 6, 46, 26, 20,
            -13, -19, -34, 9, 19, 43, 51, 51,
            -44, -33, 6, 2, 13, 51, 11, 27,
            -16, -8, -13, 9, 10, -23, 4, -9 //
        },
        {
            -10, -9, -3, 4, 13, 17, -46, -43,
            -29, -21, -14, -8, 4, 19, 63, 15,
            -37, -26, -22, -14, 1, 6, 62, 31,
            -28, -34, -22, -10, -9, 7, 44, 18,
            -21, -25, -16, -9, -7, 4, 40, 18,
            -31, -23, -23, -6, -1, 23, 57, 34,
            -30, -30, -8, -6, 7, 18, 79, 56,
            -8, -6, -2, 11, 20, 20, -25, -27 //
        },
        {
            -16, -21, -14, -7, 7, 21, 27, -45,
            -48, -26, -18, -6, 5, 26, 50, 59,
            -46, -33, -27, -16, 17, 33, 75, 70,
            -46, -30, -42, -17, 1, 9, 43, 4,
            -53, -25, -21, -3, -10, 18, 43, 54,
            -39, -31, -33, 5, 3, 18, 59, 43,
            -41, -36, -19, 0, 2, 17, 46, 32,
            -12, -12, -13, 2, 14, 21, 33, -47 //
        },
    }};

constexpr static std::array<std::array<int, 64>, PSQTBucketCount> EndGameRookTable = {
    {
        {
            53, -4, 4, 6, -9, -25, 7, 13,
            -76, 17, 21, 16, 6, -19, -21, 5,
            -26, 50, 25, -3, 22, -4, -25, 4,
            5, 3, 6, 11, 11, -25, -22, -19,
            4, 26, 2, -16, 12, -5, 11, -12,
            -27, 6, -6, -5, 12, -5, -3, -2,
            1, 6, 6, 8, 20, -19, -6, 14,
            33, 6, -14, -11, -7, -24, -19, 8 //
        },
        {
            25, 35, 27, 4, -3, -16, -16, -0,
            7, 22, 1, 11, 5, -7, 6, -3,
            3, -4, 25, 1, -9, -1, -7, -3,
            3, 3, 13, 15, -24, -6, -24, -11,
            22, 3, 5, -2, 5, -2, -19, -20,
            4, 12, -19, 4, -22, -26, -20, -17,
            -5, -3, 7, 12, -7, -4, -13, 14,
            28, -7, 16, 5, 2, -12, -16, 1 //
        },
        {
            34, 17, 32, -0, -3, -20, -24, 10,
            4, 8, 28, 10, -14, 0, 1, 1,
            8, 25, 2, -11, 0, -1, -20, -18,
            5, 11, -5, -4, -7, -4, -25, -12,
            16, -0, -1, -6, 7, -18, -7, -7,
            -2, 13, -29, -5, 3, -4, -34, -3,
            14, 13, 16, -3, 0, -4, -2, 8,
            24, 23, 4, -6, -3, -14, -23, 1 //
        },
        {
            16, 21, 24, 20, -8, -14, -24, -11,
            17, 25, 15, 1, -2, -19, -1, 1,
            9, 2, 22, -20, -5, -12, -17, -8,
            16, 0, 12, -1, 3, -24, -12, -20,
            8, 15, -3, -2, -7, -13, -6, -4,
            11, 5, -8, -21, -14, 3, -6, -0,
            13, 20, 18, -1, 7, -4, -12, -15,
            4, 33, 20, 11, -4, -17, -25, -13 //
        },
        {
            -8, 4, 12, 3, -7, 6, -3, -22,
            13, 13, 14, 3, -9, 5, -2, 1,
            11, 1, 15, -3, -29, -8, -7, -4,
            20, 9, 17, 5, -7, 2, -2, -13,
            13, 13, 26, 8, -14, -26, 2, 6,
            18, 11, 7, -14, -25, -12, -11, -2,
            16, 22, 10, -9, -2, 9, -1, -0,
            -5, -2, 4, -7, -8, -11, -5, -44 //
        },
        {
            9, 7, 10, 10, -2, 14, 7, -11,
            30, 25, 11, 1, -6, 3, 1, -8,
            14, 4, 5, 6, -14, -26, -11, -24,
            24, 0, 10, -5, -14, -13, -10, -13,
            24, 10, 14, 0, -7, -17, -13, -18,
            7, 9, 5, -4, -20, -27, -19, -13,
            25, 20, 6, 1, -12, -15, 1, -2,
            7, -5, 8, -0, -10, 12, 5, -7 //
        },
        {
            13, -0, 5, -3, -7, -3, 34, 7,
            22, 21, 20, 11, -1, -10, -20, -1,
            15, 9, 12, 7, -6, -10, -31, -25,
            15, 16, 11, 1, -6, -6, -24, -23,
            13, 8, 8, 1, -7, -4, -21, -21,
            16, 14, 9, -0, -11, -19, -34, -21,
            21, 24, 18, 11, -2, -11, -14, -20,
            9, -3, 5, -8, -17, -9, 12, 4 //
        },
        {
            11, 7, 7, 3, -4, 5, -0, 7,
            22, 20, 19, 10, 8, 4, -14, -20,
            15, 13, 8, 9, -10, -9, -30, -49,
            18, 7, 16, 15, -4, 3, -21, -19,
            11, 2, 8, -6, -2, -11, -17, -45,
            12, 10, 6, -6, 0, -10, -22, -19,
            24, 24, 20, 4, 2, 3, -12, -23,
            8, 8, 3, -1, -11, -1, -9, 2 //
        },
    }};

constexpr static std::array<std::array<int, 64>, PSQTBucketCount> MiddleGameQueenTable = {
    {
        {
            42, 24, -45, 40, -67, 27, 69, -24,
            -50, 11, 25, 13, -7, -31, 9, -25,
            -15, -13, 3, -12, 20, -25, 23, 37,
            -15, 35, 15, 52, 21, 17, -41, -11,
            -10, 14, -52, -20, 31, -14, -32, 14,
            -29, 29, 3, -3, -10, 32, 39, -20,
            8, 41, -11, 12, -3, 22, 14, 33,
            18, -61, -79, -40, 24, 22, -69, -5 //
        },
        {
            -61, 8, -4, -3, 7, -26, 53, 62,
            -26, 8, 21, 15, 15, 3, -14, 23,
            -49, 9, -1, -13, -4, -1, 33, 28,
            1, -37, -17, 3, -6, -19, 5, 36,
            3, -7, 0, 8, 1, 5, -3, 0,
            -45, -0, -24, -14, 2, -6, 17, 4,
            -28, -43, 21, 26, 14, 3, 8, 39,
            12, 9, 9, -8, -10, 2, -15, -30 //
        },
        {
            -51, -86, -28, -11, -9, 14, -14, 46,
            -2, -28, 30, 25, 4, 13, 40, 49,
            -42, 0, -8, 18, -14, 8, 18, 93,
            3, -41, -2, 5, 6, 13, 23, 33,
            -7, -21, -5, 12, 8, 9, 11, 38,
            -23, -19, 2, 6, 18, -10, 31, 58,
            -48, -45, 31, 28, 23, -12, 22, 37,
            -66, -69, -94, -6, 3, -29, -19, 31 //
        },
        {
            -57, -36, -117, -27, 21, -3, 1, -31,
            -44, -39, -13, -7, 11, 26, 55, 78,
            -64, -10, 14, -3, 37, -14, 63, 59,
            -35, -9, -5, 42, 18, 19, 45, 60,
            -14, 30, -16, 44, 24, 47, 63, -7,
            -21, -16, -8, -18, 19, 35, 32, 81,
            -110, -74, -17, 15, 30, -53, 32, 83,
            -117, -18, -75, -34, -16, 0, 19, 22 //
        },
        {
            -14, 1, 1, 18, -35, -36, 22, -27,
            -20, -14, 33, 27, 27, 13, 19, 29,
            -12, 7, -1, 1, -1, 13, 20, 46,
            6, -17, -14, -16, -29, -10, 6, 16,
            -1, -22, -11, -21, -26, -15, 8, 19,
            -11, 15, -11, 2, -2, 15, 12, 34,
            -28, -34, 23, 29, 27, 3, 36, 19,
            -26, 1, -3, 18, -72, -72, -11, 46 //
        },
        {
            -80, -34, -25, 2, -16, 19, 21, -22,
            -66, -17, -10, 7, -11, 23, 6, 47,
            -71, -14, 6, 26, 18, 50, 60, 70,
            -29, -46, -11, 32, 40, 17, 49, 55,
            -25, -37, -5, 13, 42, 46, 32, 49,
            -38, 9, -7, -1, 18, 47, 31, 51,
            -36, -85, -7, 2, 3, 24, 5, 40,
            -82, -31, 7, 2, -51, -133, -7, 29 //
        },
        {
            -13, -10, -9, 7, 6, -18, 37, -20,
            -1, -9, 2, -6, 2, 19, 28, 38,
            -7, -9, -9, -15, -17, 4, 26, 41,
            -16, -15, -16, -7, -9, -3, 4, 18,
            -17, -13, -15, -18, -7, -5, 5, 16,
            -4, -10, -17, -13, -10, -1, 14, 26,
            -17, -15, 0, 9, 8, 19, 4, 22,
            -14, -10, 1, 9, 10, -20, 45, -4 //
        },
        {
            -22, -24, -27, 1, -9, -33, -18, 59,
            -31, -18, -6, -16, -5, -0, 57, 66,
            -35, -31, -14, -5, -18, 18, 45, 64,
            -40, -37, -23, -7, 26, 16, 39, 47,
            -25, -26, -25, -12, 18, 14, 43, 38,
            -29, -24, -6, -8, -17, 10, 24, 26,
            -33, -21, -12, 0, -1, 16, 30, 43,
            -26, -31, -10, 10, -9, -28, -10, 64 //
        },
    }};

constexpr static std::array<std::array<int, 64>, PSQTBucketCount> EndGameQueenTable = {
    {
        {
            -77, -47, 41, -26, -6, -58, -139, -54,
            10, 3, -26, 3, 14, 13, -23, 46,
            -16, 60, 39, 47, 16, 23, -62, -12,
            43, 1, -12, -45, 9, -24, 25, -28,
            40, 14, 66, 61, 16, 17, 12, -7,
            48, 3, 57, -24, 11, -53, -28, 42,
            -31, -37, 45, 32, 31, -2, -39, -103,
            -95, -7, 74, 69, -13, 8, 72, -19 //
        },
        {
            -8, -55, 1, 6, -29, -9, -110, -63,
            34, -41, -18, 4, -29, -27, 10, -20,
            30, -14, 35, 34, 24, -11, -20, 19,
            -3, 50, 34, 23, 33, 57, 37, 10,
            -35, 42, 28, 27, 26, -21, 14, 0,
            59, -9, 37, 49, 14, 36, -25, 48,
            -2, 2, -23, -4, 16, 10, -25, -65,
            -80, -102, -12, 12, 15, -4, -14, 2 //
        },
        {
            -37, -12, 27, 0, 2, -82, -92, -74,
            -36, -4, -30, -17, 4, -9, -36, -17,
            42, -6, 25, -12, 41, 10, -24, -66,
            -21, 26, 22, 43, 28, -11, -10, 5,
            -8, -1, 38, 13, 14, -1, 6, 28,
            15, 45, 16, 10, 13, 61, 5, 24,
            7, 15, -29, -4, 14, 42, -45, -14,
            -31, 5, 72, 12, 6, -9, 37, -35 //
        },
        {
            -44, -11, 68, -21, 9, -37, -10, -41,
            4, 6, -18, 53, 17, -25, -90, -49,
            75, 5, 1, -11, 2, 51, -83, -44,
            -12, 5, 8, -10, 19, 18, -12, -37,
            16, -21, 30, 16, 13, -6, -71, 52,
            -55, 8, 25, 32, 34, 12, 10, -25,
            54, 54, 4, -24, -23, 77, -36, 3,
            9, -15, 25, 30, 22, -13, -16, -5 //
        },
        {
            -27, -66, -50, -86, 9, 4, -19, 12,
            -29, -3, -74, -55, -53, -25, -38, 16,
            -23, -17, 1, 4, 10, 3, -11, -11,
            -36, 3, 16, 58, 60, 66, 19, 27,
            -3, 16, 34, 54, 57, 66, 22, 10,
            -2, -33, 23, -4, 18, 10, 33, 26,
            -25, 18, -63, -59, -38, 22, -50, 24,
            51, 24, 25, -52, 71, 48, 41, -48 //
        },
        {
            11, 8, -10, -24, 4, -5, -32, -15,
            32, -17, 4, -2, 13, -24, -28, 2,
            32, -19, 0, -26, -13, -25, -56, 20,
            -35, 21, -7, -19, -16, 17, 14, -8,
            -20, 3, -3, 0, -17, -5, 24, 24,
            -6, -39, 10, 22, -1, -12, 17, 25,
            -37, 28, -7, 19, -8, -6, -4, 8,
            14, -8, -22, 6, 40, 146, 24, -13 //
        },
        {
            -7, -15, -4, -6, -32, -30, -3, 79,
            -15, -7, -15, 2, -3, -38, -49, 16,
            -20, -0, 3, 7, 17, 12, 7, 7,
            3, -4, 2, 2, 19, 19, 48, 25,
            4, -2, 1, 15, 12, 24, 14, 40,
            -26, -4, 12, 7, 7, 11, 16, 5,
            -1, 0, -16, -26, -21, -47, -17, 32,
            2, -10, -20, -11, -31, -13, -3, 25 //
        },
        {
            -13, -3, 12, -3, 16, 15, -7, -15,
            1, -3, -16, 22, 17, 27, -67, -52,
            -10, 6, -7, -16, 44, 31, -18, 7,
            -14, 17, 4, -8, -8, 35, 18, 27,
            -20, -14, 13, 4, -4, 27, 0, 32,
            -5, -17, -19, 5, 26, 17, 10, 36,
            5, -14, -7, -1, 6, -10, -53, -16,
            -3, -9, -8, -27, 14, 14, -13, -6 //
        },
    }};

constexpr static std::array<std::array<int, 64>, PSQTBucketCount> MiddleGameKingTable = {
    {
        {
            756, 0, 0, 0, 0, 0, 0, 0,
            725, 0, 0, 0, 0, 0, 0, 0,
            663, 0, 0, 0, 0, 0, 0, 0,
            640, 0, 0, 0, 0, 0, 0, 0,
            648, 0, 0, 0, 0, 0, 0, 0,
            632, 0, 0, 0, 0, 0, 0, 0,
            700, 0, 0, 0, 0, 0, 0, 0,
            685, 0, 0, 0, 0, 0, 0, 0 //
        },
        {
            0, 355, 0, 0, 0, 0, 0, 0,
            0, 331, 0, 0, 0, 0, 0, 0,
            0, 284, 0, 0, 0, 0, 0, 0,
            0, 261, 0, 0, 0, 0, 0, 0,
            0, 285, 0, 0, 0, 0, 0, 0,
            0, 275, 0, 0, 0, 0, 0, 0,
            0, 312, 0, 0, 0, 0, 0, 0,
            0, 334, 0, 0, 0, 0, 0, 0 //
        },
        {
            0, 0, 128, 0, 0, 0, 0, 0,
            0, 0, 114, 0, 0, 0, 0, 0,
            0, 0, 39, 0, 0, 0, 0, 0,
            0, 0, 34, 0, 0, 0, 0, 0,
            0, 0, 43, 0, 0, 0, 0, 0,
            0, 0, 36, 0, 0, 0, 0, 0,
            0, 0, 95, 0, 0, 0, 0, 0,
            0, 0, 129, 0, 0, 0, 0, 0 //
        },
        {
            0, 0, 0, 155, 0, 0, 0, 0,
            0, 0, 0, 168, 0, 0, 0, 0,
            0, 0, 0, 109, 0, 0, 0, 0,
            0, 0, 0, 42, 0, 0, 0, 0,
            0, 0, 0, 59, 0, 0, 0, 0,
            0, 0, 0, 102, 0, 0, 0, 0,
            0, 0, 0, 164, 0, 0, 0, 0,
            0, 0, 0, 155, 0, 0, 0, 0 //
        },
        {
            0, 0, 0, 0, 22, 0, 0, 0,
            0, 0, 0, 0, -28, 0, 0, 0,
            0, 0, 0, 0, -74, 0, 0, 0,
            0, 0, 0, 0, -122, 0, 0, 0,
            0, 0, 0, 0, -118, 0, 0, 0,
            0, 0, 0, 0, -46, 0, 0, 0,
            0, 0, 0, 0, -5, 0, 0, 0,
            0, 0, 0, 0, 46, 0, 0, 0 //
        },
        {
            0, 0, 0, 0, 0, -32, 0, 0,
            0, 0, 0, 0, 0, -13, 0, 0,
            0, 0, 0, 0, 0, -92, 0, 0,
            0, 0, 0, 0, 0, -107, 0, 0,
            0, 0, 0, 0, 0, -103, 0, 0,
            0, 0, 0, 0, 0, -84, 0, 0,
            0, 0, 0, 0, 0, -27, 0, 0,
            0, 0, 0, 0, 0, -42, 0, 0 //
        },
        {
            0, 0, 0, 0, 0, 0, -305, 0,
            0, 0, 0, 0, 0, 0, -332, 0,
            0, 0, 0, 0, 0, 0, -394, 0,
            0, 0, 0, 0, 0, 0, -441, 0,
            0, 0, 0, 0, 0, 0, -442, 0,
            0, 0, 0, 0, 0, 0, -387, 0,
            0, 0, 0, 0, 0, 0, -313, 0,
            0, 0, 0, 0, 0, 0, -289, 0 //
        },
        {
            0, 0, 0, 0, 0, 0, 0, 81,
            0, 0, 0, 0, 0, 0, 0, 70,
            0, 0, 0, 0, 0, 0, 0, -22,
            0, 0, 0, 0, 0, 0, 0, -78,
            0, 0, 0, 0, 0, 0, 0, -74,
            0, 0, 0, 0, 0, 0, 0, -15,
            0, 0, 0, 0, 0, 0, 0, 76,
            0, 0, 0, 0, 0, 0, 0, 89 //
        },
    }};

constexpr static std::array<std::array<int, 64>, PSQTBucketCount> EndGameKingTable = {
    {
        {
            -113, 0, 0, 0, 0, 0, 0, 0,
            -64, 0, 0, 0, 0, 0, 0, 0,
            -45, 0, 0, 0, 0, 0, 0, 0,
            -41, 0, 0, 0, 0, 0, 0, 0,
            -46, 0, 0, 0, 0, 0, 0, 0,
            -38, 0, 0, 0, 0, 0, 0, 0,
            -67, 0, 0, 0, 0, 0, 0, 0,
            -103, 0, 0, 0, 0, 0, 0, 0 //
        },
        {
            0, -75, 0, 0, 0, 0, 0, 0,
            0, -22, 0, 0, 0, 0, 0, 0,
            0, 2, 0, 0, 0, 0, 0, 0,
            0, 17, 0, 0, 0, 0, 0, 0,
            0, 8, 0, 0, 0, 0, 0, 0,
            0, 4, 0, 0, 0, 0, 0, 0,
            0, -22, 0, 0, 0, 0, 0, 0,
            0, -77, 0, 0, 0, 0, 0, 0 //
        },
        {
            0, 0, -48, 0, 0, 0, 0, 0,
            0, 0, -7, 0, 0, 0, 0, 0,
            0, 0, 39, 0, 0, 0, 0, 0,
            0, 0, 59, 0, 0, 0, 0, 0,
            0, 0, 57, 0, 0, 0, 0, 0,
            0, 0, 39, 0, 0, 0, 0, 0,
            0, 0, -0, 0, 0, 0, 0, 0,
            0, 0, -55, 0, 0, 0, 0, 0 //
        },
        {
            0, 0, 0, -30, 0, 0, 0, 0,
            0, 0, 0, 10, 0, 0, 0, 0,
            0, 0, 0, 57, 0, 0, 0, 0,
            0, 0, 0, 101, 0, 0, 0, 0,
            0, 0, 0, 99, 0, 0, 0, 0,
            0, 0, 0, 61, 0, 0, 0, 0,
            0, 0, 0, 13, 0, 0, 0, 0,
            0, 0, 0, -29, 0, 0, 0, 0 //
        },
        {
            0, 0, 0, 0, -32, 0, 0, 0,
            0, 0, 0, 0, 32, 0, 0, 0,
            0, 0, 0, 0, 71, 0, 0, 0,
            0, 0, 0, 0, 110, 0, 0, 0,
            0, 0, 0, 0, 112, 0, 0, 0,
            0, 0, 0, 0, 66, 0, 0, 0,
            0, 0, 0, 0, 26, 0, 0, 0,
            0, 0, 0, 0, -34, 0, 0, 0 //
        },
        {
            0, 0, 0, 0, 0, 4, 0, 0,
            0, 0, 0, 0, 0, 39, 0, 0,
            0, 0, 0, 0, 0, 84, 0, 0,
            0, 0, 0, 0, 0, 102, 0, 0,
            0, 0, 0, 0, 0, 103, 0, 0,
            0, 0, 0, 0, 0, 82, 0, 0,
            0, 0, 0, 0, 0, 41, 0, 0,
            0, 0, 0, 0, 0, 5, 0, 0 //
        },
        {
            0, 0, 0, 0, 0, 0, -33, 0,
            0, 0, 0, 0, 0, 0, 11, 0,
            0, 0, 0, 0, 0, 0, 41, 0,
            0, 0, 0, 0, 0, 0, 65, 0,
            0, 0, 0, 0, 0, 0, 67, 0,
            0, 0, 0, 0, 0, 0, 41, 0,
            0, 0, 0, 0, 0, 0, 7, 0,
            0, 0, 0, 0, 0, 0, -34, 0 //
        },
        {
            0, 0, 0, 0, 0, 0, 0, -56,
            0, 0, 0, 0, 0, 0, 0, -11,
            0, 0, 0, 0, 0, 0, 0, 25,
            0, 0, 0, 0, 0, 0, 0, 35,
            0, 0, 0, 0, 0, 0, 0, 34,
            0, 0, 0, 0, 0, 0, 0, 20,
            0, 0, 0, 0, 0, 0, 0, -14,
            0, 0, 0, 0, 0, 0, 0, -59 //
        },
    }};
