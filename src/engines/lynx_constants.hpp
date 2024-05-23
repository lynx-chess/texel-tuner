#include <array>
#include <cmath>

using u64 = uint64_t;

constexpr int32_t Pack(const int16_t mg, const int16_t eg) {
    return ((int32_t) eg << 16) + (int32_t) mg;
}

constexpr int16_t UnpackMG(const int32_t packed) {
    return (int16_t) packed;
}

constexpr int16_t UnpackEG(const int32_t packed) {
    return (int16_t) ((packed + 0x8000) >> 16);
}

constexpr int PieceOffset(bool isWhite)
{
    return 6 - (6 * isWhite);
}

constexpr std::array<int, 12> PieceValue = {
        +101, +392, +170, +488, +1114, // 0,
        +129, +443, +209, +777, +1426, // 0
};

constexpr std::array<int, 10> PackedPieceValue = {
    Pack(PieceValue[0], PieceValue[5]),
    Pack(PieceValue[1], PieceValue[6]),
    Pack(PieceValue[2], PieceValue[7]),
    Pack(PieceValue[3], PieceValue[8]),
    Pack(PieceValue[4], PieceValue[9])
};

std::array<int, 6> phaseValues = {0, 1, 1, 2, 4, 0};

constexpr static int EvalNormalizationCoefficient = 142;

constexpr static std::array<int, 64> MiddleGamePawnTable =
    {
        0,      0,      0,      0,      0,      0,      0,      0,
        -24,    -23,    -15,    -6,     -2,     28,     28,     -12,
        -25,    -26,    -6,     11,     18,     26,     22,     9,
        -24,    -16,    3,      17,     25,     29,     0,      -3,
        -24,    -12,    1,      19,     26,     27,     -1,     -4,
        -22,    -19,    -4,     4,      14,     23,     15,     3,
        -25,    -21,    -19,    -10,    -4,     22,     18,     -19,
        0,      0,      0,      0,      0,      0,      0,      0,
    };

constexpr static std::array<int, 64> EndGamePawnTable =
    {
        0,      0,      0,      0,      0,      0,      0,      0,
        14,     9,      5,      -10,    7,      1,      -7,     -7,
        12,     9,      -1,     -12,    -6,     -7,     -7,     -8,
        26,     16,     -1,     -19,    -15,    -14,    3,      0,
        24,     15,     -2,     -15,    -14,    -11,    1,      -2,
        13,     6,      -3,     -11,    -3,     -5,     -6,     -9,
        16,     9,      9,      -8,     16,     4,      -4,     -4,
        0,      0,      0,      0,      0,      0,      0,      0,
    };

constexpr static std::array<int, 64> MiddleGameKnightTable =
    {
        -146,   -23,    -50,    -31,    -13,    -21,    -11,    -98,
        -45,    -28,    -3,     17,     18,     23,     -14,    -17,
        -28,    1,      20,     56,     60,     40,     34,     -5,
        -11,    24,     44,     59,     59,     60,     44,     17,
        -7,     24,     46,     47,     57,     58,     44,     16,
        -26,    4,      20,     49,     58,     33,     27,     -6,
        -47,    -19,    0,      16,     17,     18,     -13,    -19,
        -164,   -25,    -48,    -21,    -10,    -12,    -18,    -89,
    };

constexpr static std::array<int, 64> EndGameKnightTable =
    {
        -66,    -57,    -12,    -10,    -9,     -26,    -51,    -87,
        -18,    1,      14,     9,      9,      6,      -9,     -19,
        -13,    15,     36,     36,     33,     18,     9,      -13,
        7,      20,     48,     49,     53,     47,     24,     -6,
        4,      25,     47,     52,     53,     42,     28,     0,
        -15,    18,     27,     40,     32,     19,     6,      -9,
        -24,    4,      6,      11,     5,      1,      -11,    -23,
        -71,    -57,    -7,     -13,    -10,    -25,    -49,    -86,
    };

constexpr static std::array<int, 64> MiddleGameBishopTable =
    {
        -13,    14,     -2,     -16,    -10,    -15,    -21,    4,
        6,      1,      5,      -19,    0,      -1,     26,     -13,
        -7,     3,      -7,     5,      -8,     10,     4,      25,
        -9,     -8,     -4,     23,     21,     -13,    0,      -2,
        -16,    -3,     -12,    19,     8,      -10,    -8,     4,
        4,      2,      4,      -3,     6,      3,      6,      21,
        9,      13,     9,      -7,     -4,     2,      19,     -3,
        11,     18,     11,     -31,    -12,    -20,    1,      -11,
    };

