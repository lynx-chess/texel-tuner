#include "../base.h"
#include "../config.h"
#include "./lynx_constants.hpp"
#include "./lynx_tunable.hpp"
#include "lynx_tunable_eval_terms.hpp"
#include "../external/chess.hpp"
#include "../external/builtin.hpp"
#include <algorithm>
#include <cassert>
#include <array>
#include <bit>
#include <string>
#include <cmath>

using u64 = uint64_t;

constexpr int enemyKingBaseIndex = psqtIndexCount / 2;
const static int numParameters = psqtIndexCount +
                                 // DoubledPawnPenalty.size
                                 IsolatedPawnPenalty.size +
                                 OpenFileRookBonus.size +
                                 SemiOpenFileRookBonus.size +
                                 SemiOpenFileKingPenalty.size +
                                 OpenFileKingPenalty.size +
                                 KingShieldBonus.size +
                                 BishopPairBonus.size +
                                 BishopInUnblockedLongDiagonalBonus.size +
                                 BishopRookThreatsBonus.size +
                                 BishopQueenThreatsBonus.size +
                                 PieceAttackedByPawnPenalty.size +

                                 // Arrays
                                 FriendlyKingInFrontOfPassedPawnBonus.tunableSize +
                                 PawnPhalanxBonus.tunableSize +
                                 ConnectedRooksBonus.tunableSize +
                                 PawnIslandsBonus.tunableSize +
                                 BadBishop_SameColorPawnsPenalty.tunableSize +
                                 BadBishop_BlockedCentralPawnsPenalty.tunableSize +
                                 CheckBonus.tunableSize +
                                 FriendlyKingDistanceToPassedPawnBonus.tunableSize + // 7, removing start
                                 EnemyKingDistanceToPassedPawnPenalty.tunableSize +  // 7, removing start
                                 VirtualKingMobilityBonus.tunableSize +              // 28
                                 KnightMobilityBonus.tunableSize +                   // 9
                                 BishopMobilityBonus.tunableSize +                   // 14, removing end
                                 RookMobilityBonus.tunableSize +                     // 15
                                 QueenMobilityBonus.tunableSize +

                                 // Bucketed arrays
                                 PassedPawnBonus.size +                         // PSQTBucketCount * 6, removing 1 rank values
                                 PassedPawnEnemyBonus.size +                    // PSQTBucketCount * 6, removing 1 rank values
                                 PassedPawnBonusNoEnemiesAheadBonus.size +      // PSQTBucketCount * 6, removing 1 rank values
                                 PassedPawnBonusNoEnemiesAheadEnemyBonus.size + // PSQTBucketCount * 6, removing 1 rank values
                                 PieceProtectedByPawnBonus.size;                // PSQTBucketCount * 6, removing 1 rank values

class Lynx
{

public:
    constexpr static bool includes_additional_score = false;
    // constexpr static bool includes_additional_score = true;
    constexpr static bool supports_external_chess_eval = true;

    static parameters_t get_initial_parameters()
    {
        parameters_t result{};

        assert(MiddleGamePositionalWhiteTables.size() == 2);
        assert(MiddleGamePositionalWhiteTables[0].size() == 6);
        assert(MiddleGamePositionalWhiteTables[1].size() == 6);

        assert(EndGamePositionalWhiteTables.size() == 2);
        assert(EndGamePositionalWhiteTables[0].size() == 6);
        assert(EndGamePositionalWhiteTables[1].size() == 6);

        for (int friendEnemy = 0; friendEnemy < 2; ++friendEnemy)
        {
            for (int bucket = 0; bucket < PSQTBucketCount; ++bucket)
            {
                for (int p = 0; p < 6; ++p)
                {
                    for (int sq = 0; sq < 64; ++sq)
                    {
                        assert(MiddleGamePositionalTables(friendEnemy, bucket, p, sq) == MiddleGamePositionalWhiteTables[friendEnemy][p][bucket][sq]);
                        assert(MiddleGamePositionalTables(friendEnemy, bucket, p + 6, sq) == -MiddleGamePositionalWhiteTables[friendEnemy][p][bucket][sq ^ 56]);
                        assert(MiddleGamePositionalTables(friendEnemy, bucket, p, sq) == -MiddleGamePositionalTables(friendEnemy, bucket, p + 6, sq ^ 56));
                    }
                }
            }
        }

        auto add_piece_values = [&](int piece, int start, int end, int offset = 0)
        {
            for (int friendEnemy = 0; friendEnemy < 2; ++friendEnemy)
            {
                for (int bucket = 0; bucket < PSQTBucketCount; ++bucket)
                {
                    for (int square = start; square < end; ++square)
                    {
                        result.push_back({(double)MiddleGamePositionalTables(friendEnemy, bucket, piece, square) + PieceValue[friendEnemy][bucket][piece + offset],
                                          (double)EndGamePositionalTables(friendEnemy, bucket, piece, square) + PieceValue[friendEnemy][bucket][piece + 5 + offset]});
                    }
                }
            }
        };

        add_piece_values(0, 8, 56);             // Pawns
        for (int piece = 1; piece < 5; ++piece) // N, B, R, Q
        {
            add_piece_values(piece, 0, 64);
        }
        add_piece_values(5, 0, 64, 0); // Kings

        // DoubledPawnPenalty.add(result);
        IsolatedPawnPenalty.add(result);
        OpenFileRookBonus.add(result);
        SemiOpenFileRookBonus.add(result);
        SemiOpenFileKingPenalty.add(result);
        OpenFileKingPenalty.add(result);
        KingShieldBonus.add(result);
        BishopPairBonus.add(result);
        BishopInUnblockedLongDiagonalBonus.add(result);
        BishopRookThreatsBonus.add(result);
        BishopQueenThreatsBonus.add(result);
        PieceAttackedByPawnPenalty.add(result);

        // Arrays
        FriendlyKingInFrontOfPassedPawnBonus.add(result);
        PawnPhalanxBonus.add(result);
        ConnectedRooksBonus.add(result);
        PawnIslandsBonus.add(result);
        BadBishop_SameColorPawnsPenalty.add(result);
        BadBishop_BlockedCentralPawnsPenalty.add(result);
        CheckBonus.add(result);

        FriendlyKingDistanceToPassedPawnBonus.add(result);
        EnemyKingDistanceToPassedPawnPenalty.add(result);
        VirtualKingMobilityBonus.add(result);
        KnightMobilityBonus.add(result);
        BishopMobilityBonus.add(result);
        RookMobilityBonus.add(result);
        QueenMobilityBonus.add(result);

        // Bucketed arrays
        PassedPawnBonus.add(result);
        PassedPawnEnemyBonus.add(result);
        PassedPawnBonusNoEnemiesAheadBonus.add(result);
        PassedPawnBonusNoEnemiesAheadEnemyBonus.add(result);
        PieceProtectedByPawnBonus.add(result);

        assert(PassedPawnBonus.bucketTunableSize == 6);
        assert(PassedPawnEnemyBonus.bucketTunableSize == 6);
        assert(PassedPawnBonusNoEnemiesAheadBonus.bucketTunableSize == 6);
        assert(PassedPawnBonusNoEnemiesAheadEnemyBonus.bucketTunableSize == 6);
        assert(PieceProtectedByPawnBonus.bucketTunableSize == 5);
        assert(ConnectedRooksBonus.tunableSize == 8);
        assert(FriendlyKingDistanceToPassedPawnBonus.tunableSize == 7);
        assert(EnemyKingDistanceToPassedPawnPenalty.tunableSize == 7);
        assert(VirtualKingMobilityBonus.tunableSize == 28);
        assert(KnightMobilityBonus.tunableSize == 9);
        assert(BishopMobilityBonus.tunableSize == 14);
        assert(RookMobilityBonus.tunableSize == 15);
        assert(QueenMobilityBonus.tunableSize == 28);
        assert(FriendlyKingInFrontOfPassedPawnBonus.tunableSize == 6);

        std::cout << result.size() << " == " << numParameters << std::endl;
        assert(result.size() == numParameters);

        return result;
    }

