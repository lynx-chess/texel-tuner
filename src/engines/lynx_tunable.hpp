#pragma once
#include "../base.h"
#include "../external/chess.hpp"
#include "lynx_base.hpp"
#include <string>
#include <cmath>
#include <climits>

#define NAME(a) #a;
using i32 = int32_t;

class TunableSingle
{
    i32 _mg;
    i32 _eg;

public:
    i32 packed;
    i32 index;
    constexpr static i32 size = 1;

    TunableSingle(const i32 mg, const i32 eg)
        : _mg(mg), _eg(eg)
    {
        packed = S(_mg, _eg);
    }

    void add(parameters_t &parameters)
    {
        index = parameters.size();
        parameters.push_back({(double)_mg, (double)_eg});
    }

    void to_json(const parameters_t &parameters, std::stringstream &ss, const std::string &name)
    {
        ss << "\"" << name << "\": {\n"
           << "\t\"MG\": " << std::round(parameters[index][0]) << ",\n"
           << "\t\"EG\": " << std::round(parameters[index][1]) << "\n}";
    }

    void to_csharp(const parameters_t &parameters, std::stringstream &ss, const std::string &name)
    {
        const auto mg = std::round(parameters[index][0]);
        const auto eg = std::round(parameters[index][1]);

        // Rounded values probably shouldn't be used for the actual packed value generation, but in practice it doesn't make a difference
        // (meausred in https://github.com/lynx-chess/Lynx/pull/1283) and this allows us to have consistency between C# and C++ evaluation
        const auto packed = S(mg, eg);

        ss
            << "\t/// <summary>\n"
            << "\t/// <see cref=\"Utils.Pack(" << mg << ", " << eg << ")\"/>\n"
            << "\t/// </summary>\n"
            << "\tpublic const int " << name << " = " << packed << ";\n"
            << std::endl;
    }

    void to_cpp(const parameters_t &parameters, std::stringstream &ss, const std::string &name)
    {
        ss << "TunableSingle " << name << "(" << std::round(parameters[index][0]) << ", " << std::round(parameters[index][1]) << ");\n";
    }
};

class TunableSingleBucketed
{
    i32 _index;
    std::array<i32, PSQTBucketCount> _packed;

public:
    constexpr static i32 size = PSQTBucketCount;

    TunableSingleBucketed(std::array<i32, PSQTBucketCount> packed)
        : _packed(packed)
    {
    }

    void add(parameters_t &parameters)
    {
        _index = parameters.size();
        for (int i : _packed)
        {
            parameters.push_back({(double)mg_score(i), (double)eg_score(i)});
        }
    }

    int index(int bucket)
    {
        return _index + bucket;
    }

    int packed(int bucket)
    {
        return _packed[bucket];
    }

    void to_csharp(const parameters_t &parameters, std::stringstream &ss, const std::string &name)
    {

        ss << "\tpublic static readonly int[] " << name << " =" << std::endl
           << "\t[" << std::endl;

        for (int i = 0; i < size; ++i)
        {
            auto mg = std::round(parameters[_index + i][0]);
            auto eg = std::round(parameters[_index + i][1]);
            ss << "\t\tPack(" << mg << ", " << eg << ")," << std::endl;
        }

        ss << "\t];\n\n";
    }

    void to_cpp(const parameters_t &parameters, std::stringstream &ss, const std::string &name)
    {
        ss << "TunableSingleBucketed " << name << "(std::array<i32, PSQTBucketCount>(" << std::endl
           << "\t{" << std::endl;

        for (int i = 0; i < size; ++i)
        {
            auto mg = std::round(parameters[_index + i][0]);
            auto eg = std::round(parameters[_index + i][1]);
            ss << "\t\tS(" << mg << ", " << eg << "),\n";
        }

        ss << "\t}));\n\n";
    }
};

class TunableArray
{
    static inline std::array<std::array<tune_t, 12>, PSQTBucketCount> emptyArray;
    std::vector<i32> _mg;
    std::vector<i32> _eg;

public:
    std::vector<i32> packed;
    i32 pieceIndex;
    i32 index;
    i32 size;
    i32 tunableSize;
    i32 start;
    i32 end;