constexpr static std::array<int, 64> EndGameBishopTable =
    {
        -2,     14,     -9,     1,      -1,     7,      -1,     -17,
        0,      -7,     -7,     0,      -2,     -13,    -2,     -10,
        12,     10,     6,      0,      9,      3,      3,      10,
        12,     3,      7,      6,      4,      7,      2,      5,
        6,      6,      5,      9,      2,      7,      3,      6,
        9,      0,      0,      -2,     4,      -1,     1,      6,
        -9,     -10,    -18,    -2,     -3,     -6,     -2,     -4,
        1,      -12,    -8,     5,      7,      7,      -2,     -6,
    };

constexpr static std::array<int, 64> MiddleGameRookTable =
    {
        -4,     -10,    -5,     2,      14,     3,      7,      -2,
        -26,    -17,    -14,    -12,    0,      3,      17,     -4,
        -29,    -20,    -22,    -11,    4,      9,      49,     27,
        -23,    -20,    -17,    -7,     -3,     8,      37,     19,
        -18,    -15,    -12,    -4,     -6,     7,      28,     14,
        -22,    -15,    -18,    -3,     2,      19,     47,     26,
        -24,    -26,    -9,     -5,     2,      1,      23,     2,
        -3,     -4,     0,      12,     22,     8,      14,     9,
    };

constexpr static std::array<int, 64> EndGameRookTable =
    {
        4,      2,      6,      -3,     -11,    2,      0,      -4,
        16,     18,     18,     8,      -1,     -2,     -4,     2,
        13,     11,     12,     5,      -7,     -10,    -20,    -16,
        14,     11,     13,     6,      0,      0,      -13,    -13,
        14,     10,     13,     4,      1,      -6,     -10,    -9,
        12,     13,     4,      -3,     -10,    -13,    -20,    -11,
        19,     22,     14,     4,      -3,     -2,     -5,     2,
        -1,     -3,     1,      -8,     -18,    -5,     -8,     -13,
    };

constexpr static std::array<int, 64> MiddleGameQueenTable =
    {
        -12,    -10,    -5,     9,      3,      -30,    8,      3,
        -2,     -10,    6,      -2,     2,      5,      23,     50,
        -9,     -6,     -9,     -10,    -11,    7,      33,     56,
        -12,    -19,    -19,    -9,     -10,    -5,     11,     25,
        -11,    -15,    -19,    -18,    -9,     -5,     9,      22,
        -6,     -4,     -15,    -12,    -5,     4,      20,     37,
        -15,    -20,    3,      10,     7,      3,      7,      37,
        -10,    -10,    4,      12,     6,      -36,    -13,    25,
    };

constexpr static std::array<int, 64> EndGameQueenTable =
    {
        -25,    -21,    -12,    -12,    -17,    -11,    -34,    7,
        -17,    -10,    -25,    -1,     -3,     -16,    -46,    -8,
        -16,    -4,     6,      2,      22,     20,     -8,     2,
        -9,     9,      10,     14,     27,     38,     44,     30,
        -3,     5,      16,     26,     23,     33,     25,     39,
        -16,    -11,    14,     11,     13,     19,     18,     14,
        -12,    -3,     -20,    -19,    -13,    -12,    -32,    2,
        -16,    -17,    -18,    -6,     -11,    16,     13,     -3,
    };

constexpr static std::array<int, 64> MiddleGameKingTable =
    {
        24,     49,     25,     -75,    8,      -61,    39,     47,
        -12,    -16,    -35,    -72,    -84,    -58,    -10,    19,
        -84,    -69,    -107,   -108,   -115,   -124,   -83,    -96,
        -108,   -97,    -116,   -152,   -148,   -138,   -137,   -162,
        -77,    -70,    -104,   -131,   -148,   -123,   -141,   -159,
        -82,    -46,    -98,    -104,   -93,    -104,   -75,    -88,
        72,     -9,     -38,    -64,    -68,    -47,    5,      26,
        36,     74,     37,     -60,    20,     -52,    52,     61,
    };