    static EvalResult get_fen_eval_result(const std::string &fen)
    {
        chess::Board board;
        board.setFen(fen);
        return get_external_eval_result(board);
    }

    static EvalResult get_external_eval_result(const chess::Board &board);

    static int NormalizeScore(int score);

    static std::array<std::array<tune_t, 12>, PSQTBucketCount> extract_mobility_offset(const parameters_t &parameters, bool isInitial)
    {
        std::array<std::array<tune_t, 12>, PSQTBucketCount> mobilityPieceValues;

        auto knightMobility = KnightMobilityBonus.extract_offset(parameters);
        auto bishopMobility = BishopMobilityBonus.extract_offset(parameters);
        auto rookMobility = RookMobilityBonus.extract_offset(parameters);
        auto queenMobility = QueenMobilityBonus.extract_offset(parameters);

        for (int b = 0; b < PSQTBucketCount; ++b)
        {
            mobilityPieceValues[b].fill(0);

            if (isInitial)
            {
                continue;
            }

            mobilityPieceValues[b][KnightMobilityBonus.pieceIndex] = knightMobility[0];
            mobilityPieceValues[b][KnightMobilityBonus.pieceIndex + 6] = knightMobility[1];

            mobilityPieceValues[b][BishopMobilityBonus.pieceIndex] = bishopMobility[0];
            mobilityPieceValues[b][BishopMobilityBonus.pieceIndex + 6] = bishopMobility[1];

            mobilityPieceValues[b][RookMobilityBonus.pieceIndex] = rookMobility[0];
            mobilityPieceValues[b][RookMobilityBonus.pieceIndex + 6] = rookMobility[1];

            mobilityPieceValues[b][QueenMobilityBonus.pieceIndex] = queenMobility[0];
            mobilityPieceValues[b][QueenMobilityBonus.pieceIndex + 6] = queenMobility[1];
        }

        return mobilityPieceValues;
    }

    static void print_parameters(const parameters_t &parameters, bool isInitial = false, bool isFinal = false)
    {
        auto mobilityPieceValues = extract_mobility_offset(parameters, isInitial);

        print_psqts_cpp(parameters, mobilityPieceValues, true);
        if (isFinal)
        {
            std::cout << "------------------------------------------------------------------------" << std::endl;
        }
        print_cpp_parameters(parameters, mobilityPieceValues, isFinal);
        if (isFinal)
        {
            std::cout << "------------------------------------------------------------------------" << std::endl;
        }

        print_psqts_csharp(parameters, mobilityPieceValues);
        if (isFinal)
        {
            std::cout << "------------------------------------------------------------------------" << std::endl;
        }
        print_csharp_parameters(parameters, mobilityPieceValues, isFinal);
        if (isFinal)
        {
            std::cout << "------------------------------------------------------------------------" << std::endl;
        }
    }

    static void print_step_parameters(const parameters_t &parameters)
    {
        auto mobilityPieceValues = extract_mobility_offset(parameters, false);

        print_psqts_cpp(parameters, mobilityPieceValues, true);
        // print_cpp_parameters(parameters, mobilityPieceValues);
    }

    static void print_csharp_parameters(const parameters_t &parameters, const std::array<std::array<tune_t, 12>, PSQTBucketCount> &mobilityPieceValues, bool isFinal = false)
    {
        std::stringstream ss;
        std::string name;

        // name = NAME(DoubledPawnPenalty);
        // DoubledPawnPenalty.to_json(parameters, ss, name);

        ss << "public static class EvaluationParams" << std::endl
           << "{" << std::endl;

        name = NAME(IsolatedPawnPenalty);
        IsolatedPawnPenalty.to_csharp(parameters, ss, name);

        name = NAME(OpenFileRookBonus);
        OpenFileRookBonus.to_csharp(parameters, ss, name);

        name = NAME(SemiOpenFileRookBonus);
        SemiOpenFileRookBonus.to_csharp(parameters, ss, name);

        name = NAME(SemiOpenFileKingPenalty);
        SemiOpenFileKingPenalty.to_csharp(parameters, ss, name);

        name = NAME(OpenFileKingPenalty);
        OpenFileKingPenalty.to_csharp(parameters, ss, name);

        name = NAME(KingShieldBonus);
        KingShieldBonus.to_csharp(parameters, ss, name);

        name = NAME(BishopPairBonus);
        BishopPairBonus.to_csharp(parameters, ss, name);

        name = NAME(BishopInUnblockedLongDiagonalBonus);
        BishopInUnblockedLongDiagonalBonus.to_csharp(parameters, ss, name);

        name = NAME(BishopRookThreatsBonus);
        BishopRookThreatsBonus.to_csharp(parameters, ss, name);

        name = NAME(BishopQueenThreatsBonus);
        BishopQueenThreatsBonus.to_csharp(parameters, ss, name);

        name = NAME(PieceAttackedByPawnPenalty);
        PieceAttackedByPawnPenalty.to_csharp(parameters, ss, name);

        // Arrays
        name = NAME(FriendlyKingInFrontOfPassedPawnBonus);
        FriendlyKingInFrontOfPassedPawnBonus.to_csharp(parameters, ss, name);

        name = NAME(PawnPhalanxBonus);
        PawnPhalanxBonus.to_csharp(parameters, ss, name);

        name = NAME(ConnectedRooksBonus);
        ConnectedRooksBonus.to_csharp(parameters, ss, name);

        name = NAME(PawnIslandsBonus);
        PawnIslandsBonus.to_csharp(parameters, ss, name);

        name = NAME(BadBishop_SameColorPawnsPenalty);
        BadBishop_SameColorPawnsPenalty.to_csharp(parameters, ss, name);

        name = NAME(BadBishop_BlockedCentralPawnsPenalty);
        BadBishop_BlockedCentralPawnsPenalty.to_csharp(parameters, ss, name);

        name = NAME(CheckBonus);
        CheckBonus.to_csharp(parameters, ss, name);

        name = NAME(FriendlyKingDistanceToPassedPawnBonus);
        FriendlyKingDistanceToPassedPawnBonus.to_csharp(parameters, ss, name);

        name = NAME(EnemyKingDistanceToPassedPawnPenalty);
        EnemyKingDistanceToPassedPawnPenalty.to_csharp(parameters, ss, name);

        name = NAME(VirtualKingMobilityBonus);
        VirtualKingMobilityBonus.to_csharp(parameters, ss, name);

        name = NAME(KnightMobilityBonus);
        KnightMobilityBonus.to_csharp(parameters, ss, name, mobilityPieceValues);

        name = NAME(BishopMobilityBonus);
        BishopMobilityBonus.to_csharp(parameters, ss, name, mobilityPieceValues);

        name = NAME(RookMobilityBonus);
        RookMobilityBonus.to_csharp(parameters, ss, name, mobilityPieceValues);

        name = NAME(QueenMobilityBonus);
        QueenMobilityBonus.to_csharp(parameters, ss, name, mobilityPieceValues);

        // Bucketed arrays
        name = NAME(PassedPawnBonus);
        PassedPawnBonus.to_csharp(parameters, ss, name);

        name = NAME(PassedPawnEnemyBonus);
        PassedPawnEnemyBonus.to_csharp(parameters, ss, name);

        name = NAME(PassedPawnBonusNoEnemiesAheadBonus);
        PassedPawnBonusNoEnemiesAheadBonus.to_csharp(parameters, ss, name);

        name = NAME(PassedPawnBonusNoEnemiesAheadEnemyBonus);
        PassedPawnBonusNoEnemiesAheadEnemyBonus.to_csharp(parameters, ss, name);

        name = NAME(PieceProtectedByPawnBonus);
        PieceProtectedByPawnBonus.to_csharp(parameters, ss, name);

        if (isFinal)
        {
            std::cout << ss.str() << std::endl;
        }

        std::string filename = "TunableEvalParameters-" + std::to_string(print_counter) + ".cs";
        std::ofstream file(filename, std::ofstream::out | std::ofstream::app | std::ofstream::ate);

        if (file.is_open())
        {
            file << ss.rdbuf();

            file << "}\n"
                 << std::endl
                 << "#pragma warning restore IDE0055, IDE1006 // Discard formatting and naming styles\n";
        }
        file.close();
    }