    TunableArray(chess::PieceType piece, const std::vector<i32> mg, const std::vector<i32> eg)
    {
        TunableArray(piece, mg, eg, 0, 0);
    }

    TunableArray(chess::PieceType piece, const std::vector<i32> mg, const std::vector<i32> eg, i32 start, i32 end)
        : pieceIndex(static_cast<int>(piece)), _mg(mg), _eg(eg), start(start), end(end)
    {
        for (int b = 0; b < PSQTBucketCount; ++b)
        {
            emptyArray[b] = std::array<tune_t, 12>{{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};
        }

        if (mg.size() != eg.size())
        {
            throw std::invalid_argument("mg and eg size mismatch");
        }

        size = mg.size();
        tunableSize = size - start - end;

        packed = std::vector<i32>(size);

        for (int rank = 0; rank < size; ++rank)
        {
            packed[rank] = S(mg[rank], eg[rank]);
        }
    }

    void add(parameters_t &parameters)
    {
        index = parameters.size();
        for (int rank = 0 + start; rank < size - end; ++rank)
        {
            parameters.push_back({(double)_mg[rank], (double)_eg[rank]});
        }
    }

    /// Extracts first not-zero value
    pair_t extract_offset(const parameters_t &parameters)
    {
        pair_t packed;

        for (int phase = 0; phase <= 1; ++phase)
        {
            tune_t min = std::numeric_limits<double>::max();

            for (int i = 0; i < size; ++i)
            {
                if (parameters[index + i][phase] != 0.0)
                {
                    min = parameters[index + i][phase];
                    packed[phase] = min;

                    break;
                }
            }
        }

        return packed;
    }

    void to_json(const parameters_t &parameters, std::stringstream &ss, const std::string &name)
    {
        to_json(parameters, ss, name, emptyArray);
    }

    void to_json(const parameters_t &parameters, std::stringstream &ss, const std::string &name, const std::array<std::array<tune_t, 12>, PSQTBucketCount> &mobilityPieceValues)
    {
        const std::string keyword = size == 8
                                        ? "Rank"
                                        : "Count";

        ss << "\"" << name << "\": {\n";
        for (int rank = 0; rank < start; ++rank)
        {
            ss << "\t\"" << keyword << rank << "\": {\n";
            ss << "\t\t\"MG\": " << 0 << ",\n";
            ss << "\t\t\"EG\": " << 0 << "\n\t},\n";
        }

        for (int rank = 0; rank < size - end - start; ++rank)
        {
            ss << "\t\"" << keyword << rank + start << "\": {\n";
            ss << "\t\t\"MG\": " << round(parameters[index + rank][0] - mobilityPieceValues[0][pieceIndex]) << ",\n";
            ss << "\t\t\"EG\": " << round(parameters[index + rank][1] - mobilityPieceValues[0][pieceIndex + 6]) << "\n\t}";

            if (rank != size - 1)
                ss << ",";
            ss << "\n";
        }

        for (int rank = size - end; rank < size; ++rank)
        {
            ss << "\t\"" << keyword << rank << "\": {\n";
            ss << "\t\t\"MG\": " << 0 << ",\n";
            ss << "\t\t\"EG\": " << 0 << "\n\t}\n";

            if (rank != size - 1)
                ss << ",";
            ss << "\n";
        }
        ss << "}";
    }

    void to_csharp(const parameters_t &parameters, std::stringstream &ss, const std::string &name)
    {
        to_csharp(parameters, ss, name, emptyArray);
    }

    void to_csharp(const parameters_t &parameters, std::stringstream &ss, const std::string &name, const std::array<std::array<tune_t, 12>, PSQTBucketCount> &mobilityPieceValues)
    {
        std::string variable_name;

        if (size == 8 || size == 7)
        {
            variable_name = "TaperedEvaluationTermByRank";
        }
        else if (size == 9)
        {
            variable_name = "TaperedEvaluationTermByCount8";
        }
        else if (size == 15)
        {
            variable_name = "TaperedEvaluationTermByCount14";
        }
        else if (size == 28)
        {
            variable_name = "TaperedEvaluationTermByCount27";
        }
        else
        {
            variable_name = "int[]";
        }

        ss << "\tpublic static readonly " << variable_name << " " << name << " =\n\t[\n";
        for (int rank = 0; rank < start; ++rank)
        {
            ss << "\t\tPack(0, 0),\n";
        }

        for (int rank = 0; rank < size - end - start; ++rank)
        {
            auto offsetToRemove0 = parameters[index + rank][0] == 0
                                       ? 0
                                       : mobilityPieceValues[0][pieceIndex];

            auto offsetToRemove1 = parameters[index + rank][1] == 0
                                       ? 0
                                       : mobilityPieceValues[0][pieceIndex + 6];

            ss << "\t\tPack(" << round(parameters[index + rank][0] - offsetToRemove0) << ", " << round(parameters[index + rank][1] - offsetToRemove1) << ")";
            if (rank == size - start - 1)
                ss << "\n\t];";
            else
                ss << ",\n";
        }

        for (int rank = size - end; rank < size; ++rank)
        {
            ss << "\t\tPack(0, 0)";
            if (rank == size - 1)
                ss << "\n\t];";
            else
                ss << ",\n";
        }

        ss << "\n\n";
    }

    void to_cpp(const parameters_t &parameters, std::stringstream &ss, const std::string &name)
    {
        to_cpp(parameters, ss, name, emptyArray);
    }

    void to_cpp(const parameters_t &parameters, std::stringstream &ss, const std::string &name, const std::array<std::array<tune_t, 12>, PSQTBucketCount> &mobilityPieceValues)
    {
        std::string pieceType;
        switch (pieceIndex)
        {
        case static_cast<int>(chess::PieceType::PAWN):
        {
            pieceType = "PAWN";
            break;
        }
        case static_cast<int>(chess::PieceType::KNIGHT):
        {
            pieceType = "KNIGHT";
            break;
        }
        case static_cast<int>(chess::PieceType::BISHOP):
        {
            pieceType = "BISHOP";
            break;
        }
        case static_cast<int>(chess::PieceType::ROOK):
        {
            pieceType = "ROOK";
            break;
        }
        case static_cast<int>(chess::PieceType::QUEEN):
        {
            pieceType = "QUEEN";
            break;
        }
        case static_cast<int>(chess::PieceType::KING):
        {
            pieceType = "KING";
            break;
        }
        }

        ss << "TunableArray " << name << "(\n";
        ss << "\tchess::PieceType::" << pieceType << ",\n";
        ss << "\tstd::vector<int>{";
        for (int rank = 0; rank < start; ++rank)
        {
            ss << "0, ";
        }

        for (int rank = 0; rank < size - end - start; ++rank)
        {
            ss << std::round(parameters[index + rank][0] - mobilityPieceValues[0][pieceIndex]);
            if (rank == size - start - 1)
                ss << "},\n";
            else
                ss << ", ";
        }

        for (int rank = size - end; rank < size; ++rank)
        {
            ss << "0";
            if (rank == size - 1)
                ss << "},\n";
            else
                ss << ", ";
        }

        ss << "\tstd::vector<int>{";

        for (int rank = 0; rank < start; ++rank)
        {
            ss << "0, ";
        }

        for (int rank = 0; rank < size - end - start; ++rank)
        {
            ss << std::round(parameters[index + rank][1] - mobilityPieceValues[0][pieceIndex + 6]);
            if (rank == size - start - 1)
                ss << "},\n";
            else
                ss << ", ";
        }

        for (int rank = size - end; rank < size; ++rank)
        {
            ss << "0";
            if (rank == size - 1)
                ss << "},\n";
            else
                ss << ", ";
        }

        ss << "\t" << start << ",\n";
        ss << "\t" << end << ");\n\n";
    }
};

class TunableArrayBucketed
{
    static inline std::array<std::array<tune_t, 12>, PSQTBucketCount> emptyArray;
    std::array<std::vector<i32>, PSQTBucketCount> _mg;
    std::array<std::vector<i32>, PSQTBucketCount> _eg;
    std::array<std::vector<i32>, PSQTBucketCount> _packed;
    i32 _index;

public:
    i32 pieceIndex;
    i32 size;
    i32 bucketSize;
    i32 bucketTunableSize;
    i32 start;