constexpr static std::array<int, 64> EndGameKingTable =
    {
        -71,    -45,    -20,    5,      -32,    -2,     -37,    -89,
        -12,    18,     27,     40,     46,     33,     13,     -23,
        11,     43,     59,     69,     72,     64,     44,     22,
        15,     54,     75,     90,     89,     81,     68,     39,
        6,      45,     73,     87,     92,     79,     71,     39,
        12,     39,     58,     69,     67,     59,     43,     18,
        -38,    13,     29,     38,     40,     30,     8,      -25,
        -82,    -56,    -26,    -1,     -28,    -5,     -41,    -94,
    };

constexpr static std::array<std::array<int, 64>, 6> MiddleGamePositionalWhiteTables =
    {
        MiddleGamePawnTable,
        MiddleGameKnightTable,
        MiddleGameBishopTable,
        MiddleGameRookTable,
        MiddleGameQueenTable,
        MiddleGameKingTable};

constexpr static std::array<std::array<int, 64>, 6> EndGamePositionalWhiteTables{
    EndGamePawnTable,
    EndGameKnightTable,
    EndGameBishopTable,
    EndGameRookTable,
    EndGameQueenTable,
    EndGameKingTable};

int MiddleGamePositionalTables(int piece, int square)
{
    int coefficient = 1;
    if (piece >= 6)
    {
        piece -= 6;
        square ^= 56;
        coefficient = -1;
    }

    return MiddleGamePositionalWhiteTables[piece][square] * coefficient;
}

int EndGamePositionalTables(int piece, int square)
{
    int coefficient = 1;
    if (piece >= 6)
    {
        piece -= 6;
        square ^= 56;
        coefficient = -1;
    }

    return EndGamePositionalWhiteTables[piece][square] * coefficient;
}

int PackedPositionalTables(int piece, int square)
{
    int coefficient = 1;
    if (piece >= 6)
    {
        piece -= 6;
        square ^= 56;
        coefficient = -1;
    }

    return Pack(
        MiddleGamePositionalWhiteTables[piece][square] * coefficient,
        EndGamePositionalWhiteTables[piece][square] * coefficient);
}

constexpr static std::array<u64, 64> FileMasks = {
    72340172838076673UL, 144680345676153346UL, 289360691352306692UL, 578721382704613384UL, 1157442765409226768UL, 2314885530818453536UL, 4629771061636907072UL, 9259542123273814144UL,
    72340172838076673UL, 144680345676153346UL, 289360691352306692UL, 578721382704613384UL, 1157442765409226768UL, 2314885530818453536UL, 4629771061636907072UL, 9259542123273814144UL,
    72340172838076673UL, 144680345676153346UL, 289360691352306692UL, 578721382704613384UL, 1157442765409226768UL, 2314885530818453536UL, 4629771061636907072UL, 9259542123273814144UL,
    72340172838076673UL, 144680345676153346UL, 289360691352306692UL, 578721382704613384UL, 1157442765409226768UL, 2314885530818453536UL, 4629771061636907072UL, 9259542123273814144UL,
    72340172838076673UL, 144680345676153346UL, 289360691352306692UL, 578721382704613384UL, 1157442765409226768UL, 2314885530818453536UL, 4629771061636907072UL, 9259542123273814144UL,
    72340172838076673UL, 144680345676153346UL, 289360691352306692UL, 578721382704613384UL, 1157442765409226768UL, 2314885530818453536UL, 4629771061636907072UL, 9259542123273814144UL,
    72340172838076673UL, 144680345676153346UL, 289360691352306692UL, 578721382704613384UL, 1157442765409226768UL, 2314885530818453536UL, 4629771061636907072UL, 9259542123273814144UL,
    72340172838076673UL, 144680345676153346UL, 289360691352306692UL, 578721382704613384UL, 1157442765409226768UL, 2314885530818453536UL, 4629771061636907072UL, 9259542123273814144UL};