    static void print_cpp_parameters(const parameters_t &parameters, const std::array<std::array<tune_t, 12>, PSQTBucketCount> &mobilityPieceValues, bool isFinal = false)
    {
        std::stringstream ss;
        std::string name;

        // name = NAME(DoubledPawnPenalty);
        // DoubledPawnPenalty.to_json(parameters, ss, name);

        name = NAME(IsolatedPawnPenalty);
        IsolatedPawnPenalty.to_cpp(parameters, ss, name);

        name = NAME(OpenFileRookBonus);
        OpenFileRookBonus.to_cpp(parameters, ss, name);

        name = NAME(SemiOpenFileRookBonus);
        SemiOpenFileRookBonus.to_cpp(parameters, ss, name);

        name = NAME(SemiOpenFileKingPenalty);
        SemiOpenFileKingPenalty.to_cpp(parameters, ss, name);

        name = NAME(OpenFileKingPenalty);
        OpenFileKingPenalty.to_cpp(parameters, ss, name);

        name = NAME(KingShieldBonus);
        KingShieldBonus.to_cpp(parameters, ss, name);

        name = NAME(BishopPairBonus);
        BishopPairBonus.to_cpp(parameters, ss, name);

        name = NAME(BishopInUnblockedLongDiagonalBonus);
        BishopInUnblockedLongDiagonalBonus.to_cpp(parameters, ss, name);

        name = NAME(BishopRookThreatsBonus);
        BishopRookThreatsBonus.to_cpp(parameters, ss, name);

        name = NAME(BishopQueenThreatsBonus);
        BishopQueenThreatsBonus.to_cpp(parameters, ss, name);

        name = NAME(PieceAttackedByPawnPenalty);
        PieceAttackedByPawnPenalty.to_cpp(parameters, ss, name);

        // Arrays
        name = NAME(FriendlyKingInFrontOfPassedPawnBonus);
        FriendlyKingInFrontOfPassedPawnBonus.to_cpp(parameters, ss, name);
        ss << "\n";

        name = NAME(PawnPhalanxBonus);
        PawnPhalanxBonus.to_cpp(parameters, ss, name);
        ss << "\n";

        name = NAME(ConnectedRooksBonus);
        ConnectedRooksBonus.to_cpp(parameters, ss, name);
        ss << "\n";

        name = NAME(PawnIslandsBonus);
        PawnIslandsBonus.to_cpp(parameters, ss, name);
        ss << "\n";

        name = NAME(BadBishop_SameColorPawnsPenalty);
        BadBishop_SameColorPawnsPenalty.to_cpp(parameters, ss, name);
        ss << "\n";

        name = NAME(BadBishop_BlockedCentralPawnsPenalty);
        BadBishop_BlockedCentralPawnsPenalty.to_cpp(parameters, ss, name);
        ss << "\n";

        name = NAME(CheckBonus);
        CheckBonus.to_cpp(parameters, ss, name);
        ss << "\n";

        name = NAME(FriendlyKingDistanceToPassedPawnBonus);
        FriendlyKingDistanceToPassedPawnBonus.to_cpp(parameters, ss, name);

        name = NAME(EnemyKingDistanceToPassedPawnPenalty);
        EnemyKingDistanceToPassedPawnPenalty.to_cpp(parameters, ss, name);

        name = NAME(VirtualKingMobilityBonus);
        VirtualKingMobilityBonus.to_cpp(parameters, ss, name);

        name = NAME(KnightMobilityBonus);
        KnightMobilityBonus.to_cpp(parameters, ss, name, mobilityPieceValues);

        name = NAME(BishopMobilityBonus);
        BishopMobilityBonus.to_cpp(parameters, ss, name, mobilityPieceValues);

        name = NAME(RookMobilityBonus);
        RookMobilityBonus.to_cpp(parameters, ss, name, mobilityPieceValues);

        name = NAME(QueenMobilityBonus);
        QueenMobilityBonus.to_cpp(parameters, ss, name, mobilityPieceValues);

        // Bucketed arrays
        name = NAME(PassedPawnBonus);
        PassedPawnBonus.to_cpp(parameters, ss, name);

        name = NAME(PassedPawnEnemyBonus);
        PassedPawnEnemyBonus.to_cpp(parameters, ss, name);

        name = NAME(PassedPawnBonusNoEnemiesAheadBonus);
        PassedPawnBonusNoEnemiesAheadBonus.to_cpp(parameters, ss, name);

        name = NAME(PassedPawnBonusNoEnemiesAheadEnemyBonus);
        PassedPawnBonusNoEnemiesAheadEnemyBonus.to_cpp(parameters, ss, name);

        name = NAME(PieceProtectedByPawnBonus);
        PieceProtectedByPawnBonus.to_cpp(parameters, ss, name);

        if (isFinal)
        {
            std::cout << ss.str() << std::endl;
        }

        std::string filename = "tunable_eval_terms-" + std::to_string(print_counter) + ".cpp";
        std::ofstream file(filename, std::ofstream::out | std::ofstream::app | std::ofstream::ate);

        if (file.is_open())
        {
            file << ss.rdbuf();
        }
        file.close();
    }
};