    TunableArrayBucketed(chess::PieceType piece, const std::array<std::vector<i32>, PSQTBucketCount> mg, const std::array<std::vector<i32>, PSQTBucketCount> eg)
        : TunableArrayBucketed(piece, mg, eg, 0)
    {
    }

    TunableArrayBucketed(chess::PieceType piece, const std::array<std::vector<i32>, PSQTBucketCount> mg, const std::array<std::vector<i32>, PSQTBucketCount> eg, i32 start)
        : pieceIndex(static_cast<int>(piece)), _mg(mg), _eg(eg), start(start)
    {
        for (auto b = 0; b < PSQTBucketCount; ++b)
        {
            emptyArray[b] = std::array<tune_t, 12>{{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};
        }

        assert(mg.size() == eg.size());
        for (auto i = 0; i < mg.size(); ++i)
        {
            assert(mg[i].size() == eg[i].size());
        }

        bucketSize = mg[0].size();
        bucketTunableSize = bucketSize - start;
        size = PSQTBucketCount * bucketTunableSize;
        std::cout << bucketSize << "-" << start << "=" << bucketTunableSize << std::endl;

        _packed = std::array<std::vector<i32>, PSQTBucketCount>();
        for (int bucket = 0; bucket < PSQTBucketCount; ++bucket)
        {
            _packed[bucket] = std::vector<i32>(bucketSize);

            if (_mg[bucket].size() == 0)
            {
                _mg[bucket] = std::vector<i32>(bucketSize, 0);
                std::fill(_mg[bucket].begin(), _mg[bucket].end(), 0);
                std::cout << "[WARN] empty bucket " << bucket << " mg" << std::endl;
            }
            if (_eg[bucket].size() == 0)
            {
                _eg[bucket] = std::vector<i32>(bucketSize, 0);
                std::fill(_eg[bucket].begin(), _eg[bucket].end(), 0);
                std::cout << "[WARN] empty bucket " << bucket << " eg" << std::endl;
            }

            for (int rank = 0 + start; rank < bucketSize; ++rank)
            {
                _packed[bucket][rank] = S(_mg[bucket][rank], _eg[bucket][rank]);
            }
        }
    }

    void add(parameters_t &parameters)
    {
        _index = parameters.size();
        for (int bucket = 0; bucket < PSQTBucketCount; ++bucket)
        {
            for (int rank = 0 + start; rank < bucketSize; ++rank)
            {
                parameters.push_back({(double)_mg[bucket][rank], (double)_eg[bucket][rank]});
            }
        }
    }

    int index(int bucket, int itemIndex)
    {
        return _index + bucket * bucketTunableSize + itemIndex;
    }

    int packed(int bucket, int itemIndex)
    {
        return _packed[bucket][itemIndex];
    }

    /// Extracts first not-zero value
    std::array<pair_t, PSQTBucketCount> extract_offset(const parameters_t &parameters)
    {
        std::array<pair_t, PSQTBucketCount> packed;

        // TODO revisit
        for (int bucket = 0; bucket < PSQTBucketCount; ++bucket)
        {
            packed[bucket] = pair_t();

            for (int phase = 0; phase <= 1; ++phase)
            {
                tune_t min = std::numeric_limits<double>::max();

                for (int i = 0 + start; i < bucketSize; ++i)
                {
                    if (parameters[index(bucket, i)][phase] != 0.0)
                    {
                        min = parameters[index(bucket, i)][phase];
                        packed[bucket][phase] = min;

                        break;
                    }
                }
            }
        }

        return packed;
    }

    void to_csharp(const parameters_t &parameters, std::stringstream &ss, const std::string &name)
    {
        to_csharp(parameters, ss, name, emptyArray);
    }

    void to_csharp(const parameters_t &parameters, std::stringstream &ss, const std::string &name, const std::array<std::array<tune_t, 12>, PSQTBucketCount> &mobilityPieceValues)
    {
        std::string variable_name;

        if (bucketSize == 7)
        {
            variable_name = "TaperedEvaluationTermByRank";
        }
        else if (bucketSize == 9)
        {
            variable_name = "TaperedEvaluationTermByCount8";
        }
        else if (bucketSize == 14 || bucketSize == 15)
        {
            variable_name = "TaperedEvaluationTermByCount14";
        }
        else if (bucketSize == 28)
        {
            variable_name = "TaperedEvaluationTermByCount27";
        }
        else
        {
            variable_name = "int[]";
        }

        ss << "\tpublic static readonly " << variable_name << "[] " << name << " =\n\t[\n";

        for (int bucket = 0; bucket < PSQTBucketCount; ++bucket)
        {
            ss << "\t\t[\n";
            for (int dimension = 0; dimension < start; ++dimension)
            {
                ss << "\t\t\tPack(0, 0),\n";
            }

            for (int dimension = 0; dimension < bucketTunableSize; ++dimension)
            {
                auto offsetToRemove0 = parameters[index(bucket, dimension)][0] == 0.0
                                           ? 0
                                           : mobilityPieceValues[bucket][pieceIndex];

                auto offsetToRemove1 = parameters[index(bucket, dimension)][1] == 0.0
                                           ? 0
                                           : mobilityPieceValues[bucket][pieceIndex + 6];

                ss << "\t\t\tPack(" << round(parameters[index(bucket, dimension)][0] - offsetToRemove0) << ", " << round(parameters[index(bucket, dimension)][1] - offsetToRemove1) << ")";
                if (dimension == size - start - 1)
                    ss << ");";
                else
                    ss << ",\n";
            }

            ss << "\t\t],\n";
        }

        ss << "\t];\n\n";
    }

    void to_cpp(const parameters_t &parameters, std::stringstream &ss, const std::string &name)
    {
        to_cpp(parameters, ss, name, emptyArray);
    }

    void to_cpp(const parameters_t &parameters, std::stringstream &ss, const std::string &name, const std::array<std::array<tune_t, 12>, PSQTBucketCount> &mobilityPieceValues)
    {
        std::string pieceType;
        switch (pieceIndex)
        {
        case static_cast<int>(chess::PieceType::PAWN):
        {
            pieceType = "PAWN";
            break;
        }
        case static_cast<int>(chess::PieceType::KNIGHT):
        {
            pieceType = "KNIGHT";
            break;
        }
        case static_cast<int>(chess::PieceType::BISHOP):
        {
            pieceType = "BISHOP";
            break;
        }
        case static_cast<int>(chess::PieceType::ROOK):
        {
            pieceType = "ROOK";
            break;
        }
        case static_cast<int>(chess::PieceType::QUEEN):
        {
            pieceType = "QUEEN";
            break;
        }
        case static_cast<int>(chess::PieceType::KING):
        {
            pieceType = "KING";
            break;
        }
        }

        ss << "TunableArrayBucketed " << name << "(\n";
        ss << "\tchess::PieceType::" << pieceType << ",\n";

        for (int phase = 0; phase < 2; ++phase)
        {

            ss << "\tstd::array<std::vector<int>, PSQTBucketCount>{{\n";
            for (int bucket = 0; bucket < PSQTBucketCount; ++bucket)
            {
                ss << "\t\tstd::vector<int>{";
                for (int dimension = 0; dimension < start; ++dimension)
                {
                    ss << "0, ";
                }

                for (int dimension = 0; dimension < bucketSize - start; ++dimension)
                {
                    auto offsetToRemove = parameters[index(bucket, dimension)][1] == 0.0
                                              ? 0
                                              : mobilityPieceValues[bucket][pieceIndex + 6 * phase];

                    ss << std::round(parameters[index(bucket, dimension)][phase] - offsetToRemove);
                    if (dimension == bucketSize - start - 1)
                        ss << "},\n";
                    else
                        ss << ", ";
                }
            }

            ss << "\n\t}},\n";
        }

        ss << "\t" << start << ");\n\n";
    }
};