constexpr static std::array<u64, 64> IsolatedPawnMasks = {
    144680345676153346UL,
    361700864190383365UL,
    723401728380766730UL,
    1446803456761533460UL,
    2893606913523066920UL,
    5787213827046133840UL,
    11574427654092267680UL,
    4629771061636907072UL,
    144680345676153346UL,
    361700864190383365UL,
    723401728380766730UL,
    1446803456761533460UL,
    2893606913523066920UL,
    5787213827046133840UL,
    11574427654092267680UL,
    4629771061636907072UL,
    144680345676153346UL,
    361700864190383365UL,
    723401728380766730UL,
    1446803456761533460UL,
    2893606913523066920UL,
    5787213827046133840UL,
    11574427654092267680UL,
    4629771061636907072UL,
    144680345676153346UL,
    361700864190383365UL,
    723401728380766730UL,
    1446803456761533460UL,
    2893606913523066920UL,
    5787213827046133840UL,
    11574427654092267680UL,
    4629771061636907072UL,
    144680345676153346UL,
    361700864190383365UL,
    723401728380766730UL,
    1446803456761533460UL,
    2893606913523066920UL,
    5787213827046133840UL,
    11574427654092267680UL,
    4629771061636907072UL,
    144680345676153346UL,
    361700864190383365UL,
    723401728380766730UL,
    1446803456761533460UL,
    2893606913523066920UL,
    5787213827046133840UL,
    11574427654092267680UL,
    4629771061636907072UL,
    144680345676153346UL,
    361700864190383365UL,
    723401728380766730UL,
    1446803456761533460UL,
    2893606913523066920UL,
    5787213827046133840UL,
    11574427654092267680UL,
    4629771061636907072UL,
    144680345676153346UL,
    361700864190383365UL,
    723401728380766730UL,
    1446803456761533460UL,
    2893606913523066920UL,
    5787213827046133840UL,
    11574427654092267680UL,
    4629771061636907072UL,
};

constexpr static std::array<u64, 64> WhitePassedPawnMasks = {
    0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL,
    3UL, 7UL, 14UL, 28UL, 56UL, 112UL, 224UL, 192UL,
    771UL, 1799UL, 3598UL, 7196UL, 14392UL, 28784UL, 57568UL, 49344UL,
    197379UL, 460551UL, 921102UL, 1842204UL, 3684408UL, 7368816UL, 14737632UL, 12632256UL,
    50529027UL, 117901063UL, 235802126UL, 471604252UL, 943208504UL, 1886417008UL, 3772834016UL, 3233857728UL,
    12935430915UL, 30182672135UL, 60365344270UL, 120730688540UL, 241461377080UL, 482922754160UL, 965845508320UL, 827867578560UL,
    3311470314243UL, 7726764066567UL, 15453528133134UL, 30907056266268UL, 61814112532536UL, 123628225065072UL, 247256450130144UL, 211934100111552UL,
    847736400446211UL, 1978051601041159UL, 3956103202082318UL, 7912206404164636UL, 15824412808329272UL, 31648825616658544UL, 63297651233317088UL, 54255129628557504UL};

constexpr static std::array<u64, 64> BlackPassedPawnMasks = {
    217020518514230016UL, 506381209866536704UL, 1012762419733073408UL, 2025524839466146816UL, 4051049678932293632UL, 8102099357864587264UL, 16204198715729174528UL, 13889313184910721024UL,
    217020518514229248UL, 506381209866534912UL, 1012762419733069824UL, 2025524839466139648UL, 4051049678932279296UL, 8102099357864558592UL, 16204198715729117184UL, 13889313184910671872UL,
    217020518514032640UL, 506381209866076160UL, 1012762419732152320UL, 2025524839464304640UL, 4051049678928609280UL, 8102099357857218560UL, 16204198715714437120UL, 13889313184898088960UL,
    217020518463700992UL, 506381209748635648UL, 1012762419497271296UL, 2025524838994542592UL, 4051049677989085184UL, 8102099355978170368UL, 16204198711956340736UL, 13889313181676863488UL,
    217020505578799104UL, 506381179683864576UL, 1012762359367729152UL, 2025524718735458304UL, 4051049437470916608UL, 8102098874941833216UL, 16204197749883666432UL, 13889312357043142656UL,
    217017207043915776UL, 506373483102470144UL, 1012746966204940288UL, 2025493932409880576UL, 4050987864819761152UL, 8101975729639522304UL, 16203951459279044608UL, 13889101250810609664UL,
    216172782113783808UL, 504403158265495552UL, 1008806316530991104UL, 2017612633061982208UL, 4035225266123964416UL, 8070450532247928832UL, 16140901064495857664UL, 13835058055282163712UL,
    0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL};