static inline parameters_t initialParameters = Lynx::get_initial_parameters();

void IncrementCoefficients(coefficients_t &coefficients, int index, const chess::Color &color, int increment = 1)
{
    coefficients[index] += color == chess::Color::WHITE
                               ? increment
                               : -increment;
}

chess::U64 GetPieceSwappingEndianness(const chess::Board &board, const chess::PieceType &piece, const chess::Color &color)
{
    return __builtin_bswap64(board.pieces(piece, color).getBits());
}

void ResetLS1B(std::uint64_t &board)
{
    board &= (board - 1);
}

int PawnAdditionalEvaluation(int squareIndex, int pieceIndex, int bucket, int oppositeSideBucket, int sameSideKingSquare, int oppositeSideKingSquare, const chess::Board &board, const chess::Color &color, coefficients_t &coefficients)
{
    int packedBonus = 0;
    // auto doublePawnsCount = chess::builtin::popcount(GetPieceSwappingEndianness(board, chess::PieceType::PAWN, color) & (FileMasks[squareIndex]));
    // if (doublePawnsCount > 1)
    // {
    //     packedBonus += doublePawnsCount * DoubledPawnPenalty_Packed;
    //     IncrementCoefficients(coefficients, DoubledPawnPenalty_Index, color);
    // }

    const auto whitePawns = GetPieceSwappingEndianness(board, chess::PieceType::PAWN, chess::Color::WHITE);
    const auto blackPawns = GetPieceSwappingEndianness(board, chess::PieceType::PAWN, chess::Color::BLACK);
    const auto whitePieces = __builtin_bswap64(board.us(chess::Color::WHITE).getBits());
    const auto blackPieces = __builtin_bswap64(board.us(chess::Color::BLACK).getBits());

    auto sameSidePawns = whitePawns;
    auto opposideSidePawns = blackPawns;
    auto oppositeSidePieces = blackPieces;
    auto passedPawnMask = WhitePassedPawnMasks[squareIndex];
    auto rank = Rank[squareIndex];
    auto sameSideKingRank = Rank[sameSideKingSquare];
    auto oppositeSideKingRank = Rank[oppositeSideKingSquare];

    if (color == chess::Color::BLACK)
    {
        sameSidePawns = blackPawns;
        opposideSidePawns = whitePawns;
        oppositeSidePieces = whitePieces;
        passedPawnMask = BlackPassedPawnMasks[squareIndex];
        rank = 7 - rank;
        sameSideKingRank = 7 - sameSideKingRank;
        oppositeSideKingRank = 7 - oppositeSideKingRank;
    }

    // Isolated pawn
    if ((sameSidePawns & IsolatedPawnMasks[squareIndex]) == 0) // isIsolatedPawn
    {
        packedBonus += IsolatedPawnPenalty.packed;
        IncrementCoefficients(coefficients, IsolatedPawnPenalty.index, color);
    }

    // Passed pawn
    if ((opposideSidePawns & passedPawnMask) == 0)
    {
        packedBonus += PassedPawnBonus.packed(bucket, rank);
        IncrementCoefficients(coefficients, PassedPawnBonus.index(bucket, rank - PassedPawnBonus.start), color); // There's no coefficient for rank 0

        packedBonus += PassedPawnEnemyBonus.packed(oppositeSideBucket, rank);
        IncrementCoefficients(coefficients, PassedPawnEnemyBonus.index(oppositeSideBucket, rank - PassedPawnEnemyBonus.start), color); // There's no coefficient for rank 0

        // Passed pawn without opponent pieces ahead (in its passed pawn mask)
        if ((oppositeSidePieces & passedPawnMask) == 0)
        {
            packedBonus += PassedPawnBonusNoEnemiesAheadBonus.packed(bucket, rank);
            IncrementCoefficients(coefficients, PassedPawnBonusNoEnemiesAheadBonus.index(bucket, rank - PassedPawnBonusNoEnemiesAheadBonus.start), color); // There's no coefficient for rank 0

            packedBonus += PassedPawnBonusNoEnemiesAheadEnemyBonus.packed(oppositeSideBucket, rank);
            IncrementCoefficients(coefficients, PassedPawnBonusNoEnemiesAheadEnemyBonus.index(oppositeSideBucket, rank - PassedPawnBonusNoEnemiesAheadEnemyBonus.start), color); // There's no coefficient for rank 0
        }

        // King distance to passed pawn
        const auto friendlyKingDistance = ChebyshevDistance(sameSideKingSquare, squareIndex);
        packedBonus += FriendlyKingDistanceToPassedPawnBonus.packed[friendlyKingDistance];
        IncrementCoefficients(coefficients, FriendlyKingDistanceToPassedPawnBonus.index + friendlyKingDistance - FriendlyKingDistanceToPassedPawnBonus.start, color);

        // Enemy king distance to passed pawn
        const auto enemyKingDistance = ChebyshevDistance(oppositeSideKingSquare, squareIndex);
        packedBonus += EnemyKingDistanceToPassedPawnPenalty.packed[enemyKingDistance];
        IncrementCoefficients(coefficients, EnemyKingDistanceToPassedPawnPenalty.index + enemyKingDistance - EnemyKingDistanceToPassedPawnPenalty.start, color);

        // King in front of passed pawn
        if (sameSideKingRank > rank)
        {
            packedBonus += FriendlyKingInFrontOfPassedPawnBonus.packed[sameSideKingRank];
            IncrementCoefficients(coefficients, FriendlyKingInFrontOfPassedPawnBonus.index + sameSideKingRank - FriendlyKingInFrontOfPassedPawnBonus.start, color);
        }

        // // Enemy king in front of passed pawn
        // if(oppositeSideKingRank < rank)
        // {
        //     packedBonus += EnemyKingInFrontOfPassedPawnPenalty.packed;
        //     IncrementCoefficients(coefficients, EnemyKingInFrontOfPassedPawnPenalty.index, color);
        // }
    }

    if (File[squareIndex] != 7 && GetBit(sameSidePawns, squareIndex + 1))
    {
        packedBonus += PawnPhalanxBonus.packed[rank];
        IncrementCoefficients(coefficients, PawnPhalanxBonus.index + rank - PawnPhalanxBonus.start, color);
    }

    return packedBonus;
}

