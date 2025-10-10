#pragma once
#include <array>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <ctime>
#include "lynx_tunable_eval_terms.hpp"

using u64 = uint64_t;

static int print_counter = -2;

constexpr int32_t Pack(const int16_t mg, const int16_t eg)
{
    return ((int32_t)eg << 16) + (int32_t)mg;
}

constexpr int16_t UnpackMG(const int32_t packed)
{
    return (int16_t)packed;
}

constexpr int16_t UnpackEG(const int32_t packed)
{
    return (int16_t)((packed + 0x8000) >> 16);
}

constexpr int PieceOffset(bool isWhite)
{
    return 6 - (6 * isWhite);
}

std::array<int, 6> phaseValues = {0, 1, 1, 2, 4, 0};

constexpr static int EvalNormalizationCoefficient = 119;

constexpr static std::array<std::array<std::array<std::array<int, 64>, PSQTBucketCount>, 6>, 2> MiddleGamePositionalWhiteTables = {{

    {MiddleGamePawnTable,
     MiddleGameKnightTable,
     MiddleGameBishopTable,
     MiddleGameRookTable,
     MiddleGameQueenTable,
     MiddleGameKingTable}, //
    {MiddleGameEnemyPawnTable,
     MiddleGameEnemyKnightTable,
     MiddleGameEnemyBishopTable,
     MiddleGameEnemyRookTable,
     MiddleGameEnemyQueenTable,
     MiddleGameEnemyKingTable}}};

constexpr static std::array<std::array<std::array<std::array<int, 64>, PSQTBucketCount>, 6>, 2> EndGamePositionalWhiteTables = {{

    {EndGamePawnTable,
     EndGameKnightTable,
     EndGameBishopTable,
     EndGameRookTable,
     EndGameQueenTable,
     EndGameKingTable}, //
    {EndGameEnemyPawnTable,
     EndGameEnemyKnightTable,
     EndGameEnemyBishopTable,
     EndGameEnemyRookTable,
     EndGameEnemyQueenTable,
     EndGameEnemyKingTable}}};

constexpr int PackedPieceValue(int friendEnemy, int bucket, int piece)
{
    return Pack(PieceValue[friendEnemy][bucket][piece], PieceValue[friendEnemy][bucket][piece + 5]);
}

constexpr int MiddleGamePositionalTables(int friendEnemy, int bucket, int piece, int square)
{
    int coefficient = 1;
    if (piece >= 6)
    {
        piece -= 6;
        square ^= 56;
        coefficient = -1;
    }

    return MiddleGamePositionalWhiteTables[friendEnemy][piece][bucket][square] * coefficient;
}

constexpr int EndGamePositionalTables(int friendEnemy, int bucket, int piece, int square)
{
    int coefficient = 1;
    if (piece >= 6)
    {
        piece -= 6;
        square ^= 56;
        coefficient = -1;
    }

    return EndGamePositionalWhiteTables[friendEnemy][piece][bucket][square] * coefficient;
}

constexpr int PackedPositionalTables(int friendEnemy, int bucket, int piece, int square)
{
    int coefficient = 1;
    if (piece >= 6)
    {
        piece -= 6;
        square ^= 56;
        coefficient = -1;
    }

    return Pack(
        MiddleGamePositionalWhiteTables[friendEnemy][piece][bucket][square] * coefficient,
        EndGamePositionalWhiteTables[friendEnemy][piece][bucket][square] * coefficient);
}

constexpr static std::array<int, 64> File = {
    0, 1, 2, 3, 4, 5, 6, 7,
    0, 1, 2, 3, 4, 5, 6, 7,
    0, 1, 2, 3, 4, 5, 6, 7,
    0, 1, 2, 3, 4, 5, 6, 7,
    0, 1, 2, 3, 4, 5, 6, 7,
    0, 1, 2, 3, 4, 5, 6, 7,
    0, 1, 2, 3, 4, 5, 6, 7,
    0, 1, 2, 3, 4, 5, 6, 7};

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