constexpr static std::array<int, 64> Rank =
    {
        7UL, 7UL, 7UL, 7UL, 7UL, 7UL, 7UL, 7UL,
        6UL, 6UL, 6UL, 6UL, 6UL, 6UL, 6UL, 6UL,
        5UL, 5UL, 5UL, 5UL, 5UL, 5UL, 5UL, 5UL,
        4UL, 4UL, 4UL, 4UL, 4UL, 4UL, 4UL, 4UL,
        3UL, 3UL, 3UL, 3UL, 3UL, 3UL, 3UL, 3UL,
        2UL, 2UL, 2UL, 2UL, 2UL, 2UL, 2UL, 2UL,
        1UL, 1UL, 1UL, 1UL, 1UL, 1UL, 1UL, 1UL,
        0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL};

static void print_psqt(const parameters_t &parameters)
{
    int pieceValues[12];

    for (int phase = 0; phase <= 1; ++phase)
    {
        if (phase == 0)
            std::cout << "internal static readonly short[] MiddleGamePieceValues =\n[\n\t";

        else
            std::cout << "\ninternal static readonly short[] EndGamePieceValues =\n[\n\t";

        // Pawns
        {
            int pawnSum = 0;

            for (int square = 0; square < 48; ++square)
            {
                pawnSum += parameters[square][phase];
            }

            auto average = (pawnSum / 48.0);

            auto pieceIndex = phase * 6;
            // pieceValues[pieceIndex] = PieceValue[phase * 5];
            pieceValues[pieceIndex] = static_cast<int>(std::round(average));
            std::cout << "+" << pieceValues[pieceIndex] << ", ";
        }

        for (int piece = 1; piece < 5; ++piece)
        {
            int sum = 0;

            for (int square = 0; square < 64; ++square)
            {
                sum += parameters[piece * 64 - 16 + square][phase]; // Substract 16 since we're only tuning 48 pawn values
            }

            auto average = (sum / 64.0);

            auto pieceIndex = piece + phase * 6;
            // pieceValues[pieceIndex] = PieceValue[piece + phase * 5];
            pieceValues[pieceIndex] = static_cast<int>(std::round(average));
            std::cout << "+" << pieceValues[pieceIndex] << ", ";
        }

        // Kings
        auto kingIndex = 5 + phase * 6;
        pieceValues[kingIndex] = 0;
        std::cout << pieceValues[kingIndex] << ",\n\t";

        for (int piece = 0; piece < 5; ++piece)
        {
            auto pieceIndex = piece + phase * 6;
            std::cout << "-" << pieceValues[pieceIndex] << ", ";
        }

        std::cout << pieceValues[kingIndex] << "\n];\n";
    }

    std::string names[] = {"Pawn", "Knight", "Bishop", "Rook", "Queen", "King"};

    // Pawns
    {
        const int piece = 0;
        for (int phase = 0; phase <= 1; ++phase)
        {
            std::cout << "\ninternal static readonly short[] " << (phase == 0 ? "MiddleGame" : "EndGame") << names[piece] << "Table =\n[\n\t";

            std::cout << "0,\t0,\t0,\t0,\t0,\t0,\t0,\t0,\n\t";
            for (int square = 0; square < 48; ++square)
            {
                std::cout << round(parameters[square][phase] - pieceValues[phase * 6]) << ", ";
                if (square % 8 == 7)
                    std::cout << "\n";
                if (square != 63)
                    std::cout << "\t";
            }
            std::cout << "0,\t0,\t0,\t0,\t0,\t0,\t0,\t0," << std::endl;

            std::cout << "];\n";
        }
    }

    for (int piece = 1; piece < 6; ++piece)
    {
        for (int phase = 0; phase <= 1; ++phase)
        {
            std::cout << "\ninternal static readonly short[] " << (phase == 0 ? "MiddleGame" : "EndGame") << names[piece] << "Table =\n[\n\t";
            for (int square = 0; square < 64; ++square)
            {
                std::cout << round(parameters[piece * 64 - 16 + square][phase] - pieceValues[piece + phase * 6]) << ", "; // We substract the 16 non-tuned pawn valeus
                if (square % 8 == 7)
                    std::cout << "\n";
                if (square != 63)
                    std::cout << "\t";
            }
            std::cout << "];\n";
        }
    }
    std::cout << std::endl;
}
