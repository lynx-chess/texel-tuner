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
const static size_t numParameters = psqtIndexCount +
                                    // DoubledPawnPenalty.size
                                    KingShieldBonus.size +
                                    KingShieldNonAttackedBonus.size +
                                    BishopPairBonus.size +
                                    BishopCorneredPenalty.size +
                                    BishopCorneredAndBlockedPenalty.size +
                                    BishopInUnblockedLongDiagonalBonus.size +
                                    PieceAttackedByPawnPenalty.size +
                                    PawnKingRingAttacksBonus.size +   // 3
                                    KnightKingRingAttacksBonus.size + // 3
                                    BishopKingRingAttacksBonus.size + // 3
                                    RookKingRingAttacksBonus.size +   // 5
                                    QueenKingRingAttacksBonus.size +  // 6

                                    // Arrays
                                    TotalKingRingAttacksBonus.tunableSize + // 5, removing king
                                    PieceProtectedByPawnBonus.tunableSize + // 5, removing king
                                    IsolatedPawnPenalty.tunableSize +       // 8, files
                                    PawnPhalanxBonus.tunableSize +          // 6
                                    ConnectedRooksBonus.tunableSize +
                                    PawnIslandsBonus.tunableSize +
                                    BadBishop_SameColorPawnsPenalty.tunableSize +
                                    BadBishop_BlockedCentralPawnsPenalty.tunableSize +
                                    SafeCheckBonus.tunableSize +
                                    UnsafeCheckBonus.tunableSize +
                                    FriendlyKingDistanceToPassedPawnBonus.tunableSize + // 7, removing start
                                    EnemyKingDistanceToPassedPawnPenalty.tunableSize +  // 7, removing start
                                    BackwardsPawnPenalty.tunableSize +                  // 7, removing start
                                    VirtualKingMobilityBonus.tunableSize +              // 28
                                    KnightMobilityBonus.tunableSize +                   // 9
                                    BishopMobilityBonus.tunableSize +                   // 14, removing end
                                    RookMobilityBonus.tunableSize +                     // 15
                                    QueenMobilityBonus.tunableSize +
                                    KingMobilityBonus.tunableSize +
                                    KnightThreatsBonus.tunableSize +
                                    KnightThreatsBonus_Defended.tunableSize +
                                    BishopThreatsBonus.tunableSize +
                                    BishopThreatsBonus_Defended.tunableSize +
                                    RookThreatsBonus.tunableSize +
                                    RookThreatsBonus_Defended.tunableSize +
                                    QueenThreatsBonus.tunableSize +
                                    QueenThreatsBonus_Defended.tunableSize +
                                    KingThreatsBonus.tunableSize +
                                    KingThreatsBonus_Defended.tunableSize +
                                    PawnPushThreatBonus.tunableSize +

                                    // Bucketed arrays
                                    PassedPawnBonus.size +                    // PSQTBucketCount * 6, removing 1 rank values
                                    PassedPawnEnemyBonus.size +               // PSQTBucketCount * 6, removing 1 rank values
                                    PassedPawnNoEnemiesAheadBonus.size +      // PSQTBucketCount * 6, removing 1 rank values
                                    PassedPawnNoEnemiesAheadEnemyBonus.size + // PSQTBucketCount * 6, removing 1 rank values
                                    OpenFileKingPenalty.size +
                                    SemiOpenFileKingPenalty.size +
                                    OpenFileRookBonus.size +
                                    SemiOpenFileRookBonus.size +
                                    OpenFileRookEnemyBonus.size +
                                    SemiOpenFileRookEnemyBonus.size;

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
        KingShieldBonus.add(result);
        KingShieldNonAttackedBonus.add(result);
        BishopPairBonus.add(result);
        BishopCorneredPenalty.add(result);
        BishopCorneredAndBlockedPenalty.add(result);
        BishopInUnblockedLongDiagonalBonus.add(result);
        PieceAttackedByPawnPenalty.add(result);
        PawnKingRingAttacksBonus.add(result);
        KnightKingRingAttacksBonus.add(result);
        BishopKingRingAttacksBonus.add(result);
        RookKingRingAttacksBonus.add(result);
        QueenKingRingAttacksBonus.add(result);

        // Arrays
        TotalKingRingAttacksBonus.add(result);
        PieceProtectedByPawnBonus.add(result);
        IsolatedPawnPenalty.add(result);
        PawnPhalanxBonus.add(result);
        OpenFileKingPenalty.add(result);
        SemiOpenFileKingPenalty.add(result);
        ConnectedRooksBonus.add(result);
        PawnIslandsBonus.add(result);
        BadBishop_SameColorPawnsPenalty.add(result);
        BadBishop_BlockedCentralPawnsPenalty.add(result);
        SafeCheckBonus.add(result);
        UnsafeCheckBonus.add(result);

        FriendlyKingDistanceToPassedPawnBonus.add(result);
        EnemyKingDistanceToPassedPawnPenalty.add(result);
        BackwardsPawnPenalty.add(result);
        VirtualKingMobilityBonus.add(result);
        KnightMobilityBonus.add(result);
        BishopMobilityBonus.add(result);
        RookMobilityBonus.add(result);
        QueenMobilityBonus.add(result);
        KingMobilityBonus.add(result);
        KnightThreatsBonus.add(result);
        KnightThreatsBonus_Defended.add(result);
        BishopThreatsBonus.add(result);
        BishopThreatsBonus_Defended.add(result);
        RookThreatsBonus.add(result);
        RookThreatsBonus_Defended.add(result);
        QueenThreatsBonus.add(result);
        QueenThreatsBonus_Defended.add(result);
        KingThreatsBonus.add(result);
        KingThreatsBonus_Defended.add(result);
        PawnPushThreatBonus.add(result);

        // Bucketed arrays
        PassedPawnBonus.add(result);
        PassedPawnEnemyBonus.add(result);
        PassedPawnNoEnemiesAheadBonus.add(result);
        PassedPawnNoEnemiesAheadEnemyBonus.add(result);
        OpenFileRookBonus.add(result);
        SemiOpenFileRookBonus.add(result);
        OpenFileRookEnemyBonus.add(result);
        SemiOpenFileRookEnemyBonus.add(result);

        assert(PassedPawnBonus.bucketTunableSize == 6);
        assert(PassedPawnEnemyBonus.bucketTunableSize == 6);
        assert(PassedPawnNoEnemiesAheadBonus.bucketTunableSize == 6);
        assert(PassedPawnNoEnemiesAheadEnemyBonus.bucketTunableSize == 6);
        assert(OpenFileKingPenalty.bucketTunableSize == 8);
        assert(SemiOpenFileKingPenalty.bucketTunableSize == 8);
        assert(OpenFileRookBonus.bucketTunableSize == 8);
        assert(SemiOpenFileRookBonus.bucketTunableSize == 8);
        assert(OpenFileRookEnemyBonus.bucketTunableSize == 8);
        assert(SemiOpenFileRookEnemyBonus.bucketTunableSize == 8);

        assert(TotalKingRingAttacksBonus.tunableSize == 14);
        assert(PieceProtectedByPawnBonus.tunableSize == 5);
        assert(ConnectedRooksBonus.tunableSize == 8);
        assert(IsolatedPawnPenalty.tunableSize == 8);
        assert(PawnPhalanxBonus.tunableSize == 6);
        assert(FriendlyKingDistanceToPassedPawnBonus.tunableSize == 7);
        assert(EnemyKingDistanceToPassedPawnPenalty.tunableSize == 7);
        assert(BackwardsPawnPenalty.tunableSize == 7);
        assert(VirtualKingMobilityBonus.tunableSize == 28);
        assert(KnightMobilityBonus.tunableSize == 9);
        assert(BishopMobilityBonus.tunableSize == 14);
        assert(RookMobilityBonus.tunableSize == 15);
        assert(QueenMobilityBonus.tunableSize == 28);
        assert(KingMobilityBonus.tunableSize == 9);
        assert(KnightThreatsBonus.tunableSize == 6);
        assert(KnightThreatsBonus_Defended.tunableSize == 6);
        assert(BishopThreatsBonus.tunableSize == 6);
        assert(BishopThreatsBonus_Defended.tunableSize == 6);
        assert(RookThreatsBonus.tunableSize == 6);
        assert(RookThreatsBonus_Defended.tunableSize == 6);
        assert(QueenThreatsBonus.tunableSize == 6);
        assert(QueenThreatsBonus_Defended.tunableSize == 6);
        assert(KingThreatsBonus.tunableSize == 6);
        assert(KingThreatsBonus_Defended.tunableSize == 6);

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

        name = NAME(KingShieldBonus);
        KingShieldBonus.to_csharp(parameters, ss, name);

        name = NAME(KingShieldNonAttackedBonus);
        KingShieldNonAttackedBonus.to_csharp(parameters, ss, name);

        name = NAME(BishopPairBonus);
        BishopPairBonus.to_csharp(parameters, ss, name);

        name = NAME(BishopCorneredPenalty);
        BishopCorneredPenalty.to_csharp(parameters, ss, name);

        name = NAME(BishopCorneredAndBlockedPenalty);
        BishopCorneredAndBlockedPenalty.to_csharp(parameters, ss, name);

        name = NAME(BishopInUnblockedLongDiagonalBonus);
        BishopInUnblockedLongDiagonalBonus.to_csharp(parameters, ss, name);

        name = NAME(PieceAttackedByPawnPenalty);
        PieceAttackedByPawnPenalty.to_csharp(parameters, ss, name);

        name = NAME(PawnKingRingAttacksBonus);
        PawnKingRingAttacksBonus.to_csharp(parameters, ss, name);

        name = NAME(KnightKingRingAttacksBonus);
        KnightKingRingAttacksBonus.to_csharp(parameters, ss, name);

        name = NAME(BishopKingRingAttacksBonus);
        BishopKingRingAttacksBonus.to_csharp(parameters, ss, name);

        name = NAME(RookKingRingAttacksBonus);
        RookKingRingAttacksBonus.to_csharp(parameters, ss, name);

        name = NAME(QueenKingRingAttacksBonus);
        QueenKingRingAttacksBonus.to_csharp(parameters, ss, name);

        // Arrays
        name = NAME(TotalKingRingAttacksBonus);
        TotalKingRingAttacksBonus.to_csharp(parameters, ss, name);

        name = NAME(PieceProtectedByPawnBonus);
        PieceProtectedByPawnBonus.to_csharp(parameters, ss, name);

        name = NAME(IsolatedPawnPenalty);
        IsolatedPawnPenalty.to_csharp(parameters, ss, name);

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

        name = NAME(SafeCheckBonus);
        SafeCheckBonus.to_csharp(parameters, ss, name);

        name = NAME(UnsafeCheckBonus);
        UnsafeCheckBonus.to_csharp(parameters, ss, name);

        name = NAME(FriendlyKingDistanceToPassedPawnBonus);
        FriendlyKingDistanceToPassedPawnBonus.to_csharp(parameters, ss, name);

        name = NAME(EnemyKingDistanceToPassedPawnPenalty);
        EnemyKingDistanceToPassedPawnPenalty.to_csharp(parameters, ss, name);

        name = NAME(BackwardsPawnPenalty);
        BackwardsPawnPenalty.to_csharp(parameters, ss, name);

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

        name = NAME(KingMobilityBonus);
        KingMobilityBonus.to_csharp(parameters, ss, name, mobilityPieceValues);

        name = NAME(KnightThreatsBonus);
        KnightThreatsBonus.to_csharp(parameters, ss, name);

        name = NAME(KnightThreatsBonus_Defended);
        KnightThreatsBonus_Defended.to_csharp(parameters, ss, name);

        name = NAME(BishopThreatsBonus);
        BishopThreatsBonus.to_csharp(parameters, ss, name);

        name = NAME(BishopThreatsBonus_Defended);
        BishopThreatsBonus_Defended.to_csharp(parameters, ss, name);

        name = NAME(RookThreatsBonus);
        RookThreatsBonus.to_csharp(parameters, ss, name);

        name = NAME(RookThreatsBonus_Defended);
        RookThreatsBonus_Defended.to_csharp(parameters, ss, name);

        name = NAME(QueenThreatsBonus);
        QueenThreatsBonus.to_csharp(parameters, ss, name);

        name = NAME(QueenThreatsBonus_Defended);
        QueenThreatsBonus_Defended.to_csharp(parameters, ss, name);

        name = NAME(KingThreatsBonus);
        KingThreatsBonus.to_csharp(parameters, ss, name);

        name = NAME(KingThreatsBonus_Defended);
        KingThreatsBonus_Defended.to_csharp(parameters, ss, name);

        name = NAME(PawnPushThreatBonus);
        PawnPushThreatBonus.to_csharp(parameters, ss, name);

        // Bucketed arrays
        name = NAME(PassedPawnBonus);
        PassedPawnBonus.to_csharp(parameters, ss, name);

        name = NAME(PassedPawnEnemyBonus);
        PassedPawnEnemyBonus.to_csharp(parameters, ss, name);

        name = NAME(PassedPawnNoEnemiesAheadBonus);
        PassedPawnNoEnemiesAheadBonus.to_csharp(parameters, ss, name);

        name = NAME(PassedPawnNoEnemiesAheadEnemyBonus);
        PassedPawnNoEnemiesAheadEnemyBonus.to_csharp(parameters, ss, name);

        name = NAME(OpenFileKingPenalty);
        OpenFileKingPenalty.to_csharp(parameters, ss, name);

        name = NAME(SemiOpenFileKingPenalty);
        SemiOpenFileKingPenalty.to_csharp(parameters, ss, name);

        name = NAME(OpenFileRookBonus);
        OpenFileRookBonus.to_csharp(parameters, ss, name);

        name = NAME(SemiOpenFileRookBonus);
        SemiOpenFileRookBonus.to_csharp(parameters, ss, name);

        name = NAME(OpenFileRookEnemyBonus);
        OpenFileRookEnemyBonus.to_csharp(parameters, ss, name);

        name = NAME(SemiOpenFileRookEnemyBonus);
        SemiOpenFileRookEnemyBonus.to_csharp(parameters, ss, name);

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
                 << "#pragma warning restore IDE0055, IDE1006, RCS1036 // Discard formatting and naming styles\n";
        }
        file.close();
    }

    static void print_cpp_parameters(const parameters_t &parameters, const std::array<std::array<tune_t, 12>, PSQTBucketCount> &mobilityPieceValues, bool isFinal = false)
    {
        std::stringstream ss;
        std::string name;

        // name = NAME(DoubledPawnPenalty);
        // DoubledPawnPenalty.to_json(parameters, ss, name);

        name = NAME(KingShieldBonus);
        KingShieldBonus.to_cpp(parameters, ss, name);

        name = NAME(KingShieldNonAttackedBonus);
        KingShieldNonAttackedBonus.to_cpp(parameters, ss, name);

        name = NAME(BishopPairBonus);
        BishopPairBonus.to_cpp(parameters, ss, name);

        name = NAME(BishopCorneredPenalty);
        BishopCorneredPenalty.to_cpp(parameters, ss, name);

        name = NAME(BishopCorneredAndBlockedPenalty);
        BishopCorneredAndBlockedPenalty.to_cpp(parameters, ss, name);

        name = NAME(BishopInUnblockedLongDiagonalBonus);
        BishopInUnblockedLongDiagonalBonus.to_cpp(parameters, ss, name);

        name = NAME(PieceAttackedByPawnPenalty);
        PieceAttackedByPawnPenalty.to_cpp(parameters, ss, name);

        name = NAME(PawnKingRingAttacksBonus);
        PawnKingRingAttacksBonus.to_cpp(parameters, ss, name);

        name = NAME(KnightKingRingAttacksBonus);
        KnightKingRingAttacksBonus.to_cpp(parameters, ss, name);

        name = NAME(BishopKingRingAttacksBonus);
        BishopKingRingAttacksBonus.to_cpp(parameters, ss, name);

        name = NAME(RookKingRingAttacksBonus);
        RookKingRingAttacksBonus.to_cpp(parameters, ss, name);

        name = NAME(QueenKingRingAttacksBonus);
        QueenKingRingAttacksBonus.to_cpp(parameters, ss, name);

        // Arrays
        name = NAME(TotalKingRingAttacksBonus);
        TotalKingRingAttacksBonus.to_cpp(parameters, ss, name);
        ss << "\n";

        name = NAME(PieceProtectedByPawnBonus);
        PieceProtectedByPawnBonus.to_cpp(parameters, ss, name);
        ss << "\n";

        name = NAME(IsolatedPawnPenalty);
        IsolatedPawnPenalty.to_cpp(parameters, ss, name);
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

        name = NAME(SafeCheckBonus);
        SafeCheckBonus.to_cpp(parameters, ss, name);
        ss << "\n";

        name = NAME(UnsafeCheckBonus);
        UnsafeCheckBonus.to_cpp(parameters, ss, name);
        ss << "\n";

        name = NAME(FriendlyKingDistanceToPassedPawnBonus);
        FriendlyKingDistanceToPassedPawnBonus.to_cpp(parameters, ss, name);

        name = NAME(EnemyKingDistanceToPassedPawnPenalty);
        EnemyKingDistanceToPassedPawnPenalty.to_cpp(parameters, ss, name);

        name = NAME(BackwardsPawnPenalty);
        BackwardsPawnPenalty.to_cpp(parameters, ss, name);

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

        name = NAME(KingMobilityBonus);
        KingMobilityBonus.to_cpp(parameters, ss, name, mobilityPieceValues);

        name = NAME(KnightThreatsBonus);
        KnightThreatsBonus.to_cpp(parameters, ss, name);

        name = NAME(KnightThreatsBonus_Defended);
        KnightThreatsBonus_Defended.to_cpp(parameters, ss, name);

        name = NAME(BishopThreatsBonus);
        BishopThreatsBonus.to_cpp(parameters, ss, name);

        name = NAME(BishopThreatsBonus_Defended);
        BishopThreatsBonus_Defended.to_cpp(parameters, ss, name);

        name = NAME(RookThreatsBonus);
        RookThreatsBonus.to_cpp(parameters, ss, name);

        name = NAME(RookThreatsBonus_Defended);
        RookThreatsBonus_Defended.to_cpp(parameters, ss, name);

        name = NAME(QueenThreatsBonus);
        QueenThreatsBonus.to_cpp(parameters, ss, name);

        name = NAME(QueenThreatsBonus_Defended);
        QueenThreatsBonus_Defended.to_cpp(parameters, ss, name);

        name = NAME(KingThreatsBonus);
        KingThreatsBonus.to_cpp(parameters, ss, name);

        name = NAME(KingThreatsBonus_Defended);
        KingThreatsBonus_Defended.to_cpp(parameters, ss, name);

        name = NAME(PawnPushThreatBonus);
        PawnPushThreatBonus.to_cpp(parameters, ss, name);

        // Bucketed arrays
        name = NAME(PassedPawnBonus);
        PassedPawnBonus.to_cpp(parameters, ss, name);

        name = NAME(PassedPawnEnemyBonus);
        PassedPawnEnemyBonus.to_cpp(parameters, ss, name);

        name = NAME(PassedPawnNoEnemiesAheadBonus);
        PassedPawnNoEnemiesAheadBonus.to_cpp(parameters, ss, name);

        name = NAME(PassedPawnNoEnemiesAheadEnemyBonus);
        PassedPawnNoEnemiesAheadEnemyBonus.to_cpp(parameters, ss, name);

        name = NAME(OpenFileKingPenalty);
        OpenFileKingPenalty.to_cpp(parameters, ss, name);

        name = NAME(SemiOpenFileKingPenalty);
        SemiOpenFileKingPenalty.to_cpp(parameters, ss, name);

        name = NAME(OpenFileRookBonus);
        OpenFileRookBonus.to_cpp(parameters, ss, name);

        name = NAME(SemiOpenFileRookBonus);
        SemiOpenFileRookBonus.to_cpp(parameters, ss, name);

        name = NAME(OpenFileRookEnemyBonus);
        OpenFileRookEnemyBonus.to_cpp(parameters, ss, name);

        name = NAME(SemiOpenFileRookEnemyBonus);
        SemiOpenFileRookEnemyBonus.to_cpp(parameters, ss, name);

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

int PawnAdditionalEvaluation(int squareIndex, int bucket, int oppositeSideBucket, int sameSideKingSquare, int oppositeSideKingSquare, const std::array<u64, 12> &attacks, const chess::Board &board, const chess::Color &color, coefficients_t &coefficients)
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

    auto pieceIndex = 0;
    auto sameSidePawns = whitePawns;
    auto oppositeSidePawns = blackPawns;
    auto oppositeSidePieces = blackPieces;
    auto passedPawnMask = WhitePassedPawnMasks[squareIndex];
    auto rank = Rank[squareIndex];
    auto pushSquare = squareIndex - 8;

    if (color == chess::Color::BLACK)
    {
        pieceIndex = 6;
        sameSidePawns = blackPawns;
        oppositeSidePawns = whitePawns;
        oppositeSidePieces = whitePieces;
        passedPawnMask = BlackPassedPawnMasks[squareIndex];
        rank = 7 - rank;
        pushSquare = squareIndex + 8;
    }

    // Isolated pawn
    if ((sameSidePawns & IsolatedPawnMasks[squareIndex]) == 0) // isIsolatedPawn
    {
        const auto file = File[squareIndex];
        packedBonus += IsolatedPawnPenalty.packed[file];
        IncrementCoefficients(coefficients, IsolatedPawnPenalty.index - IsolatedPawnPenalty.start + file, color);
    }
    // Backwards pawn
    else if (!GetBit(attacks[pieceIndex], squareIndex) &&
             (GetBit(oppositeSidePawns, pushSquare) ||      // Blocked
              GetBit(attacks[6 - pieceIndex], pushSquare))) // Push square attacked by opponent pawns
    {
        packedBonus += BackwardsPawnPenalty.packed[rank];
        IncrementCoefficients(coefficients, BackwardsPawnPenalty.index - BackwardsPawnPenalty.start + rank, color);
    }

    // Passed pawn
    if ((oppositeSidePawns & passedPawnMask) == 0)
    {
        packedBonus += PassedPawnBonus.packed(bucket, rank);
        IncrementCoefficients(coefficients, PassedPawnBonus.index(bucket, rank - PassedPawnBonus.start), color); // There's no coefficient for rank 0

        packedBonus += PassedPawnEnemyBonus.packed(oppositeSideBucket, rank);
        IncrementCoefficients(coefficients, PassedPawnEnemyBonus.index(oppositeSideBucket, rank - PassedPawnEnemyBonus.start), color); // There's no coefficient for rank 0

        // Passed pawn without opponent pieces ahead (in its passed pawn mask)
        if ((oppositeSidePieces & passedPawnMask) == 0)
        {
            packedBonus += PassedPawnNoEnemiesAheadBonus.packed(bucket, rank);
            IncrementCoefficients(coefficients, PassedPawnNoEnemiesAheadBonus.index(bucket, rank - PassedPawnNoEnemiesAheadBonus.start), color); // There's no coefficient for rank 0

            packedBonus += PassedPawnNoEnemiesAheadEnemyBonus.packed(oppositeSideBucket, rank);
            IncrementCoefficients(coefficients, PassedPawnNoEnemiesAheadEnemyBonus.index(oppositeSideBucket, rank - PassedPawnNoEnemiesAheadEnemyBonus.start), color); // There's no coefficient for rank 0
        }

        // King distance to passed pawn
        const auto friendlyKingDistance = ChebyshevDistance(sameSideKingSquare, squareIndex);
        packedBonus += FriendlyKingDistanceToPassedPawnBonus.packed[friendlyKingDistance];
        IncrementCoefficients(coefficients, FriendlyKingDistanceToPassedPawnBonus.index + friendlyKingDistance - FriendlyKingDistanceToPassedPawnBonus.start, color);

        // Enemy king distance to passed pawn
        const auto enemyKingDistance = ChebyshevDistance(oppositeSideKingSquare, squareIndex);
        packedBonus += EnemyKingDistanceToPassedPawnPenalty.packed[enemyKingDistance];
        IncrementCoefficients(coefficients, EnemyKingDistanceToPassedPawnPenalty.index + enemyKingDistance - EnemyKingDistanceToPassedPawnPenalty.start, color);
    }

    // Pawn phalanx
    if (File[squareIndex] != 7 && GetBit(sameSidePawns, squareIndex + 1))
    {
        packedBonus += PawnPhalanxBonus.packed[rank];
        IncrementCoefficients(coefficients, PawnPhalanxBonus.index + rank - PawnPhalanxBonus.start, color);
    }

    return packedBonus;
}