constexpr static std::array<int, 64> Rank = {
    7UL, 7UL, 7UL, 7UL, 7UL, 7UL, 7UL, 7UL,
    6UL, 6UL, 6UL, 6UL, 6UL, 6UL, 6UL, 6UL,
    5UL, 5UL, 5UL, 5UL, 5UL, 5UL, 5UL, 5UL,
    4UL, 4UL, 4UL, 4UL, 4UL, 4UL, 4UL, 4UL,
    3UL, 3UL, 3UL, 3UL, 3UL, 3UL, 3UL, 3UL,
    2UL, 2UL, 2UL, 2UL, 2UL, 2UL, 2UL, 2UL,
    1UL, 1UL, 1UL, 1UL, 1UL, 1UL, 1UL, 1UL,
    0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL};

constexpr u64 AFile = 0x101010101010101;
constexpr u64 HFile = 0x8080808080808080;

constexpr u64 NotAFile = 0xFEFEFEFEFEFEFEFE;

constexpr u64 NotHFile = 0x7F7F7F7F7F7F7F7F;

constexpr u64 Corners = 0x8100000000000081;

constexpr static std::array<int, 64> LightSquares = {
    1, 0, 1, 0, 1, 0, 1, 0,
    0, 1, 0, 1, 0, 1, 0, 1,
    1, 0, 1, 0, 1, 0, 1, 0,
    0, 1, 0, 1, 0, 1, 0, 1,
    1, 0, 1, 0, 1, 0, 1, 0,
    0, 1, 0, 1, 0, 1, 0, 1,
    1, 0, 1, 0, 1, 0, 1, 0,
    0, 1, 0, 1, 0, 1, 0, 1};

constexpr static std::array<int, 64> DarkSquares = {
    0, 1, 0, 1, 0, 1, 0, 1,
    1, 0, 1, 0, 1, 0, 1, 0,
    0, 1, 0, 1, 0, 1, 0, 1,
    1, 0, 1, 0, 1, 0, 1, 0,
    0, 1, 0, 1, 0, 1, 0, 1,
    1, 0, 1, 0, 1, 0, 1, 0,
    0, 1, 0, 1, 0, 1, 0, 1,
    1, 0, 1, 0, 1, 0, 1, 0};

constexpr u64 LightSquaresBitBoard = 0xAA55AA55AA55AA55UL;

constexpr u64 DarkSquaresBitBoard = 0x55AA55AA55AA55AAUL;

constexpr u64 CentralFiles = 0x3c3c3c3c3c3c3c3c;

constexpr u64 CentralSquares = 0x1818000000;

constexpr u64 NotAorH = 0x7e7e7e7e7e7e00;

constexpr u64 LongDiagonals = 0x8142241818244281;

constexpr int PositiveCheckmateDetectionLimit = 27000;

constexpr int NegativeCheckmateDetectionLimit = -27000;

constexpr int MinEval = NegativeCheckmateDetectionLimit + 1;

constexpr int MaxEval = PositiveCheckmateDetectionLimit - 1;

constexpr static std::array<u64, 64> KingRing =
    {
197378UL,
460549UL,
921098UL,
1842196UL,
3684392UL,
7368784UL,
14737568UL,
12632128UL,
197123UL,
460039UL,
920078UL,
1840156UL,
3680312UL,
7360624UL,
14721248UL,
12599488UL,
50463488UL,
117769984UL,
235539968UL,
471079936UL,
942159872UL,
1884319744UL,
3768639488UL,
3225468928UL,
12918652928UL,
30149115904UL,
60298231808UL,
120596463616UL,
241192927232UL,
482385854464UL,
964771708928UL,
825720045568UL,
3307175149568UL,
7718173671424UL,
15436347342848UL,
30872694685696UL,
61745389371392UL,
123490778742784UL,
246981557485568UL,
211384331665408UL,
846636838289408UL,
1975852459884544UL,
3951704919769088UL,
7903409839538176UL,
15806819679076352UL,
31613639358152704UL,
63227278716305408UL,
54114388906344448UL,
216739030602088448UL,
505818229730443264UL,
1011636459460886528UL,
2023272918921773056UL,
4046545837843546112UL,
8093091675687092224UL,
16186183351374184448UL,
13853283560024178688UL,
144962911540871168UL,
362265991608008704UL,
724531983216017408UL,
1449063966432034816UL,
2898127932864069632UL,
5796255865728139264UL,
11592511731456278528UL,
4665940320188366848UL
};

[[nodiscard]] static u64 ShiftUp(const u64 board)
{
    return board >> 8;
}

[[nodiscard]] static u64 ShiftDown(const u64 board)
{
    return board << 8;
}

[[nodiscard]] static u64 ShiftLeft(const u64 board)
{
    return (board >> 1) & NotHFile;
}