int RookAdditonalEvaluation(int squareIndex, int pieceIndex, int bucket, int oppositeSideKingSquare, const u64 opponentPawnAttacks, const chess::Board &board, const chess::Color &color, coefficients_t &coefficients)
{
    const auto noColorPieceIndex = static_cast<int>(chess::PieceType::ROOK);
    const auto occupancy = __builtin_bswap64(board.occ().getBits());
    const auto attacks = chess::attacks::rook(static_cast<chess::Square>(squareIndex), occupancy).getBits();

    // Mobility
    const auto mobilityCount = chess::builtin::popcount(
        attacks &
        (~__builtin_bswap64(board.us(color).getBits())) &
        (~opponentPawnAttacks));

    int packedBonus = RookMobilityBonus.packed[mobilityCount];
    IncrementCoefficients(coefficients, RookMobilityBonus.index + mobilityCount, color);

    // Open file
    if (((GetPieceSwappingEndianness(board, chess::PieceType::PAWN, chess::Color::WHITE) | GetPieceSwappingEndianness(board, chess::PieceType::PAWN, chess::Color::BLACK)) & FileMasks[squareIndex]) == 0)
    {
        // std::cout << "OpenFileRookBonus" << std::endl;
        IncrementCoefficients(coefficients, OpenFileRookBonus.index, color);
        packedBonus += OpenFileRookBonus.packed;
    }
    else
    {
        // Semi-open file
        if ((GetPieceSwappingEndianness(board, chess::PieceType::PAWN, color) & FileMasks[squareIndex]) == 0)
        {
            packedBonus += SemiOpenFileRookBonus.packed;
            IncrementCoefficients(coefficients, SemiOpenFileRookBonus.index, color);
        }
    }

    // Checks
    const auto enemyKingCheckThreats = chess::attacks::rook(oppositeSideKingSquare, occupancy).getBits();
    const auto checksCount = chess::builtin::popcount(attacks & enemyKingCheckThreats);

    packedBonus += CheckBonus.packed[noColorPieceIndex] * checksCount;
    IncrementCoefficients(coefficients, CheckBonus.index + noColorPieceIndex - CheckBonus.start, color, checksCount);

    // Connected rooks
    if (chess::builtin::popcount(attacks & GetPieceSwappingEndianness(board, chess::PieceType::ROOK, color)) >= 1)
    {
        auto rank = Rank[squareIndex];
        if (color == chess::Color::BLACK)
        {
            rank = 7 - rank;
        }

        packedBonus += ConnectedRooksBonus.packed[rank];
        IncrementCoefficients(coefficients, ConnectedRooksBonus.index - ConnectedRooksBonus.start + rank, color);
    }

    return packedBonus;
}

int KnightAdditionalEvaluation(int squareIndex, int pieceIndex, int bucket, int oppositeSideKingSquare, const u64 opponentPawnAttacks, const chess::Board &board, const chess::Color &color, coefficients_t &coefficients)
{
    const auto noColorPieceIndex = static_cast<int>(chess::PieceType::KNIGHT);
    const auto attacks = chess::attacks::knight(static_cast<chess::Square>(squareIndex)).getBits();

    // Mobility
    const auto mobilityCount = chess::builtin::popcount(
        attacks &
        (~__builtin_bswap64(board.us(color).getBits())) &
        (~opponentPawnAttacks));

    auto packedBonus = KnightMobilityBonus.packed[mobilityCount];
    IncrementCoefficients(coefficients, KnightMobilityBonus.index + mobilityCount, color);

    // Checks
    const auto enemyKingCheckThreats = chess::attacks::knight(oppositeSideKingSquare).getBits();
    const auto checksCount = chess::builtin::popcount(attacks & enemyKingCheckThreats);

    packedBonus += CheckBonus.packed[noColorPieceIndex] * checksCount;
    IncrementCoefficients(coefficients, CheckBonus.index + noColorPieceIndex - CheckBonus.start, color, checksCount);

    // Major threats
    // const auto threatsCount = (board.pieces(chess::PieceType::ROOK, ~color) | board.pieces(chess::PieceType::QUEEN, ~color)).count();
    // packedBonus += BishopMajorThreatsBonus.packed * threatsCount;
    // IncrementCoefficients(coefficients, BishopMajorThreatsBonus.index, color, threatsCount);

    return packedBonus;
}

int BishopAdditionalEvaluation(int squareIndex, int pieceIndex, int bucket, int oppositeSideKingSquare, const u64 opponentPawnAttacks, const chess::Board &board, const chess::Color &color, coefficients_t &coefficients)
{
    const auto noColorPieceIndex = static_cast<int>(chess::PieceType::BISHOP);
    const auto occupancy = __builtin_bswap64(board.occ().getBits());
    const auto attacks = chess::attacks::bishop(static_cast<chess::Square>(squareIndex), occupancy).getBits();

    // Mobility
    const auto mobilityCount = chess::builtin::popcount(
        attacks &
        (~__builtin_bswap64(board.us(color).getBits())) &
        (~opponentPawnAttacks));

    auto packedBonus = BishopMobilityBonus.packed[mobilityCount];
    IncrementCoefficients(coefficients, BishopMobilityBonus.index + mobilityCount, color);

    // Bad bishop - same color pawns
    const auto sameSidePawns = GetPieceSwappingEndianness(board, chess::PieceType::PAWN, color);
    const auto sameColorPawnsCount = chess::builtin::popcount(sameSidePawns &
                                                              (DarkSquares[squareIndex] == 1
                                                                   ? DarkSquaresBitBoard
                                                                   : LightSquaresBitBoard));

    packedBonus += BadBishop_SameColorPawnsPenalty.packed[sameColorPawnsCount];
    IncrementCoefficients(coefficients, BadBishop_SameColorPawnsPenalty.index + sameColorPawnsCount, color);

    // Blocked central pawns
    const auto sameSideCentralPawns = sameSidePawns & CentralFiles;
    const auto pawnBlockerSquares = pieceIndex == static_cast<int>(chess::Piece::WHITEBISHOP)
                                        ? ShiftUp(sameSideCentralPawns)
                                        : ShiftDown(sameSideCentralPawns);

    const auto pawnBlockers = pawnBlockerSquares & __builtin_bswap64(board.them(color).getBits());
    const auto pawnBlockersCount = chess::builtin::popcount(pawnBlockers);

    packedBonus += BadBishop_BlockedCentralPawnsPenalty.packed[pawnBlockersCount];
    IncrementCoefficients(coefficients, BadBishop_BlockedCentralPawnsPenalty.index - BadBishop_BlockedCentralPawnsPenalty.start + pawnBlockersCount, color);

    // Bishop in unblocked long diagonals
    if (chess::builtin::popcount(attacks & CentralSquares) == 2)
    {
        packedBonus += BishopInUnblockedLongDiagonalBonus.packed;
        IncrementCoefficients(coefficients, BishopInUnblockedLongDiagonalBonus.index, color);
    }

    // Checks
    const auto enemyKingCheckThreats = chess::attacks::bishop(static_cast<chess::Square>(oppositeSideKingSquare), occupancy).getBits();
    const auto checksCount = chess::builtin::popcount(attacks & enemyKingCheckThreats);

    packedBonus += CheckBonus.packed[noColorPieceIndex] * checksCount;
    IncrementCoefficients(coefficients, CheckBonus.index + noColorPieceIndex - CheckBonus.start, color, checksCount);

    // Major threats
    const auto rooksThreatsCount = chess::builtin::popcount(attacks & GetPieceSwappingEndianness(board, chess::PieceType::ROOK, ~color));
    packedBonus += BishopRookThreatsBonus.packed * rooksThreatsCount;
    IncrementCoefficients(coefficients, BishopRookThreatsBonus.index, color, rooksThreatsCount);

    const auto queenThreatsCount = chess::builtin::popcount(attacks & GetPieceSwappingEndianness(board, chess::PieceType::QUEEN, ~color));
    packedBonus += BishopQueenThreatsBonus.packed * queenThreatsCount;
    IncrementCoefficients(coefficients, BishopQueenThreatsBonus.index, color, queenThreatsCount);

    return packedBonus;
}