int RookAdditionalEvaluation(int squareIndex, int bucket, int oppositeSideBucket, const u64 opponentPawnAttacks, int oppositeSideKingSquare, const chess::Board &board, const chess::Color &color, coefficients_t &coefficients, std::array<int, 2> &totalKingRingAttacks)
{
    const auto occupancy = __builtin_bswap64(board.occ().getBits());
    const auto attacks = chess::attacks::rook(static_cast<chess::Square>(squareIndex), occupancy).getBits();
    const auto sameSidePawns = GetPieceSwappingEndianness(board, chess::PieceType::PAWN, color);

    // Mobility
    const auto squaresToExcludeFromMobility = (~sameSidePawns) & (~opponentPawnAttacks);
    const auto mobilityCount = chess::builtin::popcount(attacks & squaresToExcludeFromMobility);
    int packedBonus = RookMobilityBonus.packed[mobilityCount];
    IncrementCoefficients(coefficients, RookMobilityBonus.index + mobilityCount, color);

    // King ring attacks
    const auto kingRing = KingRing[oppositeSideKingSquare];
    const auto kingRingAttacksCount = chess::builtin::popcount(attacks & kingRing);
    packedBonus += RookKingRingAttacksBonus.packed * kingRingAttacksCount;
    IncrementCoefficients(coefficients, RookKingRingAttacksBonus.index, color, kingRingAttacksCount);

    totalKingRingAttacks[color] += kingRingAttacksCount;

    // Open file
    if (((GetPieceSwappingEndianness(board, chess::PieceType::PAWN, chess::Color::WHITE) | GetPieceSwappingEndianness(board, chess::PieceType::PAWN, chess::Color::BLACK)) & FileMasks[squareIndex]) == 0)
    {
        const auto file = File[squareIndex];

        packedBonus += OpenFileRookBonus.packed(bucket, file);
        IncrementCoefficients(coefficients, OpenFileRookBonus.index(bucket, file), color);

        packedBonus += OpenFileRookEnemyBonus.packed(oppositeSideBucket, file);
        IncrementCoefficients(coefficients, OpenFileRookEnemyBonus.index(oppositeSideBucket, file), color);
    }
    else
    {
        // Semi-open file
        if ((GetPieceSwappingEndianness(board, chess::PieceType::PAWN, color) & FileMasks[squareIndex]) == 0)
        {
            const auto file = File[squareIndex];

            packedBonus += SemiOpenFileRookBonus.packed(bucket, file);
            IncrementCoefficients(coefficients, SemiOpenFileRookBonus.index(bucket, file), color);

            packedBonus += SemiOpenFileRookEnemyBonus.packed(oppositeSideBucket, file);
            IncrementCoefficients(coefficients, SemiOpenFileRookEnemyBonus.index(oppositeSideBucket, file), color);
        }
    }

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

int KnightAdditionalEvaluation(int squareIndex, const u64 opponentPawnAttacks, int oppositeSideKingSquare, const chess::Board &board, const chess::Color &color, coefficients_t &coefficients, std::array<int, 2> &totalKingRingAttacks)
{
    const auto attacks = chess::attacks::knight(static_cast<chess::Square>(squareIndex)).getBits();
    const auto sameSidePawns = GetPieceSwappingEndianness(board, chess::PieceType::PAWN, color);

    // Mobility
    const auto squaresToExcludeFromMobility = (~sameSidePawns) & (~opponentPawnAttacks);
    const auto mobilityCount = chess::builtin::popcount(attacks & squaresToExcludeFromMobility);
    auto packedBonus = KnightMobilityBonus.packed[mobilityCount];
    IncrementCoefficients(coefficients, KnightMobilityBonus.index + mobilityCount, color);

    // King ring attacks
    const auto kingRing = KingRing[oppositeSideKingSquare];
    const auto kingRingAttacksCount = chess::builtin::popcount(attacks & kingRing);
    packedBonus += KnightKingRingAttacksBonus.packed * kingRingAttacksCount;
    IncrementCoefficients(coefficients, KnightKingRingAttacksBonus.index, color, kingRingAttacksCount);

    totalKingRingAttacks[color] += kingRingAttacksCount;

    return packedBonus;
}

int BishopAdditionalEvaluation(int squareIndex, int pieceIndex, const u64 opponentPawnAttacks, int oppositeSideKingSquare, const chess::Board &board, const chess::Color &color, coefficients_t &coefficients, std::array<int, 2> &totalKingRingAttacks)
{
    const auto occupancy = __builtin_bswap64(board.occ().getBits());
    const auto attacks = chess::attacks::bishop(static_cast<chess::Square>(squareIndex), occupancy).getBits();
    const auto sameSidePawns = GetPieceSwappingEndianness(board, chess::PieceType::PAWN, color);

    // Mobility
    const auto squaresToExcludeFromMobility = (~sameSidePawns) & (~opponentPawnAttacks);
    const auto mobilityCount = chess::builtin::popcount(attacks & squaresToExcludeFromMobility);
    auto packedBonus = BishopMobilityBonus.packed[mobilityCount];
    IncrementCoefficients(coefficients, BishopMobilityBonus.index + mobilityCount, color);

    // King ring attacks
    const auto kingRing = KingRing[oppositeSideKingSquare];
    const auto kingRingAttacksCount = chess::builtin::popcount(attacks & kingRing);
    packedBonus += BishopKingRingAttacksBonus.packed * kingRingAttacksCount;
    IncrementCoefficients(coefficients, BishopKingRingAttacksBonus.index, color, kingRingAttacksCount);

    totalKingRingAttacks[color] += kingRingAttacksCount;

    // Bad bishop - same color pawns
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

    if (!GetBit(Corners, squareIndex))
    {
        return packedBonus;
    }

    // Cornered/trapped bishop
    if (color == chess::Color::WHITE)
    {
        const int a1 = 56, h1 = 63, b2 = 49, g2 = 54;

        if (squareIndex == a1 && board.at(b2 ^ 56) == chess::Piece::WHITEPAWN)
        {
            const auto squareInFrontOfPawn = b2 - 8;

            if (board.at(squareInFrontOfPawn ^ 56) == chess::Piece::NONE)
            {
                packedBonus += BishopCorneredPenalty.packed;
                IncrementCoefficients(coefficients, BishopCorneredPenalty.index, color);
            }
            else
            {
                packedBonus += BishopCorneredAndBlockedPenalty.packed;
                IncrementCoefficients(coefficients, BishopCorneredAndBlockedPenalty.index, color);
            }
        }
        else if (squareIndex == h1 && board.at(g2 ^ 56) == chess::Piece::WHITEPAWN)
        {
            const auto squareInFrontOfPawn = g2 - 8;

            if (board.at(squareInFrontOfPawn ^ 56) == chess::Piece::NONE)
            {
                packedBonus += BishopCorneredPenalty.packed;
                IncrementCoefficients(coefficients, BishopCorneredPenalty.index, color);
            }
            else
            {
                packedBonus += BishopCorneredAndBlockedPenalty.packed;
                IncrementCoefficients(coefficients, BishopCorneredAndBlockedPenalty.index, color);
            }
        }
    }
    else
    {
        const int a8 = 0, h8 = 7, b7 = 9, g7 = 14;

        if (squareIndex == a8 && board.at(b7 ^ 56) == chess::Piece::BLACKPAWN)
        {
            const auto squareInFrontOfPawn = b7 + 8;

            if (board.at(squareInFrontOfPawn ^ 56) == chess::Piece::NONE)
            {
                packedBonus += BishopCorneredPenalty.packed;
                IncrementCoefficients(coefficients, BishopCorneredPenalty.index, color);
            }
            else
            {
                packedBonus += BishopCorneredAndBlockedPenalty.packed;
                IncrementCoefficients(coefficients, BishopCorneredAndBlockedPenalty.index, color);
            }
        }
        else if (squareIndex == h8 && board.at(g7 ^ 56) == chess::Piece::BLACKPAWN)
        {
            const auto squareInFrontOfPawn = g7 + 8;
            if (board.at(squareInFrontOfPawn ^ 56) == chess::Piece::NONE)
            {
                packedBonus += BishopCorneredPenalty.packed;
                IncrementCoefficients(coefficients, BishopCorneredPenalty.index, color);
            }
            else
            {
                packedBonus += BishopCorneredAndBlockedPenalty.packed;
                IncrementCoefficients(coefficients, BishopCorneredAndBlockedPenalty.index, color);
            }
        }
    }

    return packedBonus;
}

int QueenAdditionalEvaluation(int squareIndex, const u64 opponentPawnAttacks, int oppositeSideKingSquare, const chess::Board &board, const chess::Color &color, coefficients_t &coefficients, std::array<int, 2> &totalKingRingAttacks)
{
    const auto occupancy = __builtin_bswap64(board.occ().getBits());
    const auto attacks = chess::attacks::queen(static_cast<chess::Square>(squareIndex), occupancy).getBits();
    const auto sameSidePawns = GetPieceSwappingEndianness(board, chess::PieceType::PAWN, color);

    // Mobility
    const auto squaresToExcludeFromMobility = (~sameSidePawns) & (~opponentPawnAttacks);
    const auto mobilityCount = chess::builtin::popcount(attacks & squaresToExcludeFromMobility);
    auto packedBonus = QueenMobilityBonus.packed[mobilityCount];
    IncrementCoefficients(coefficients, QueenMobilityBonus.index + mobilityCount, color);

    // King ring attacks
    const auto kingRing = KingRing[oppositeSideKingSquare];
    const auto kingRingAttacksCount = chess::builtin::popcount(attacks & kingRing);
    packedBonus += QueenKingRingAttacksBonus.packed * kingRingAttacksCount;
    IncrementCoefficients(coefficients, QueenKingRingAttacksBonus.index, color, kingRingAttacksCount);

    totalKingRingAttacks[color] += kingRingAttacksCount;

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
            const auto file = File[squareIndex];

            packedBonus += OpenFileKingPenalty.packed(bucket, file);
            IncrementCoefficients(coefficients, OpenFileKingPenalty.index(bucket, file), kingSide);
        }
        // King on semi-open file
        else if ((GetPieceSwappingEndianness(board, chess::PieceType::PAWN, kingSide) & FileMasks[squareIndex]) == 0) // isSemiOpenFile
        {
            const auto file = File[squareIndex];

            packedBonus += SemiOpenFileKingPenalty.packed(bucket, file);
            IncrementCoefficients(coefficients, SemiOpenFileKingPenalty.index(bucket, file), kingSide);
        }
    }

    // King shield
    const auto kingShield = chess::attacks::king(static_cast<chess::Square>(squareIndex)).getBits() &
                            GetPieceSwappingEndianness(board, chess::PieceType::PAWN, kingSide);
    const auto kingShieldCount = chess::builtin::popcount(kingShield);

    const auto nonAttackedKingShield = kingShield & (~opponentPawnAttacks);
    const auto nonAttackedKingShieldCount = chess::builtin::popcount(nonAttackedKingShield);

    packedBonus += KingShieldBonus.packed * (kingShieldCount - nonAttackedKingShieldCount);
    IncrementCoefficients(coefficients, KingShieldBonus.index, kingSide, (kingShieldCount - nonAttackedKingShieldCount));

    packedBonus += KingShieldNonAttackedBonus.packed * nonAttackedKingShieldCount;
    IncrementCoefficients(coefficients, KingShieldNonAttackedBonus.index, kingSide, nonAttackedKingShieldCount);

    return packedBonus;
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