[[nodiscard]] static u64 ShiftRight(const u64 board)
{
    return (board << 1) & NotAFile;
}

[[nodiscard]] static u64 ShiftUpRight(const u64 board)
{
    return ShiftRight(ShiftUp(board));
}

[[nodiscard]] static u64 ShiftUpLeft(const u64 board)
{
    return ShiftLeft(ShiftUp(board));
}

[[nodiscard]] static u64 ShiftDownRight(const u64 board)
{
    return ShiftRight(ShiftDown(board));
}

[[nodiscard]] static u64 ShiftDownLeft(const u64 board)
{
    return ShiftLeft(ShiftDown(board));
}

[[nodiscard]] static u64 ChebyshevDistance(const int square1, const int square2)
{
    auto xDelta = std::abs(File[square1] - File[square2]);
    auto yDelta = std::abs(Rank[square1] - Rank[square2]);

    return xDelta >= yDelta
               ? xDelta
               : yDelta;
}

[[nodiscard]] static u64 ManhattanDistance(const int square1, const int square2)
{
    auto xDelta = std::abs(File[square1] - File[square2]);
    auto yDelta = std::abs(Rank[square1] - Rank[square2]);

    return xDelta + yDelta;
}

[[nodiscard]] static bool SameColor(int squareIndex1, int squareIndex2)
{
    return ((9 * (squareIndex1 ^ squareIndex2)) & 8) == 0;
}

[[nodiscard]] static bool DifferentColor(int squareIndex1, int squareIndex2)
{
    return ((9 * (squareIndex1 ^ squareIndex2)) & 8) != 0;
}

[[nodiscard]] static bool GetBit(const u64 board, const int squareIndex)
{
    return (board & (1UL << squareIndex)) != 0;
}