int QueenAdditionalEvaluation(int squareIndex, int bucket, int oppositeSideKingSquare, const u64 opponentPawnAttacks, const chess::Board &board, const chess::Color &color, coefficients_t &coefficients)
{
    const auto noColorPieceIndex = static_cast<int>(chess::PieceType::QUEEN);
    const auto occupancy = __builtin_bswap64(board.occ().getBits());
    const auto attacks = chess::attacks::queen(static_cast<chess::Square>(squareIndex), occupancy).getBits();

    // Mobility
    const auto mobilityCount = chess::builtin::popcount(
        attacks &
        (~__builtin_bswap64(board.us(color).getBits())) &
        (~opponentPawnAttacks));

    auto packedBonus = QueenMobilityBonus.packed[mobilityCount];
    IncrementCoefficients(coefficients, QueenMobilityBonus.index + mobilityCount, color);

    // Checks
    const auto enemyKingCheckThreats = chess::attacks::queen(static_cast<chess::Square>(oppositeSideKingSquare), occupancy).getBits();
    const auto checksCount = chess::builtin::popcount(attacks & enemyKingCheckThreats);

    packedBonus += CheckBonus.packed[noColorPieceIndex] * checksCount;
    IncrementCoefficients(coefficients, CheckBonus.index + noColorPieceIndex - CheckBonus.start, color, checksCount);

    return packedBonus;
}

int KingAdditionalEvaluation(int squareIndex, int bucket, const u64 opponentPawnAttacks, chess::Color kingSide, const chess::Board &board, const int pieceCount[], coefficients_t &coefficients)
{
    // Virtual mobility (as if Queen)
    const auto mobilityCount = chess::builtin::popcount(
        chess::attacks::queen(static_cast<chess::Square>(squareIndex), __builtin_bswap64(board.occ().getBits())).getBits() &
        (~__builtin_bswap64(board.us(kingSide).getBits())) &
        (~opponentPawnAttacks));

    IncrementCoefficients(coefficients, VirtualKingMobilityBonus.index + mobilityCount, kingSide);

    int packedBonus = VirtualKingMobilityBonus.packed[mobilityCount];

    const auto kingSideOffset = kingSide == chess::Color::WHITE ? 0 : 6;

    // Opposite side rooks or queens on the board
    if (pieceCount[9 - kingSideOffset] + pieceCount[10 - kingSideOffset] != 0)
    {
        // King on open file
        if (((GetPieceSwappingEndianness(board, chess::PieceType::PAWN, chess::Color::WHITE) | GetPieceSwappingEndianness(board, chess::PieceType::PAWN, chess::Color::BLACK)) & FileMasks[squareIndex]) == 0) // isOpenFile
        {
            // std::cout << "Open: " << (kingSide == chess::Color::WHITE ? "White" : "Black") << std::endl;
            packedBonus += OpenFileKingPenalty.packed;
            IncrementCoefficients(coefficients, OpenFileKingPenalty.index, kingSide);
        }
        // King on semi-open file
        else if ((GetPieceSwappingEndianness(board, chess::PieceType::PAWN, kingSide) & FileMasks[squareIndex]) == 0) // isSemiOpenFile
        {
            // std::cout << "Semiopen: " << (kingSide == chess::Color::WHITE ? "White" : "Black") << std::endl;
            packedBonus += SemiOpenFileKingPenalty.packed;
            IncrementCoefficients(coefficients, SemiOpenFileKingPenalty.index, kingSide);
        }
    }

    // King shield
    const auto ownPawnsAroundCount = chess::builtin::popcount(
        chess::attacks::king(static_cast<chess::Square>(squareIndex)).getBits() &
        GetPieceSwappingEndianness(board, chess::PieceType::PAWN, kingSide));

    IncrementCoefficients(coefficients, KingShieldBonus.index, kingSide, ownPawnsAroundCount);

    return packedBonus + KingShieldBonus.packed * ownPawnsAroundCount;
}

int PawnIslands(const u64 bitboard)
{
    auto islandCount = 0;
    auto isIsland = false;

    for (int file = 0; file < 8; ++file)
    {
        auto pawnInRank = false;

        for (int rank = 1; rank < 7; ++rank)
        {
            auto squareIndex = rank * 8 + file;

            if (GetBit(bitboard, squareIndex))
            {
                pawnInRank = true;

                if (!isIsland)
                {
                    isIsland = true;
                    ++islandCount;
                }

                break;
            }
        }

        if (!pawnInRank)
        {
            isIsland = false;
        }
    }

    return islandCount;
}

int AdditionalPieceEvaluation(int pieceSquareIndex, int pieceIndex, int bucket, int oppositeSideBucket, int sameSideKingSquare, int oppositeSideKingSquare, const u64 opponentPawnAttacks, const chess::Board &board, const chess::Color &color, coefficients_t &coefficients)
{
    switch (pieceIndex)
    {
    case 0:
    case 6:
        return PawnAdditionalEvaluation(pieceSquareIndex, pieceIndex, bucket, oppositeSideBucket, sameSideKingSquare, oppositeSideKingSquare, board, color, coefficients);

    case 1:
    case 7:
        return KnightAdditionalEvaluation(pieceSquareIndex, pieceIndex, bucket, oppositeSideKingSquare, opponentPawnAttacks, board, color, coefficients);

    case 3:
    case 9:
        return RookAdditonalEvaluation(pieceSquareIndex, pieceIndex, bucket, oppositeSideKingSquare, opponentPawnAttacks, board, color, coefficients);

    case 2:
    case 8:
        return BishopAdditionalEvaluation(pieceSquareIndex, pieceIndex, bucket, oppositeSideKingSquare, opponentPawnAttacks, board, color, coefficients);

    case 4:
    case 10:
        return QueenAdditionalEvaluation(pieceSquareIndex, bucket, oppositeSideKingSquare, opponentPawnAttacks, board, color, coefficients);

    default:
        return 0;
    }
}

[[nodiscard]] int Lynx::NormalizeScore(const int score)
{
    return (score == 0 || score > 27000 || score < -27000)
               ? score
               : score * 100 / EvalNormalizationCoefficient;
}