std::array<u64, 12> CalculateAttacks(const chess::Board &board)
{
    const auto occupancy = __builtin_bswap64(board.occ().getBits());

    std::array<u64, 12> pieceAttacks = {};

    for (auto color : {chess::Color::WHITE, chess::Color::BLACK})
    {
        const auto offset = color == chess::Color::WHITE ? 0 : 6;

        auto pawns = GetPieceSwappingEndianness(board, chess::PieceType::PAWN, color);
        while (pawns != 0)
        {
            const auto pieceSquareIndex = chess::builtin::lsb(pawns).index();
            ResetLS1B(pawns);

            // Using oppositeColor here instead of color because of little/big endian
            const auto attacks = chess::attacks::pawn(~color, static_cast<chess::Square>(pieceSquareIndex)).getBits();
            pieceAttacks[static_cast<int>(chess::PieceType::PAWN) + offset] |= attacks;
        }

        auto knights = GetPieceSwappingEndianness(board, chess::PieceType::KNIGHT, color);
        while (knights != 0)
        {
            const auto pieceSquareIndex = chess::builtin::lsb(knights).index();
            ResetLS1B(knights);

            const auto attacks = chess::attacks::knight(static_cast<chess::Square>(pieceSquareIndex)).getBits();
            pieceAttacks[static_cast<int>(chess::PieceType::KNIGHT) + offset] |= attacks;
        }

        auto bishops = GetPieceSwappingEndianness(board, chess::PieceType::BISHOP, color);
        while (bishops != 0)
        {
            const auto pieceSquareIndex = chess::builtin::lsb(bishops).index();
            ResetLS1B(bishops);

            const auto attacks = chess::attacks::bishop(static_cast<chess::Square>(pieceSquareIndex), occupancy).getBits();
            pieceAttacks[static_cast<int>(chess::PieceType::BISHOP) + offset] |= attacks;
        }

        auto rooks = GetPieceSwappingEndianness(board, chess::PieceType::ROOK, color);
        while (rooks != 0)
        {
            const auto pieceSquareIndex = chess::builtin::lsb(rooks).index();
            ResetLS1B(rooks);

            const auto attacks = chess::attacks::rook(static_cast<chess::Square>(pieceSquareIndex), occupancy).getBits();
            pieceAttacks[static_cast<int>(chess::PieceType::ROOK) + offset] |= attacks;
        }

        auto queens = GetPieceSwappingEndianness(board, chess::PieceType::QUEEN, color);
        while (queens != 0)
        {
            const auto pieceSquareIndex = chess::builtin::lsb(queens).index();
            ResetLS1B(queens);

            const auto attacks = chess::attacks::queen(static_cast<chess::Square>(pieceSquareIndex), occupancy).getBits();
            pieceAttacks[static_cast<int>(chess::PieceType::QUEEN) + offset] |= attacks;
        }

        auto kings = GetPieceSwappingEndianness(board, chess::PieceType::KING, color);
        while (kings != 0)
        {
            const auto pieceSquareIndex = chess::builtin::lsb(kings).index();
            ResetLS1B(kings);

            const auto attacks = chess::attacks::king(static_cast<chess::Square>(pieceSquareIndex)).getBits();
            pieceAttacks[static_cast<int>(chess::PieceType::KING) + offset] |= attacks;
        }
    }

    return pieceAttacks;
}