static void print_psqts_csharp(const parameters_t &parameters, const std::array<std::array<tune_t, 12>, PSQTBucketCount> &existingPieceValues)
{
    std::stringstream ss;
    std::string names[] = {"Pawn", "Knight", "Bishop", "Rook", "Queen", "King"};

    std::array<std::array<std::array<std::array<tune_t, 12>, PSQTBucketCount>, 2>, 2> psqtPieceValues;

    // Extract and print pieces values
    for (int phase = 0; phase <= 1; ++phase)
    {
        for (int friendEnemy = 0; friendEnemy < 2; ++friendEnemy)
        {
            auto friendEnemyIndexOffset = friendEnemy * psqtIndexCount / 2;
            if (friendEnemy == 0)
            {
                if (phase == 0)
                    ss << "\tinternal static readonly short[][][] MiddleGamePieceValues =\n\t[";

                else
                    ss << "\n\tinternal static readonly short[][][] EndGamePieceValues =\n\t[";
            }

            ss << "\n\t\t[";
            for (int bucket = 0; bucket < PSQTBucketCount; ++bucket)
            {
                ss << "\n\t\t\t[\n\t\t\t\t";

                // Pawns
                {
                    tune_t pawnSum = 0;

                    for (int square = 0; square < 48; ++square)
                    {
                        pawnSum += parameters[friendEnemyIndexOffset + 48 * bucket + square][phase];
                    }

                    auto average = (pawnSum / 48.0);
                    auto pieceIndex = phase * 6;

                    psqtPieceValues[friendEnemy][phase][bucket][pieceIndex] = average;

                    auto value = std::round(
                        average +
                        (friendEnemy == 0
                             ? existingPieceValues[bucket][pieceIndex]
                             : 0));

                    ss << std::showpos << value << std::noshowpos << ", ";
                }

                for (int piece = 1; piece < 5; ++piece)
                {
                    tune_t sum = 0;

                    for (int square = 0; square < 64; ++square)
                    {
                        sum += parameters
                            [friendEnemyIndexOffset +
                             (48 * PSQTBucketCount) +               // 64 - 16, since we're only tuning 48 pawn values
                             (64 * PSQTBucketCount) * (piece - 1) + // piece - 1 since we already took pawns into account
                             (64 * bucket) +
                             square]
                            [phase];
                    }

                    auto average = (sum / 64.0);
                    auto pieceIndex = piece + phase * 6;

                    // ss << std::endl
                    //           << existingPieceValues[0][pieceIndex] << "==" << existingPieceValues[1][pieceIndex] << std::endl;

                    psqtPieceValues[friendEnemy][phase][bucket][pieceIndex] = average;
                    auto value = std::round(
                        average +
                        (friendEnemy == 0
                             ? existingPieceValues[bucket][pieceIndex]
                             : 0));

                    ss << std::showpos << value << std::noshowpos << ", ";
                }

                // Kings
                auto kingIndex = 5 + phase * 6;
                psqtPieceValues[friendEnemy][phase][bucket][kingIndex] = 0;
                auto extraKingValues = friendEnemy == 0 ? existingPieceValues[bucket][kingIndex] : 0;
                ss << psqtPieceValues[friendEnemy][phase][bucket][kingIndex] + extraKingValues << ",\n\t\t\t\t";

                for (int piece = 0; piece < 5; ++piece)
                {
                    auto pieceIndex = piece + phase * 6;
                    auto value = std::round(
                        psqtPieceValues[friendEnemy][phase][bucket][pieceIndex] +
                        (friendEnemy == 0
                             ? existingPieceValues[bucket][pieceIndex]
                             : 0));

                    value = -value;

                    ss << std::showpos << value << std::noshowpos << ", ";
                }
                ss << std::round(psqtPieceValues[friendEnemy][phase][bucket][kingIndex] + extraKingValues) << "\n\t\t\t],";
            }

            ss << "\n\t\t],\n";
        }
        ss << "\t];\n";
    }

    // Print PSQTs
    for (int friendEnemy = 0; friendEnemy < 2; ++friendEnemy)
    {
        auto friendEnemyIndexOffset = friendEnemy * psqtIndexCount / 2;
        auto friendEnemyPrefix = friendEnemy == 0 ? "" : "Enemy";

        // Pawns
        {
            const int piece = 0;
            for (int phase = 0; phase <= 1; ++phase)
            {
                for (int bucket = 0; bucket < PSQTBucketCount; ++bucket)
                {
                    if (bucket == 0)
                    {
                        ss << "\n\tinternal static readonly short[][] " << (phase == 0 ? "MiddleGame" : "EndGame") << friendEnemyPrefix << names[piece] << "Table =\n\t[\n";
                    }

                    ss << "\t\t[\n\t\t\t";

                    ss << "   0,\t   0,\t   0,\t   0,\t   0,\t   0,\t   0,\t   0,\n\t\t\t";

                    for (int square = 0; square < 48; ++square)
                    {
                        ss << std::setw(4) << std::round(parameters[friendEnemyIndexOffset + 48 * bucket + square][phase] - psqtPieceValues[friendEnemy][phase][bucket][phase * 6]) << ",";
                        if (square % 8 == 7)
                            ss << "\n\t\t";
                        if (square != 47)
                            ss << "\t";
                    }
                    ss << "\t   0,\t   0,\t   0,\t   0,\t   0,\t   0,\t   0,\t   0," << std::endl;
                    ss << "\t\t],\n";

                    if (bucket == PSQTBucketCount - 1)
                    {
                        ss << "\t];\n";
                    }
                }
            }
        }

        // Pieces
        for (int piece = 1; piece < 6; ++piece)
        {
            for (int phase = 0; phase <= 1; ++phase)
            {
                for (int bucket = 0; bucket < PSQTBucketCount; ++bucket)
                {
                    if (bucket == 0)
                    {
                        ss << "\n\tinternal static readonly short[][] " << (phase == 0 ? "MiddleGame" : "EndGame") << friendEnemyPrefix << names[piece] << "Table =\n\t[\n";
                    }

                    ss << "\t\t[\n\t\t\t";

                    for (int square = 0; square < 64; ++square)
                    {
                        ss << std::setw(4) << std::round(parameters[friendEnemyIndexOffset + (48 * PSQTBucketCount) + // 64 - 16, since we're only tuning 48 pawn values
                                                                    (64 * PSQTBucketCount) * (piece - 1) +            // piece - 1 since we already took pawns into account
                                                                    (64 * bucket) + square][phase] -
                                                         psqtPieceValues[friendEnemy][phase][bucket][piece + phase * 6])
                           << ",";
                        if (square % 8 == 7)
                            ss << "\n\t\t";
                        if (square != 63)
                            ss << "\t";
                    }
                    ss << "],\n";

                    if (bucket == PSQTBucketCount - 1)
                    {
                        ss << "\t];\n";
                    }
                }
            }
        }
    }

    ss << "}\n\n";

    // No console output
    // std::cout << ss.str();

    time_t rawtime;
    struct tm *timeInfo;

    time(&rawtime);
    timeInfo = localtime(&rawtime);

    std::string filename = "TunableEvalParameters-" + std::to_string(print_counter) + ".cs";
    std::ofstream file(filename, std::ofstream::out | std::ofstream::trunc);

    if (file.is_open())
    {
        file << "// " << timeInfo->tm_year + 1900 << "-" << timeInfo->tm_mon + 1 << "-" << timeInfo->tm_mday
             << " " << timeInfo->tm_hour << ":" << timeInfo->tm_min << ":" << timeInfo->tm_sec
             << "  " << print_counter << std::endl
             << std::endl
             << "using static Lynx.Utils;\n"
             << std::endl
             << "namespace Lynx;\n"
             << std::endl
             << "#pragma warning disable IDE0055, IDE1006 // Discard formatting and naming styles\n"
             << std::endl
             << "internal static class TunableEvalParameters\n"
             << "{\n";

        file << ss.rdbuf();

        file.close();
    }
    else
    {
        std::cout << "Error opening " + filename << std::endl;
    }
}