[[nodiscard]] static int ScaleEvalWith50MovesDrawDistance(const int eval, const int movesWithoutCaptureOrPawnMove)
{
    return eval * (200 - movesWithoutCaptureOrPawnMove) / 200;
}

EvalResult Lynx::get_external_eval_result(const chess::Board &board)
{
    std::vector<std::int16_t> coefficients(numParameters, 0);

    int pieceCount[12] = {}; // Init to zeros

    int packedScore = 0;
    int gamePhase = 0;

    const auto whitePawns = GetPieceSwappingEndianness(board, chess::PieceType::PAWN, chess::Color::WHITE);
    const auto whitePawnAttacks = ShiftUpLeft(whitePawns) | ShiftUpRight(whitePawns);

    const auto blackPawns = GetPieceSwappingEndianness(board, chess::PieceType::PAWN, chess::Color::BLACK);
    const auto blackPawnAttacks = ShiftDownLeft(blackPawns) | ShiftDownRight(blackPawns);

    const auto whiteKing = chess::builtin::lsb(GetPieceSwappingEndianness(board, chess::PieceType::KING, chess::Color::WHITE)).index();
    const auto blackKing = chess::builtin::lsb(GetPieceSwappingEndianness(board, chess::PieceType::KING, chess::Color::BLACK)).index();

    const auto whiteBucket = PSQTBucketLayout[whiteKing];
    const auto blackBucket = PSQTBucketLayout[blackKing ^ 56];

    // White pieces PSQTs and additional eval, except king
    for (int pieceIndex = 0; pieceIndex < 5; ++pieceIndex)
    {
        // Bitboard copy that we 'empty'
        auto bitboard = GetPieceSwappingEndianness(board, chess::PieceType(static_cast<chess::PieceType::underlying>(pieceIndex)), chess::Color::WHITE);
        // std::cout << pieceIndex << "bb: " << bitboard << std::endl;

        // Pieces protected by pawns bonus
        const auto protectedPiecesByWhitePawns = chess::builtin::popcount(whitePawnAttacks & bitboard);
        packedScore += (PieceProtectedByPawnBonus.packed(whiteBucket, pieceIndex) * protectedPiecesByWhitePawns);
        IncrementCoefficients(coefficients, PieceProtectedByPawnBonus.index(whiteBucket, pieceIndex), chess::Color::WHITE, protectedPiecesByWhitePawns);

        while (bitboard != 0)
        {
            const auto pieceSquareIndex = chess::builtin::lsb(bitboard).index();
            ResetLS1B(bitboard);

            packedScore += PackedPositionalTables(0, whiteBucket, pieceIndex, pieceSquareIndex) + PackedPieceValue(0, whiteBucket, pieceIndex) +
                           PackedPositionalTables(1, blackBucket, pieceIndex, pieceSquareIndex) + PackedPieceValue(1, blackBucket, pieceIndex);
            gamePhase += phaseValues[pieceIndex];

            ++pieceCount[pieceIndex];

            packedScore += AdditionalPieceEvaluation(pieceSquareIndex, pieceIndex, whiteBucket, blackBucket, whiteKing, blackKing, blackPawnAttacks, board, chess::Color::WHITE, coefficients);

            if (pieceIndex == 0)
            {
                IncrementCoefficients(coefficients,
                                      (48 * whiteBucket) + pieceSquareIndex - 8,
                                      chess::Color::WHITE);

                IncrementCoefficients(coefficients,
                                      enemyKingBaseIndex + (48 * blackBucket) + pieceSquareIndex - 8,
                                      chess::Color::WHITE);
            }
            else
            {
                IncrementCoefficients(
                    coefficients,
                    (48 * PSQTBucketCount) + (64 * PSQTBucketCount * (pieceIndex - 1)) + (64 * whiteBucket) + pieceSquareIndex,
                    chess::Color::WHITE);

                IncrementCoefficients(
                    coefficients,
                    enemyKingBaseIndex + (48 * PSQTBucketCount) + (64 * PSQTBucketCount * (pieceIndex - 1)) + (64 * blackBucket) + pieceSquareIndex,
                    chess::Color::WHITE);
            }
        }
    }

    // Black pieces PSQTs and additional eval, except king

    for (int pieceIndex = 6; pieceIndex < 11; ++pieceIndex)
    {
        const auto tunerPieceIndex = (pieceIndex - 6); // [0, 5]
        // Bitboard copy that we 'empty'
        auto bitboard = GetPieceSwappingEndianness(board, chess::PieceType(static_cast<chess::PieceType::underlying>(tunerPieceIndex)), chess::Color::BLACK);

        // Pieces protected by pawns bonus
        const auto protectedPiecesByBlackPawns = chess::builtin::popcount(blackPawnAttacks & bitboard);
        packedScore -= (PieceProtectedByPawnBonus.packed(blackBucket, tunerPieceIndex) * protectedPiecesByBlackPawns);
        IncrementCoefficients(coefficients, PieceProtectedByPawnBonus.index(blackBucket, tunerPieceIndex), chess::Color::BLACK, protectedPiecesByBlackPawns);

        while (bitboard != 0)
        {
            const auto pieceSquareIndex = chess::builtin::lsb(bitboard).index();
            ResetLS1B(bitboard);

            packedScore += PackedPositionalTables(0, blackBucket, pieceIndex, pieceSquareIndex) - PackedPieceValue(0, blackBucket, tunerPieceIndex) +
                           PackedPositionalTables(1, whiteBucket, pieceIndex, pieceSquareIndex) - PackedPieceValue(1, whiteBucket, tunerPieceIndex);
            gamePhase += phaseValues[tunerPieceIndex];

            ++pieceCount[pieceIndex];

            packedScore -= AdditionalPieceEvaluation(pieceSquareIndex, pieceIndex, blackBucket, whiteBucket, blackKing, whiteKing, whitePawnAttacks, board, chess::Color::BLACK, coefficients);

            if (pieceIndex == 6)
            {
                IncrementCoefficients(coefficients,
                                      (48 * blackBucket) + (pieceSquareIndex ^ 56) - 8,
                                      chess::Color::BLACK);

                IncrementCoefficients(coefficients,
                                      enemyKingBaseIndex + (48 * whiteBucket) + (pieceSquareIndex ^ 56) - 8,
                                      chess::Color::BLACK);
            }
            else
            {
                IncrementCoefficients(
                    coefficients,
                    (48 * PSQTBucketCount) + (64 * PSQTBucketCount * (tunerPieceIndex - 1)) + (64 * blackBucket) + (pieceSquareIndex ^ 56),
                    chess::Color::BLACK);

                IncrementCoefficients(
                    coefficients,
                    enemyKingBaseIndex + (48 * PSQTBucketCount) + (64 * PSQTBucketCount * (tunerPieceIndex - 1)) + (64 * whiteBucket) + (pieceSquareIndex ^ 56),
                    chess::Color::BLACK);
            }
        }
    }

    // Kings
    packedScore += PackedPositionalTables(0, whiteBucket, 5, whiteKing) +
                   PackedPositionalTables(0, blackBucket, 11, blackKing) +
                   PackedPositionalTables(1, blackBucket, 5, whiteKing) +
                   PackedPositionalTables(1, whiteBucket, 11, blackKing) +
                   KingAdditionalEvaluation(whiteKing, whiteBucket, blackPawnAttacks, chess::Color::WHITE, board, pieceCount, coefficients) -
                   KingAdditionalEvaluation(blackKing, blackBucket, whitePawnAttacks, chess::Color::BLACK, board, pieceCount, coefficients);

    IncrementCoefficients(
        coefficients,
        (48 * PSQTBucketCount) + (64 * PSQTBucketCount * 4) + (64 * whiteBucket) + whiteKing,
        chess::Color::WHITE);

    IncrementCoefficients(
        coefficients,
        (48 * PSQTBucketCount) + (64 * PSQTBucketCount * 4) + (64 * blackBucket) + (blackKing ^ 56),
        chess::Color::BLACK);

    IncrementCoefficients(
        coefficients,
        enemyKingBaseIndex + (48 * PSQTBucketCount) + (64 * PSQTBucketCount * 4) + (64 * blackBucket) + whiteKing,
        chess::Color::WHITE);

    IncrementCoefficients(
        coefficients,
        enemyKingBaseIndex + (48 * PSQTBucketCount) + (64 * PSQTBucketCount * 4) + (64 * whiteBucket) + (blackKing ^ 56),
        chess::Color::BLACK);

    const auto whiteBishops = GetPieceSwappingEndianness(board, chess::PieceType::BISHOP, chess::Color::WHITE);
    const auto blackBishops = GetPieceSwappingEndianness(board, chess::PieceType::BISHOP, chess::Color::BLACK);

    // Bishop pair bonus
    if (chess::builtin::popcount(whiteBishops) >= 2)
    {
        packedScore += BishopPairBonus.packed;
        IncrementCoefficients(coefficients, BishopPairBonus.index, chess::Color::WHITE);
    }

    if (chess::builtin::popcount(blackBishops) >= 2)
    {
        packedScore -= BishopPairBonus.packed;
        IncrementCoefficients(coefficients, BishopPairBonus.index, chess::Color::BLACK);
    }

    // Pieces attacked by pawns bonus
    const auto attackedPiecesByBlackPawns = chess::builtin::popcount(blackPawnAttacks & __builtin_bswap64(board.us(chess::Color::WHITE).getBits()) /*&(~GetPieceSwappingEndianness(board, chess::PieceType::PAWN, chess::Color::WHITE))*/);
    const auto attackedPiecesByWhitePawns = chess::builtin::popcount(whitePawnAttacks & __builtin_bswap64(board.us(chess::Color::BLACK).getBits()) /*&(~GetPieceSwappingEndianness(board, chess::PieceType::PAWN, chess::Color::BLACK))*/);

    IncrementCoefficients(coefficients, PieceAttackedByPawnPenalty.index, chess::Color::WHITE, attackedPiecesByBlackPawns);
    IncrementCoefficients(coefficients, PieceAttackedByPawnPenalty.index, chess::Color::BLACK, attackedPiecesByWhitePawns);

    packedScore += (PieceAttackedByPawnPenalty.packed * attackedPiecesByBlackPawns) -
                   (PieceAttackedByPawnPenalty.packed * attackedPiecesByWhitePawns);

    // Pawn islands
    const auto whitePawnIslands = PawnIslands(whitePawns);
    const auto blackPawnIslands = PawnIslands(blackPawns);

    packedScore += PawnIslandsBonus.packed[whitePawnIslands] - PawnIslandsBonus.packed[blackPawnIslands];
    IncrementCoefficients(coefficients, PawnIslandsBonus.index + whitePawnIslands - PawnIslandsBonus.start, chess::Color::WHITE);
    IncrementCoefficients(coefficients, PawnIslandsBonus.index + blackPawnIslands - PawnIslandsBonus.start, chess::Color::BLACK);

    // Debugging eval
    // return EvalResult{
    //     std::move(coefficients),
    //     (double)(board.sideToMove() == chess::Color::WHITE
    //                  ? eval
    //                  : -eval)};

    // Check if drawn position due to lack of material

    const int totalPawnsCount = board.pieces(chess::PieceType::PAWN, chess::Color::WHITE).count() +
                                board.pieces(chess::PieceType::PAWN, chess::Color::BLACK).count();

    // Pawnless endgames with few pieces
    if (gamePhase <= 3 && totalPawnsCount == 0)
    {
        switch (gamePhase)
        {
        // case 5:
        //     {
        //         // RB vs R, RN vs R - escale it down due to the chances of it being a draw
        //         if (pieceCount[(int)Piece.R] == 1 && pieceCount[(int)Piece.r] == 1)
        //         {
        //             packedScore >>= 1; // /2
        //         }

        //        break;
        //    }
        case 3:
        {
            const auto winningSideOffset = PieceOffset(packedScore >= 0);

            if (pieceCount[1 + winningSideOffset] == 2) // NN vs N, NN vs B
            {
                return EvalResult{
                    std::move(coefficients),
                    (double)0};
            }

            // Without rooks, only BB vs N is a win and BN vs N can have some chances
            // Not taking that into account here though, we would need this to rule them out: `pieceCount[(int)Piece.b - winningSideOffset] == 1 || pieceCount[(int)Piece.B + winningSideOffset] <= 1`
            // if (pieceCount[(int)Piece.R + winningSideOffset] == 0)  // BN vs B, NN vs B, BB vs B, BN vs N, NN vs N
            //{
            //    packedScore >>= 1; // /2
            //}

            break;
        }
        case 2:
        {
            if (pieceCount[1] + pieceCount[7] == 2     // NN vs -, N vs N
                || pieceCount[1] + pieceCount[2] == 1) // B vs N, B vs B
            {
                return EvalResult{
                    std::move(coefficients),
                    (double)0};
            }

            break;
        }
        case 1:
        case 0:
        {
            return EvalResult{
                std::move(coefficients),
                (double)0};
        }
        }
    }

    const int maxPhase = 24;

    if (gamePhase > maxPhase) // Early promotions
    {
        gamePhase = maxPhase;
    }

    const int endGamePhase = maxPhase - gamePhase;

    const auto middleGameScore = UnpackMG(packedScore);
    const auto endGameScore = UnpackEG(packedScore);
    int eval = ((middleGameScore * gamePhase) + (endGameScore * endGamePhase)) / maxPhase;

    eval = (int)(eval * ((80 + (totalPawnsCount * 7)) / 128.0));

    eval = ScaleEvalWith50MovesDrawDistance(eval, 0);

    eval = std::clamp(eval, MinEval, MaxEval);

    // Always white's perspective
    return EvalResult{
        std::move(coefficients),
        (double)eval};
}