std::array<u64, 2> CalculateSideAttacks(const std::array<u64, 12> &attacks)
{
    std::array<u64, 2> sideAttacks = {0, 0};
    for (auto color : {chess::Color::WHITE, chess::Color::BLACK})
    {
        const auto offset = color == chess::Color::WHITE ? 0 : 6;

        sideAttacks[static_cast<int>(color)] |=
            attacks[0 + offset] |
            attacks[1 + offset] |
            attacks[2 + offset] |
            attacks[3 + offset] |
            attacks[4 + offset] |
            attacks[5 + offset];
    }

    return sideAttacks;
}

int Threats(const chess::Board &board, const chess::Color &color, coefficients_t &coefficients, const std::array<u64, 12> &attacks)
{
    int packedBonus = 0;

    const auto them = __builtin_bswap64(board.them(color).getBits());

    // Extract attacks
    const auto offset = color == chess::Color::WHITE ? 0 : 6;
    const auto oppositeSideoffset = 6 - offset;

    auto knightThreats = attacks[static_cast<int>(chess::PieceType::KNIGHT) + offset] & them;
    auto bishopThreats = attacks[static_cast<int>(chess::PieceType::BISHOP) + offset] & them;
    auto rookThreats = attacks[static_cast<int>(chess::PieceType::ROOK) + offset] & them;
    auto queenThreats = attacks[static_cast<int>(chess::PieceType::QUEEN) + offset] & them;
    auto kingThreats = attacks[static_cast<int>(chess::PieceType::KING) + offset] & them;

    const auto defendedSquares = attacks[static_cast<int>(chess::PieceType::PAWN) + oppositeSideoffset];

    // Calculate bonus
    auto defendedKnightThreats = knightThreats & defendedSquares;
    while (defendedKnightThreats != 0)
    {
        const auto pieceSquareIndex = chess::builtin::lsb(defendedKnightThreats).index();
        ResetLS1B(defendedKnightThreats);

        const auto attackedPiece = static_cast<int>(board.at(pieceSquareIndex ^ 56).type());

        packedBonus += KnightThreatsBonus_Defended.packed[attackedPiece];
        IncrementCoefficients(coefficients, KnightThreatsBonus_Defended.index - KnightThreatsBonus_Defended.start + attackedPiece, color);
    }

    auto undefendedKnightThreats = knightThreats & (~defendedSquares);
    while (undefendedKnightThreats != 0)
    {
        const auto pieceSquareIndex = chess::builtin::lsb(undefendedKnightThreats).index();
        ResetLS1B(undefendedKnightThreats);

        const auto attackedPiece = static_cast<int>(board.at(pieceSquareIndex ^ 56).type());

        packedBonus += KnightThreatsBonus.packed[attackedPiece];
        IncrementCoefficients(coefficients, KnightThreatsBonus.index - KnightThreatsBonus.start + attackedPiece, color);
    }

    auto defendedBishopThreats = bishopThreats & defendedSquares;
    while (defendedBishopThreats != 0)
    {
        const auto pieceSquareIndex = chess::builtin::lsb(defendedBishopThreats).index();
        ResetLS1B(defendedBishopThreats);

        const auto attackedPiece = static_cast<int>(board.at(pieceSquareIndex ^ 56).type());

        packedBonus += BishopThreatsBonus_Defended.packed[attackedPiece];
        IncrementCoefficients(coefficients, BishopThreatsBonus_Defended.index - BishopThreatsBonus_Defended.start + attackedPiece, color);
    }

    auto undefendedBishopThreats = bishopThreats & (~defendedSquares);
    while (undefendedBishopThreats != 0)
    {
        const auto pieceSquareIndex = chess::builtin::lsb(undefendedBishopThreats).index();
        ResetLS1B(undefendedBishopThreats);

        const auto attackedPiece = static_cast<int>(board.at(pieceSquareIndex ^ 56).type());

        packedBonus += BishopThreatsBonus.packed[attackedPiece];
        IncrementCoefficients(coefficients, BishopThreatsBonus.index - BishopThreatsBonus.start + attackedPiece, color);
    }

    auto defendedRookThreats = rookThreats & defendedSquares;
    while (defendedRookThreats != 0)
    {
        const auto pieceSquareIndex = chess::builtin::lsb(defendedRookThreats).index();
        ResetLS1B(defendedRookThreats);

        const auto attackedPiece = static_cast<int>(board.at(pieceSquareIndex ^ 56).type());

        packedBonus += RookThreatsBonus_Defended.packed[attackedPiece];
        IncrementCoefficients(coefficients, RookThreatsBonus_Defended.index - RookThreatsBonus_Defended.start + attackedPiece, color);
    }

    auto undefendedRookThreats = rookThreats & (~defendedSquares);
    while (undefendedRookThreats != 0)
    {
        const auto pieceSquareIndex = chess::builtin::lsb(undefendedRookThreats).index();
        ResetLS1B(undefendedRookThreats);

        const auto attackedPiece = static_cast<int>(board.at(pieceSquareIndex ^ 56).type());

        packedBonus += RookThreatsBonus.packed[attackedPiece];
        IncrementCoefficients(coefficients, RookThreatsBonus.index - RookThreatsBonus.start + attackedPiece, color);
    }

    auto defendedQueenThreats = queenThreats & defendedSquares;
    while (defendedQueenThreats != 0)
    {
        const auto pieceSquareIndex = chess::builtin::lsb(defendedQueenThreats).index();
        ResetLS1B(defendedQueenThreats);

        const auto attackedPiece = static_cast<int>(board.at(pieceSquareIndex ^ 56).type());

        packedBonus += QueenThreatsBonus_Defended.packed[attackedPiece];
        IncrementCoefficients(coefficients, QueenThreatsBonus_Defended.index - QueenThreatsBonus_Defended.start + attackedPiece, color);
    }

    auto undefendedQueenThreats = queenThreats & (~defendedSquares);
    while (undefendedQueenThreats != 0)
    {
        const auto pieceSquareIndex = chess::builtin::lsb(undefendedQueenThreats).index();
        ResetLS1B(undefendedQueenThreats);

        const auto attackedPiece = static_cast<int>(board.at(pieceSquareIndex ^ 56).type());

        packedBonus += QueenThreatsBonus.packed[attackedPiece];
        IncrementCoefficients(coefficients, QueenThreatsBonus.index - QueenThreatsBonus.start + attackedPiece, color);
    }

    auto defendedKingThreats = kingThreats & defendedSquares;
    while (defendedKingThreats != 0)
    {
        const auto pieceSquareIndex = chess::builtin::lsb(defendedKingThreats).index();
        ResetLS1B(defendedKingThreats);

        const auto attackedPiece = static_cast<int>(board.at(pieceSquareIndex ^ 56).type());

        packedBonus += KingThreatsBonus_Defended.packed[attackedPiece];
        IncrementCoefficients(coefficients, KingThreatsBonus_Defended.index - KingThreatsBonus_Defended.start + attackedPiece, color);
    }

    auto undefendedKingThreats = kingThreats & (~defendedSquares);
    while (undefendedKingThreats != 0)
    {
        const auto pieceSquareIndex = chess::builtin::lsb(undefendedKingThreats).index();
        ResetLS1B(undefendedKingThreats);

        const auto attackedPiece = static_cast<int>(board.at(pieceSquareIndex ^ 56).type());

        packedBonus += KingThreatsBonus.packed[attackedPiece];
        IncrementCoefficients(coefficients, KingThreatsBonus.index - KingThreatsBonus.start + attackedPiece, color);
    }

    // Pawn push threats
    auto ourPawns = GetPieceSwappingEndianness(board, chess::PieceType::PAWN, color);
    auto theirPawns = GetPieceSwappingEndianness(board, chess::PieceType::PAWN, ~color);

    auto nonPawnEnemies = __builtin_bswap64(board.them(color).getBits()) & ~theirPawns;

    auto safe = ~defendedSquares;
    // TODO: if we take into account all the piece attacks for defendedSquares
    //| (evaluationContext.AttacksBySide[(int)Side] & ~evaluationContext.Attacks[oppositeSidePawnIndex]);

    auto pushes = ~__builtin_bswap64(board.occ().getBits()) & PawnPush(ourPawns, color);

    // Double pushes
    u64 thirdRank = color == chess::Color::WHITE ? 280375465082880 : 16711680;
    auto doublePushes = ~__builtin_bswap64(board.occ().getBits()) & PawnPush(pushes & thirdRank, color);
    pushes |= doublePushes;

    auto pushThreats = PawnAttacks(pushes & safe, color) & nonPawnEnemies;
    while (pushThreats != 0)
    {
        const auto pushThreat = chess::builtin::lsb(pushThreats).index();
        ResetLS1B(pushThreats);

        const auto piece = static_cast<int>(board.at(pushThreat ^ 56).type());

        packedBonus += PawnPushThreatBonus.packed[piece];
        IncrementCoefficients(coefficients, PawnPushThreatBonus.index - PawnPhalanxBonus.start + piece, color);
    }

    return packedBonus;
}