static void print_psqts_cpp(const parameters_t &parameters, const std::array<std::array<tune_t, 12>, PSQTBucketCount> &existingPieceValues, bool isFinal = false)
{
    ++print_counter;

    std::stringstream ss;
    std::string names[] = {"Pawn", "Knight", "Bishop", "Rook", "Queen", "King"};
    std::array<std::array<std::array<std::array<tune_t, 12>, PSQTBucketCount>, 2>, 2> psqtPieceValues;

    ss << "constexpr static std::array<std::array<std::array<int, 12>, PSQTBucketCount>, 2> PieceValue = {{\n\n";

    for (int friendEnemy = 0; friendEnemy < 2; ++friendEnemy)
    {
        auto friendEnemyIndexOffset = friendEnemy * psqtIndexCount / 2;

        ss << "\t{{\n\t";
        // Extract and print pieces values
        for (int bucket = 0; bucket < PSQTBucketCount; ++bucket)
        {
            ss << "\t{\n";

            for (int phase = 0; phase <= 1; ++phase)
            {
                ss << "\t\t\t";

                // Pawns
                {
                    tune_t pawnSum = 0;

                    for (int square = 0; square < 48; ++square)
                    {
                        pawnSum += parameters[friendEnemyIndexOffset + 48 * bucket + square][phase];
                    }

                    auto average = (pawnSum / 48.0);
                    auto pieceIndex = phase * 6;

                    psqtPieceValues[friendEnemy][phase][bucket][pieceIndex] = average;

                    auto value = std::round(
                        average +
                        (friendEnemy == 0
                             ? existingPieceValues[bucket][pieceIndex]
                             : 0));

                    ss << std::showpos << value << std::noshowpos << ", ";
                }

                for (int piece = 1; piece < 5; ++piece)
                {
                    tune_t sum = 0;

                    for (int square = 0; square < 64; ++square)
                    {
                        sum += parameters
                            [friendEnemyIndexOffset +
                             (48 * PSQTBucketCount) +               // 64 - 16, since we're only tuning 48 pawn values
                             (64 * PSQTBucketCount) * (piece - 1) + // piece - 1 since we already took pawns into account
                             (64 * bucket) +
                             square]
                            [phase];
                    }

                    auto average = (sum / 64.0);
                    auto pieceIndex = piece + phase * 6;

                    psqtPieceValues[friendEnemy][phase][bucket][pieceIndex] = average;
                    auto value = std::round(
                        average +
                        (friendEnemy == 0
                             ? existingPieceValues[bucket][pieceIndex]
                             : 0));

                    ss << std::showpos << value << std::noshowpos << ", ";
                }

                // Kings
                auto kingIndex = 5 + phase * 6;
                psqtPieceValues[friendEnemy][phase][bucket][kingIndex] = 0;
                auto extraKingValues = friendEnemy == 0 ? existingPieceValues[bucket][kingIndex] : 0;

                ss << "// (" << std::round(psqtPieceValues[friendEnemy][phase][bucket][kingIndex] + extraKingValues) << ") - bucket " << bucket << "\n";
            }
            ss << "\t\t},\n\t";
        }

        ss << "}},\n";
    }

    ss << "}};\n";

    if (isFinal)
    {
        std::cout << ss.str();
    }
    // Print PSQTs

    for (int friendEnemy = 0; friendEnemy < 2; ++friendEnemy)
    {
        auto friendEnemyIndexOffset = friendEnemy * psqtIndexCount / 2;
        auto friendEnemyPrefix = friendEnemy == 0 ? "" : "Enemy";

        // Pawns
        {
            const int piece = 0;
            for (int phase = 0; phase <= 1; ++phase)
            {
                for (int bucket = 0; bucket < PSQTBucketCount; ++bucket)
                {
                    if (bucket == 0)
                    {
                        ss << "\nconstexpr static std::array<std::array<int, 64>, PSQTBucketCount> " << (phase == 0 ? "MiddleGame" : "EndGame") << friendEnemyPrefix << names[piece] << "Table = {\n\t{\n\t";
                    }

                    ss << "\t{\n\t\t\t";

                    ss << "   0,\t   0,\t   0,\t   0,\t   0,\t   0,\t   0,\t   0,\n\t\t\t";

                    for (int square = 0; square < 48; ++square)
                    {
                        ss << std::setw(4) << std::round(parameters[friendEnemyIndexOffset + 48 * bucket + square][phase] - psqtPieceValues[friendEnemy][phase][bucket][phase * 6]) << ",";
                        if (square % 8 == 7)
                            ss << "\n\t\t";
                        if (square != 47)
                            ss << "\t";
                    }

                    ss << "\t   0,\t   0,\t   0,\t   0,\t   0,\t   0,\t   0,\t   0\t//\n"
                       << std::endl;
                    ss << "\t\t},\n\t";

                    if (bucket == PSQTBucketCount - 1)
                    {
                        ss << "}};\n";
                    }
                }
            }
        }

        // Pieces
        for (int piece = 1; piece < 6; ++piece)
        {
            for (int phase = 0; phase <= 1; ++phase)
            {
                for (int bucket = 0; bucket < PSQTBucketCount; ++bucket)
                {
                    if (bucket == 0)
                    {
                        ss << "\nconstexpr static std::array<std::array<int, 64>, PSQTBucketCount> " << (phase == 0 ? "MiddleGame" : "EndGame") << friendEnemyPrefix << names[piece] << "Table = {\n\t{\n\t";
                    }

                    ss << "\t{\n\t\t\t";

                    for (int square = 0; square < 64; ++square)
                    {
                        ss << std::setw(4) << std::round(parameters[friendEnemyIndexOffset + (48 * PSQTBucketCount) + // 64 - 16, since we're only tuning 48 pawn values
                                                                    (64 * PSQTBucketCount) * (piece - 1) +            // piece - 1 since we already took pawns into account
                                                                    (64 * bucket) + square][phase] -
                                                         psqtPieceValues[friendEnemy][phase][bucket][piece + phase * 6]); // We substract the 16 non-tuned pawn valeus
                        if (square != 63)
                        {
                            ss << ",";

                            if (square % 8 == 7)
                                ss << "\n\t\t";

                            ss << "\t";
                        }
                    }

                    ss << "\t//\n\t\t},\n\t";

                    if (bucket == PSQTBucketCount - 1)
                    {
                        ss << "}};\n";
                    }
                }
            }
        }
    }
    ss << std::endl;

    // No console output
    // std::cout << ss.str();

    time_t rawtime;
    struct tm *timeInfo;

    time(&rawtime);
    timeInfo = localtime(&rawtime);

    std::string filename = "tunable_eval_terms-" + std::to_string(print_counter) + ".cpp";
    std::ofstream file(filename, std::ofstream::out | std::ofstream::trunc);

    if (file.is_open())
    {
        file << "// " << timeInfo->tm_year + 1900 << "-" << timeInfo->tm_mon + 1 << "-" << timeInfo->tm_mday
             << " " << timeInfo->tm_hour << ":" << timeInfo->tm_min << ":" << timeInfo->tm_sec
             << "  " << print_counter << std::endl
             << std::endl
             << "#pragma once\n"
             << "#include <array>\n"
             << "#include \"lynx_tunable.hpp\"\n"
             << std::endl;

        file << ss.rdbuf()
             << std::endl;

        file.close();
    }
    else
    {
        std::cout << "Error opening " + filename << std::endl;
    }
}