int Checks(const chess::Board &board, const chess::Color &color, coefficients_t &coefficients, const std::array<u64, 12> &attacks, const std::array<u64, 2> &attacksBySide)
{
    int packedBonus = 0;

    const auto offset = color == chess::Color::WHITE ? 0 : 6;
    const auto oppositeColor = ~color;
    const auto occupancy = __builtin_bswap64(board.occ().getBits());

    const auto oppositeSideKingSquare = chess::builtin::lsb(GetPieceSwappingEndianness(board, chess::PieceType::KING, ~color)).index();
    const auto oppositeSideAttacks = attacksBySide[static_cast<int>(oppositeColor)];

    const std::array<u64, 5> checkThreats = {
        0,
        chess::attacks::knight(oppositeSideKingSquare).getBits(),
        chess::attacks::bishop(oppositeSideKingSquare, occupancy).getBits(),
        chess::attacks::rook(oppositeSideKingSquare, occupancy).getBits(),
        chess::attacks::queen(oppositeSideKingSquare, occupancy).getBits(),
    };

    for (int pieceIndex = static_cast<int>(chess::PieceType::KNIGHT); pieceIndex < static_cast<int>(chess::PieceType::KING); ++pieceIndex)
    {
        const auto checks = attacks[pieceIndex + offset] & checkThreats[pieceIndex];
        const auto checkThreatsCount = chess::builtin::popcount(checks);

        const auto unsafeChecksCount = chess::builtin::popcount(checks & oppositeSideAttacks);
        const auto safeChecksCount = checkThreatsCount - unsafeChecksCount;

        packedBonus += SafeCheckBonus.packed[pieceIndex] * safeChecksCount;
        IncrementCoefficients(coefficients, SafeCheckBonus.index + pieceIndex - SafeCheckBonus.start, color, safeChecksCount);

        packedBonus += UnsafeCheckBonus.packed[pieceIndex] * unsafeChecksCount;
        IncrementCoefficients(coefficients, UnsafeCheckBonus.index + pieceIndex - UnsafeCheckBonus.start, color, unsafeChecksCount);
    }

    return packedBonus;
}

int AdditionalPieceEvaluation(int pieceSquareIndex, int pieceIndex, int bucket, int oppositeSideBucket, int sameSideKingSquare, int oppositeSideKingSquare, const u64 opponentPawnAttacks, const std::array<u64, 12> &attacks, const chess::Board &board, const chess::Color &color, coefficients_t &coefficients, std::array<int, 2> &totalKingRingAttacks)
{
    switch (pieceIndex)
    {
    case 0:
    case 6:
        return PawnAdditionalEvaluation(pieceSquareIndex, bucket, oppositeSideBucket, sameSideKingSquare, oppositeSideKingSquare, attacks, board, color, coefficients);

    case 1:
    case 7:
        return KnightAdditionalEvaluation(pieceSquareIndex, opponentPawnAttacks, oppositeSideKingSquare, board, color, coefficients, totalKingRingAttacks);

    case 3:
    case 9:
        return RookAdditionalEvaluation(pieceSquareIndex, bucket, oppositeSideBucket, opponentPawnAttacks, oppositeSideKingSquare, board, color, coefficients, totalKingRingAttacks);

    case 2:
    case 8:
        return BishopAdditionalEvaluation(pieceSquareIndex, pieceIndex, opponentPawnAttacks, oppositeSideKingSquare, board, color, coefficients, totalKingRingAttacks);

    case 4:
    case 10:
        return QueenAdditionalEvaluation(pieceSquareIndex, opponentPawnAttacks, oppositeSideKingSquare, board, color, coefficients, totalKingRingAttacks);

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

bool IsBishopPawnDraw(const chess::Board &board, chess::Color winningSide)
{
    const auto pawns = GetPieceSwappingEndianness(board, chess::PieceType::PAWN, winningSide);

    const bool hasAFilePawn = (pawns & AFile) != 0;
    const bool hasHFilePawn = (pawns & HFile) != 0;

    if (hasAFilePawn && hasHFilePawn)
    {
        return false;
    }

    auto promotionCornerSquare = hasAFilePawn
                                     ? 0  // a8
                                     : 7; // h8

    const auto whiteBlackDiff = 56; // a1 - a8

    // 1 is black is winning
    const auto inverseWinningSide = winningSide == chess::Color::BLACK
                                        ? 1
                                        : 0;

    promotionCornerSquare += inverseWinningSide * whiteBlackDiff;

    const auto defendingKing = chess::builtin::lsb(GetPieceSwappingEndianness(board, chess::PieceType::KING, ~winningSide)).index();

    // Not in the corner or adjacent squares
    if (ChebyshevDistance(promotionCornerSquare, defendingKing) >= 1)
    {
        return false;
    }

    const auto bishopSquare = chess::builtin::lsb(GetPieceSwappingEndianness(board, chess::PieceType::BISHOP, winningSide)).index();
    return DifferentColor(bishopSquare, promotionCornerSquare);
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

    const auto attacks = CalculateAttacks(board);
    const auto attacksBySide = CalculateSideAttacks(attacks);

    std::array<int, 2> totalKingRingAttacks = {0, 0};

    // White pieces PSQTs and additional eval, except king
    for (int pieceIndex = 0; pieceIndex < 5; ++pieceIndex)
    {
        // Bitboard copy that we 'empty'
        auto bitboard = GetPieceSwappingEndianness(board, chess::PieceType(static_cast<chess::PieceType::underlying>(pieceIndex)), chess::Color::WHITE);
        // std::cout << pieceIndex << "bb: " << bitboard << std::endl;

        // Pieces protected by pawns bonus
        const auto protectedPiecesByWhitePawns = chess::builtin::popcount(whitePawnAttacks & bitboard);
        packedScore += (PieceProtectedByPawnBonus.packed[pieceIndex] * protectedPiecesByWhitePawns);
        IncrementCoefficients(coefficients, PieceProtectedByPawnBonus.index - PieceProtectedByPawnBonus.start + pieceIndex, chess::Color::WHITE, protectedPiecesByWhitePawns);

        while (bitboard != 0)
        {
            const auto pieceSquareIndex = chess::builtin::lsb(bitboard).index();
            ResetLS1B(bitboard);

            packedScore += PackedPositionalTables(0, whiteBucket, pieceIndex, pieceSquareIndex) + PackedPieceValue(0, whiteBucket, pieceIndex) +
                           PackedPositionalTables(1, blackBucket, pieceIndex, pieceSquareIndex) + PackedPieceValue(1, blackBucket, pieceIndex);
            gamePhase += phaseValues[pieceIndex];

            ++pieceCount[pieceIndex];

            packedScore += AdditionalPieceEvaluation(pieceSquareIndex, pieceIndex, whiteBucket, blackBucket, whiteKing, blackKing, blackPawnAttacks, attacks, board, chess::Color::WHITE, coefficients, totalKingRingAttacks);

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
        packedScore -= (PieceProtectedByPawnBonus.packed[tunerPieceIndex] * protectedPiecesByBlackPawns);
        IncrementCoefficients(coefficients, PieceProtectedByPawnBonus.index - PieceProtectedByPawnBonus.start + tunerPieceIndex, chess::Color::BLACK, protectedPiecesByBlackPawns);

        while (bitboard != 0)
        {
            const auto pieceSquareIndex = chess::builtin::lsb(bitboard).index();
            ResetLS1B(bitboard);

            packedScore += PackedPositionalTables(0, blackBucket, pieceIndex, pieceSquareIndex) - PackedPieceValue(0, blackBucket, tunerPieceIndex) +
                           PackedPositionalTables(1, whiteBucket, pieceIndex, pieceSquareIndex) - PackedPieceValue(1, whiteBucket, tunerPieceIndex);
            gamePhase += phaseValues[tunerPieceIndex];

            ++pieceCount[pieceIndex];

            packedScore -= AdditionalPieceEvaluation(pieceSquareIndex, pieceIndex, blackBucket, whiteBucket, blackKing, whiteKing, whitePawnAttacks, attacks, board, chess::Color::BLACK, coefficients, totalKingRingAttacks);

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

    // King mobility
    const auto whiteKingAttacks = chess::attacks::king(static_cast<chess::Square>(whiteKing)).getBits();
    const auto whiteMobilityCount = chess::builtin::popcount(
        whiteKingAttacks &
        (~whitePawns) &
        (~blackPawnAttacks));

    packedScore += KingMobilityBonus.packed[whiteMobilityCount];
    IncrementCoefficients(coefficients, KingMobilityBonus.index + whiteMobilityCount - KingMobilityBonus.start, chess::Color::WHITE);

    const auto blackKingAttacks = chess::attacks::king(static_cast<chess::Square>(blackKing)).getBits();
    const auto blackMobilityCount = chess::builtin::popcount(
        blackKingAttacks &
        (~blackPawns) &
        (~whitePawnAttacks));

    packedScore -= KingMobilityBonus.packed[blackMobilityCount];
    IncrementCoefficients(coefficients, KingMobilityBonus.index + blackMobilityCount - KingMobilityBonus.start, chess::Color::BLACK);

    // Bishop pair bonus
    const auto whiteBishops = GetPieceSwappingEndianness(board, chess::PieceType::BISHOP, chess::Color::WHITE);
    const auto blackBishops = GetPieceSwappingEndianness(board, chess::PieceType::BISHOP, chess::Color::BLACK);

    // Pawn king ring attacks
    const auto whiteKingRing = KingRing[whiteKing];
    const auto blackKingRing = KingRing[blackKing];

    const auto whitePawnKingRingAttacks = chess::builtin::popcount(whitePawnAttacks & blackKingRing);
    const auto blackPawnKingRingAttacks = chess::builtin::popcount(blackPawnAttacks & whiteKingRing);

    totalKingRingAttacks[static_cast<int>(chess::Color::WHITE)] += whitePawnKingRingAttacks;
    totalKingRingAttacks[static_cast<int>(chess::Color::BLACK)] += blackPawnKingRingAttacks;

    packedScore += PawnKingRingAttacksBonus.packed;
    IncrementCoefficients(coefficients, PawnKingRingAttacksBonus.index, chess::Color::WHITE, whitePawnKingRingAttacks);

    packedScore -= PawnKingRingAttacksBonus.packed;
    IncrementCoefficients(coefficients, PawnKingRingAttacksBonus.index, chess::Color::BLACK, blackPawnKingRingAttacks);

    // Total king ring attacks
    const auto totalKingRingWhiteAttacks = std::min(13, totalKingRingAttacks[static_cast<int>(chess::Color::WHITE)]);
    packedScore += TotalKingRingAttacksBonus.packed[totalKingRingWhiteAttacks];
    IncrementCoefficients(coefficients, TotalKingRingAttacksBonus.index + totalKingRingWhiteAttacks - TotalKingRingAttacksBonus.start, chess::Color::WHITE);

    const auto totalKingRingBlackAttacks = std::min(13, totalKingRingAttacks[static_cast<int>(chess::Color::BLACK)]);
    packedScore -= TotalKingRingAttacksBonus.packed[totalKingRingBlackAttacks];
    IncrementCoefficients(coefficients, TotalKingRingAttacksBonus.index + totalKingRingBlackAttacks - TotalKingRingAttacksBonus.start, chess::Color::BLACK);

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

    // Threats
    packedScore += Threats(board, chess::Color::WHITE, coefficients, attacks);
    packedScore -= Threats(board, chess::Color::BLACK, coefficients, attacks);

    // Checks
    packedScore += Checks(board, chess::Color::WHITE, coefficients, attacks, attacksBySide);
    packedScore -= Checks(board, chess::Color::BLACK, coefficients, attacks, attacksBySide);

    // Debugging eval
    // return EvalResult{
    //     std::move(coefficients),
    //     (double)(board.sideToMove() == chess::Color::WHITE
    //                  ? eval
    //                  : -eval)};

    // Check if drawn position due to lack of material

    const int maxPhase = 24;

    if (gamePhase > maxPhase) // Early promotions
    {
        gamePhase = maxPhase;
    }

    const int endGamePhase = maxPhase - gamePhase;

    const auto middleGameScore = UnpackMG(packedScore);
    const auto endGameScore = UnpackEG(packedScore);
    int eval = ((middleGameScore * gamePhase) + (endGameScore * endGamePhase)) / maxPhase;

    const int totalPawnsCount = board.pieces(chess::PieceType::PAWN, chess::Color::WHITE).count() +
                                board.pieces(chess::PieceType::PAWN, chess::Color::BLACK).count();

    if (gamePhase <= 5)
    {
        // Pawnless endgames with few pieces
        if (totalPawnsCount == 0)
        {
            switch (gamePhase)
            {
            case 5:
            {
                // RB vs R, RN vs R - scale it down due to the chances of it being a draw
                if (pieceCount[static_cast<int>(chess::PieceType::ROOK)] == 1 &&
                    pieceCount[static_cast<int>(chess::PieceType::ROOK) + 6] == 1)
                {
                    eval >>= 1; // /2
                }

                break;
            }
            case 4:
            {
                // Rook vs 2 minors and R vs r should be a draw
                if (
                    (pieceCount[static_cast<int>(chess::PieceType::ROOK)] != 0 &&
                     (pieceCount[static_cast<int>(chess::PieceType::BISHOP)] + pieceCount[static_cast<int>(chess::PieceType::KNIGHT)] == 0)) ||
                    ((pieceCount[static_cast<int>(chess::PieceType::ROOK) + 6] != 0 &&
                      (pieceCount[static_cast<int>(chess::PieceType::BISHOP) + 6] + pieceCount[static_cast<int>(chess::PieceType::KNIGHT) + 6] == 0))))
                {
                    eval >>= 1; // /2
                }

                break;
            }
            case 3:
            {
                const auto winningSideOffset = PieceOffset(eval >= 0);

                if (pieceCount[1 + winningSideOffset] == 2) // NN vs N, NN vs B
                {
                    return EvalResult{
                        std::move(coefficients),
                        (double)0};
                }

                // Rook vs a minor is a draw
                // Without rooks, only BB vs N is a win and BN vs N can have some chances

                eval >>= 1; // /2

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
        else
        {
            const auto winningSide = eval >= 0 ? chess::Color::WHITE : chess::Color::BLACK;

            if (gamePhase == 1)
            {
                if (GetPieceSwappingEndianness(board, chess::PieceType::BISHOP, winningSide) != 0 &&
                    (GetPieceSwappingEndianness(board, chess::PieceType::PAWN, winningSide) & NotAorH) == 0)
                {
                    if (IsBishopPawnDraw(board, winningSide))
                    {
                        return EvalResult{
                            std::move(coefficients),
                            (double)0};
                    }

                    // We can reduce the rest of positions, i.e. if the king hasn't reached the corner
                    // This also reduces won positions, but it shouldn't matter
                    eval >>= 1; // /2
                }
            }
            else if (gamePhase == 2)
            {
                const auto whiteBishops = GetPieceSwappingEndianness(board, chess::PieceType::BISHOP, chess::Color::WHITE);
                const auto blackBishops = GetPieceSwappingEndianness(board, chess::PieceType::BISHOP, chess::Color::BLACK);

                // Opposite color bishop endgame with pawns
                if (whiteBishops > 0 && blackBishops > 0 &&
                    DarkSquares[chess::builtin::lsb(whiteBishops).index()] != DarkSquares[chess::builtin::lsb(blackBishops).index()])
                {
                    eval >>= 1; // /2
                }
            }
        }
    }

    // Endgame scaling with pawn count
    eval = (int)(eval * ((80 + (totalPawnsCount * 7)) / 128.0));

    // 50 moves rule distance scaling
    eval = ScaleEvalWith50MovesDrawDistance(eval, 0);

    eval = std::clamp(eval, MinEval, MaxEval);

    // Always white's perspective
    return EvalResult{
        std::move(coefficients),
        (double)eval};
}
