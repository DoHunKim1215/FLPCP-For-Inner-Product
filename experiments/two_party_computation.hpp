#ifndef TWO_PARTY_COMPUTATION_H
#define TWO_PARTY_COMPUTATION_H

#include <chrono>
#include <iostream>

#include "network.hpp"
#include "../circuit/inner_product_circuit.hpp"
#include "../math/square_matrix.hpp"
#include "../unit/proof.hpp"

struct FLPCPMeasurement
{
    size_t proofLength;
    size_t nQueries;
    double proverTime;
    double verifierTime;
    bool isVaild;
};

struct FLIOPMeasurement
{
    size_t proofLength;
    size_t nQueries;
    double proverTime;
    double verifierTime;
    double LANTime;
    double WANTime;
    bool isVaild;
};

template <typename Int> class TwoPC
{
public:
    static FLPCPMeasurement FLPCP(uint32_t seed, size_t inputLength, size_t nGGate);
    static FLPCPMeasurement FLPCPWithPrecompute(size_t inputLength, size_t nGGate);
    static void ExperimentFLPCP();
    static void ExperimentFLPCPWithPrecompute();
    static void ExperimentFLPCPSqrt();
    static void ExperimentFLPCPSqrtWithPrecompute();
    static FLPCPMeasurement FLPCPCoefficient(size_t inputLength, size_t nGGate);
    static void ExperimentFLPCPCoefficient();
    static void ExperimentFLPCPCoefficientSqrt();
    static FLIOPMeasurement FLIOP(const size_t inputLength, const size_t compressFactor);
    static FLIOPMeasurement FLIOPWithPrecompute(const size_t inputLength, const size_t compressFactor);
    static void ExperimentFLIOP();
    static void ExperimentFLIOPWithPrecompute();
    static FLIOPMeasurement FLIOPWithRandomOracle(const size_t inputLength, const size_t compressFactor);
    static FLIOPMeasurement FLIOPWithRandomOracleAndPrecompute(const size_t inputLength, const size_t compressFactor);
    static void ExperimentFLIOPWithRandomOracle();
    static void ExperimentFLIOPWithRandomOracleAndPrecompute();
    static FLIOPMeasurement FLIOPCoefficient(const size_t inputLength, const size_t compressFactor);
    static void ExperimentFLIOPCoefficient();
    static FLIOPMeasurement FLIOPCoefficientWithRandomOracle(const size_t inputLength, const size_t compressFactor);
    static void ExperimentFLIOPCoefficientWithRandomOracle();
};

template <typename Int> FLPCPMeasurement TwoPC<Int>::FLPCP(uint32_t seed, size_t inputLength, size_t nGGate)
{
    Int::SetSeed(seed);

    Int* const op0 = new Int[inputLength];
    for (size_t i = 0; i < inputLength; ++i)
    {
        op0[i] = Int::GenerateRandom();
    }

    Int* const op1 = new Int[inputLength];
    for (size_t i = 0; i < inputLength; ++i)
    {
        op1[i] = Int::GenerateRandom();
    }

    const Int circuitOutput = InnerProductCircuit<Int>::Forward(op0, op1, inputLength);

    // Prover make proof vector : (inputs || constant terms || coefficients)
    auto start_proof = std::chrono::high_resolution_clock::now();
    Proof<Int> proof = InnerProductCircuit<Int>::MakeProof(op0, op1, inputLength, nGGate);
    auto end_proof = std::chrono::high_resolution_clock::now();
    double time_taken_proof = std::chrono::duration_cast<std::chrono::nanoseconds>(end_proof - start_proof).count();

    // Verifier make queries and perform inner products between proof and queries.
    // Assumption : Verifier only has linear access on proof vector.
    auto start_query = std::chrono::high_resolution_clock::now();
    std::vector<Query<Int>> queries =
        InnerProductCircuit<Int>::MakeQuery(Int::GenerateRandomAbove(nGGate + 1), nGGate, inputLength);

    const size_t nInputQueriesHalf = (queries.size() - 2u) / 2u;
    Int gR(0u);
    for (size_t i = 0; i < nInputQueriesHalf; ++i)
    {
        gR += proof.GetQueryAnswer(queries[i]) * proof.GetQueryAnswer(queries[i + nInputQueriesHalf]);
    }

    bool isValid = (proof.GetQueryAnswer(queries[queries.size() - 2u]) == gR) &&
                   (proof.GetQueryAnswer(queries[queries.size() - 1u]) == circuitOutput);

    auto end_query = std::chrono::high_resolution_clock::now();
    double time_taken_query = std::chrono::duration_cast<std::chrono::nanoseconds>(end_query - start_query).count();

    delete[] op0;
    delete[] op1;

    return FLPCPMeasurement(proof.GetBytes(), queries.size(), time_taken_proof * 1e-6, time_taken_query * 1e-6,
                            isValid);
}

template <typename Int> FLPCPMeasurement TwoPC<Int>::FLPCPWithPrecompute(size_t inputLength, size_t nGGate)
{
    Int::SetSeed(10);

    Int* const op0 = new Int[inputLength];
    for (size_t i = 0; i < inputLength; ++i)
    {
        op0[i] = Int::GenerateRandom();
    }

    Int* const op1 = new Int[inputLength];
    for (size_t i = 0; i < inputLength; ++i)
    {
        op1[i] = Int::GenerateRandom();
    }

    const Int circuitOutput = InnerProductCircuit<Int>::Forward(op0, op1, inputLength);

    // Precompute a Vandermonde matrix for interpolation
    Int* xs = new Int[nGGate + 1];
    for (size_t i = 0; i < nGGate + 1; ++i)
    {
        xs[i] = Int(i);
    }
    SquareMatrix<Int> evalToCoeff = SquareMatrix<Int>::GetVandermonde(xs, nGGate + 1);
    evalToCoeff.Inverse();
    delete[] xs;
    xs = (Int*)0;

    // Precompute a process to make linear queries
    std::vector<Query<Int>> queries =
        InnerProductCircuit<Int>::MakeQuery(Int::GenerateRandomAbove(nGGate + 1), nGGate, inputLength);
    const size_t nInputQueriesHalf = (queries.size() - 2u) / 2u;

    // Prover make proof vector : (inputs || constant terms || coefficients)
    auto start_proof = std::chrono::high_resolution_clock::now();
    Proof<Int> proof = InnerProductCircuit<Int>::MakeProofWithPrecompute(op0, op1, inputLength, nGGate, evalToCoeff);
    auto end_proof = std::chrono::high_resolution_clock::now();
    double time_taken_proof = std::chrono::duration_cast<std::chrono::nanoseconds>(end_proof - start_proof).count();

    // Verifier make queries and perform inner products between proof and queries.
    // Assumption : Verifier only has linear access on proof vector.
    auto start_query = std::chrono::high_resolution_clock::now();
    Int gR(0u);
    for (size_t i = 0; i < nInputQueriesHalf; ++i)
    {
        gR += proof.GetQueryAnswer(queries[i]) * proof.GetQueryAnswer(queries[i + nInputQueriesHalf]);
    }

    bool isValid = (proof.GetQueryAnswer(queries[queries.size() - 2u]) == gR) &&
                   (proof.GetQueryAnswer(queries[queries.size() - 1u]) == circuitOutput);

    auto end_query = std::chrono::high_resolution_clock::now();
    double time_taken_query = std::chrono::duration_cast<std::chrono::nanoseconds>(end_query - start_query).count();

    delete[] op0;
    delete[] op1;

    return FLPCPMeasurement(proof.GetBytes(), queries.size(), time_taken_proof * 1e-6, time_taken_query * 1e-6,
                            isValid);
}

template <typename Int> void TwoPC<Int>::ExperimentFLPCP()
{
    size_t j = 0;
    FLPCPMeasurement measures[11];
    for (size_t i = 4; i <= 4096; i *= 2)
    {
        measures[j] = TwoPC<Int>::FLPCP(i, i);
        if (!measures[j++].isVaild)
        {
            std::cout << "Invalid!" << std::endl;
            return;
        }
    }

    std::cout << "Vector Length : ";
    for (size_t i = 4; i <= 4096; i *= 2)
    {
        std::cout << i << ", ";
    }
    std::cout << std::endl;

    std::cout << "# of G-gates : ";
    for (size_t i = 4; i <= 4096; i *= 2)
    {
        std::cout << i << ", ";
    }
    std::cout << std::endl;

    std::cout << "Proof Length : ";
    for (size_t i = 0; i < 11; ++i)
    {
        std::cout << measures[i].proofLength << ", ";
    }
    std::cout << std::endl;

    std::cout << "Query Complexity : ";
    for (size_t i = 0; i < 11; ++i)
    {
        std::cout << measures[i].nQueries << ", ";
    }
    std::cout << std::endl;

    std::cout << "Prover Time : ";
    for (size_t i = 0; i < 11; ++i)
    {
        std::cout << std::fixed << measures[i].proverTime << std::setprecision(9) << ", ";
    }
    std::cout << std::endl;

    std::cout << "Verifier Time : ";
    for (size_t i = 0; i < 11; ++i)
    {
        std::cout << std::fixed << measures[i].verifierTime << std::setprecision(9) << ", ";
    }
    std::cout << std::endl;
}

template <typename Int> void TwoPC<Int>::ExperimentFLPCPWithPrecompute()
{
    size_t j = 0;
    FLPCPMeasurement measures[11];
    for (size_t i = 4; i <= 4096; i *= 2)
    {
        measures[j] = TwoPC<Int>::FLPCPWithPrecompute(i, i);
        if (!measures[j++].isVaild)
        {
            std::cout << "Invalid!" << std::endl;
            return;
        }
    }

    std::cout << "Vector Length : ";
    for (size_t i = 4; i <= 4096; i *= 2)
    {
        std::cout << i << ", ";
    }
    std::cout << std::endl;

    std::cout << "# of G-gates : ";
    for (size_t i = 4; i <= 4096; i *= 2)
    {
        std::cout << i << ", ";
    }
    std::cout << std::endl;

    std::cout << "Proof Length : ";
    for (size_t i = 0; i < 11; ++i)
    {
        std::cout << measures[i].proofLength << ", ";
    }
    std::cout << std::endl;

    std::cout << "Query Complexity : ";
    for (size_t i = 0; i < 11; ++i)
    {
        std::cout << measures[i].nQueries << ", ";
    }
    std::cout << std::endl;

    std::cout << "Prover Time : ";
    for (size_t i = 0; i < 11; ++i)
    {
        std::cout << std::fixed << measures[i].proverTime << std::setprecision(9) << ", ";
    }
    std::cout << std::endl;

    std::cout << "Verifier Time : ";
    for (size_t i = 0; i < 11; ++i)
    {
        std::cout << std::fixed << measures[i].verifierTime << std::setprecision(9) << ", ";
    }
    std::cout << std::endl;
}

template <typename Int> void TwoPC<Int>::ExperimentFLPCPSqrt()
{
    size_t j = 0;
    FLPCPMeasurement measures[6];
    for (size_t i = 2; i <= 64; i *= 2)
    {
        measures[j] = TwoPC<Int>::FLPCP(i * i, i);
        if (!measures[j++].isVaild)
        {
            std::cout << "Invalid!" << std::endl;
            return;
        }
    }

    std::cout << "Vector Length : ";
    for (size_t i = 2; i <= 64; i *= 2)
    {
        std::cout << i * i << ", ";
    }
    std::cout << std::endl;

    std::cout << "# of G-gates : ";
    for (size_t i = 2; i <= 64; i *= 2)
    {
        std::cout << i << ", ";
    }
    std::cout << std::endl;

    std::cout << "Proof Length : ";
    for (size_t i = 0; i < 6; ++i)
    {
        std::cout << measures[i].proofLength << ", ";
    }
    std::cout << std::endl;

    std::cout << "Query Complexity : ";
    for (size_t i = 0; i < 6; ++i)
    {
        std::cout << measures[i].nQueries << ", ";
    }
    std::cout << std::endl;

    std::cout << "Prover Time : ";
    for (size_t i = 0; i < 6; ++i)
    {
        std::cout << std::fixed << measures[i].proverTime << std::setprecision(9) << ", ";
    }
    std::cout << std::endl;

    std::cout << "Verifier Time : ";
    for (size_t i = 0; i < 6; ++i)
    {
        std::cout << std::fixed << measures[i].verifierTime << std::setprecision(9) << ", ";
    }
    std::cout << std::endl;
}

template <typename Int> void TwoPC<Int>::ExperimentFLPCPSqrtWithPrecompute()
{
    size_t j = 0;
    FLPCPMeasurement measures[6];
    for (size_t i = 2; i <= 64; i *= 2)
    {
        measures[j] = TwoPC<Int>::FLPCPWithPrecompute(i * i, i);
        if (!measures[j++].isVaild)
        {
            std::cout << "Invalid!" << std::endl;
            return;
        }
    }

    std::cout << "Vector Length : ";
    for (size_t i = 2; i <= 64; i *= 2)
    {
        std::cout << i * i << ", ";
    }
    std::cout << std::endl;

    std::cout << "# of G-gates : ";
    for (size_t i = 2; i <= 64; i *= 2)
    {
        std::cout << i << ", ";
    }
    std::cout << std::endl;

    std::cout << "Proof Length : ";
    for (size_t i = 0; i < 6; ++i)
    {
        std::cout << measures[i].proofLength << ", ";
    }
    std::cout << std::endl;

    std::cout << "Query Complexity : ";
    for (size_t i = 0; i < 6; ++i)
    {
        std::cout << measures[i].nQueries << ", ";
    }
    std::cout << std::endl;

    std::cout << "Prover Time : ";
    for (size_t i = 0; i < 6; ++i)
    {
        std::cout << std::fixed << measures[i].proverTime << std::setprecision(9) << ", ";
    }
    std::cout << std::endl;

    std::cout << "Verifier Time : ";
    for (size_t i = 0; i < 6; ++i)
    {
        std::cout << std::fixed << measures[i].verifierTime << std::setprecision(9) << ", ";
    }
    std::cout << std::endl;
}

// Fully Linear PCP using input as coefficient of polynomials
template <typename Int> FLPCPMeasurement TwoPC<Int>::FLPCPCoefficient(size_t inputLength, size_t nPoly)
{
    Int::SetSeed(10);

    Int* const op0 = new Int[inputLength];
    for (size_t i = 0; i < inputLength; ++i)
    {
        op0[i] = Int::GenerateRandom();
    }

    Int* const op1 = new Int[inputLength];
    for (size_t i = 0; i < inputLength; ++i)
    {
        op1[i] = Int::GenerateRandom();
    }

    const Int trueResult = InnerProductCircuit<Int>::Forward(op0, op1, inputLength);

    auto start_proof = std::chrono::high_resolution_clock::now();
    Proof<Int> proof = InnerProductCircuit<Int>::MakeCoefficientProof(op0, op1, inputLength, nPoly);
    auto end_proof = std::chrono::high_resolution_clock::now();
    double time_taken_proof = std::chrono::duration_cast<std::chrono::nanoseconds>(end_proof - start_proof).count();

    auto start_query = std::chrono::high_resolution_clock::now();
    std::vector<Query<Int>> queries =
        InnerProductCircuit<Int>::MakeCoefficientQuery(Int::GenerateRandom(), inputLength, nPoly);
    const size_t nInputQueriesHalf = (queries.size() - 2u) / 2u;

    Int gR(0u);
    for (size_t i = 0; i < nInputQueriesHalf; ++i)
    {
        gR += proof.GetQueryAnswer(queries[i]) * proof.GetQueryAnswer(queries[i + nInputQueriesHalf]);
    }

    bool isValid = (proof.GetQueryAnswer(queries[queries.size() - 2u]) == gR) &&
                   (proof.GetQueryAnswer(queries[queries.size() - 1u]) == trueResult);

    auto end_query = std::chrono::high_resolution_clock::now();
    double time_taken_query = std::chrono::duration_cast<std::chrono::nanoseconds>(end_query - start_query).count();

    delete[] op0;
    delete[] op1;

    return FLPCPMeasurement(proof.GetBytes(), queries.size(), time_taken_proof * 1e-6, time_taken_query * 1e-6,
                            isValid);
}

template <typename Int> void TwoPC<Int>::ExperimentFLPCPCoefficient()
{
    size_t j = 0;
    FLPCPMeasurement measures[11];
    for (size_t i = 4; i <= 4096; i *= 2)
    {
        measures[j] = TwoPC<Int>::FLPCPCoefficient(i, 1);
        if (!measures[j++].isVaild)
        {
            std::cout << "Invalid!" << std::endl;
            return;
        }
    }

    std::cout << "Vector Length : ";
    for (size_t i = 4; i <= 4096; i *= 2)
    {
        std::cout << i << ", ";
    }
    std::cout << std::endl;

    std::cout << "# of polynomial : ";
    for (size_t i = 4; i <= 4096; i *= 2)
    {
        std::cout << 1 << ", ";
    }
    std::cout << std::endl;

    std::cout << "Proof Length : ";
    for (size_t i = 0; i < 11; ++i)
    {
        std::cout << measures[i].proofLength << ", ";
    }
    std::cout << std::endl;

    std::cout << "Query Complexity : ";
    for (size_t i = 0; i < 11; ++i)
    {
        std::cout << measures[i].nQueries << ", ";
    }
    std::cout << std::endl;

    std::cout << "Prover Time : ";
    for (size_t i = 0; i < 11; ++i)
    {
        std::cout << std::fixed << measures[i].proverTime << std::setprecision(9) << ", ";
    }
    std::cout << std::endl;

    std::cout << "Verifier Time : ";
    for (size_t i = 0; i < 11; ++i)
    {
        std::cout << std::fixed << measures[i].verifierTime << std::setprecision(9) << ", ";
    }
    std::cout << std::endl;
}

template <typename Int> void TwoPC<Int>::ExperimentFLPCPCoefficientSqrt()
{
    size_t j = 0;
    FLPCPMeasurement measures[6];
    for (size_t i = 2; i <= 64; i *= 2)
    {
        measures[j] = TwoPC<Int>::FLPCPCoefficient(i * i, i);
        if (!measures[j++].isVaild)
        {
            std::cout << "Invalid!" << std::endl;
            return;
        }
    }

    std::cout << "Vector Length : ";
    for (size_t i = 2; i <= 64; i *= 2)
    {
        std::cout << i * i << ", ";
    }
    std::cout << std::endl;

    std::cout << "# of G-gates : ";
    for (size_t i = 2; i <= 64; i *= 2)
    {
        std::cout << i << ", ";
    }
    std::cout << std::endl;

    std::cout << "Proof Length : ";
    for (size_t i = 0; i < 6; ++i)
    {
        std::cout << measures[i].proofLength << ", ";
    }
    std::cout << std::endl;

    std::cout << "Query Complexity : ";
    for (size_t i = 0; i < 6; ++i)
    {
        std::cout << measures[i].nQueries << ", ";
    }
    std::cout << std::endl;

    std::cout << "Prover Time : ";
    for (size_t i = 0; i < 6; ++i)
    {
        std::cout << std::fixed << measures[i].proverTime << std::setprecision(9) << ", ";
    }
    std::cout << std::endl;

    std::cout << "Verifier Time : ";
    for (size_t i = 0; i < 6; ++i)
    {
        std::cout << std::fixed << measures[i].verifierTime << std::setprecision(9) << ", ";
    }
    std::cout << std::endl;
}

template <typename Int> FLIOPMeasurement TwoPC<Int>::FLIOP(const size_t inputLength, const size_t compressFactor)
{
    Int::SetSeed(10u);

    std::vector<Int> op0(inputLength);
    for (size_t i = 0; i < inputLength; ++i)
    {
        op0[i] = Int::GenerateRandom();
    }
    std::vector<Int> op1(inputLength);
    for (size_t i = 0; i < inputLength; ++i)
    {
        op1[i] = Int::GenerateRandom();
    }

    double proverTime = 0.;
    double verifierTime = 0.;
    double LANTime = 0.;
    double WANTime = 0.;
    size_t totalProofSize = 0u;
    size_t totalQueryComplexity = 0u;

    bool isValid = true;
    Int out = InnerProductCircuit<Int>::Forward(op0.data(), op1.data(), inputLength);
    while (ceil(op0.size() / (double)compressFactor) > 1)
    {
        // Prover
        auto start = std::chrono::high_resolution_clock::now();
        InteractiveProof<Int> proof =
            InnerProductCircuit<Int>::MakeRoundProof(op0.data(), op1.data(), op0.size(), compressFactor);
        auto end = std::chrono::high_resolution_clock::now();
        proverTime += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

        // Communication : Prover give the verifier a single round proof
        LANTime += Network::GetLANDelay(proof.GetBytes());
        WANTime += Network::GetWANDelay(proof.GetBytes());

        // Verifier
        start = std::chrono::high_resolution_clock::now();
        Int random = Int::GenerateRandom();
        std::vector<Query<Int>> roundQueries = InnerProductCircuit<Int>::MakeRoundQuery(random, compressFactor);
        isValid = isValid && (out == proof.GetQueryAnswer(roundQueries[0]));
        out = proof.GetQueryAnswer(roundQueries[1]);
        end = std::chrono::high_resolution_clock::now();
        verifierTime += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

        // Communication : Verifier give the prover a single round randomness
        LANTime += Network::GetLANDelay(sizeof(Int));
        WANTime += Network::GetWANDelay(sizeof(Int));

        // Prover
        start = std::chrono::high_resolution_clock::now();
        op0 = proof.EvaluatePolyPs(random);
        op1 = proof.EvaluatePolyQs(random);
        end = std::chrono::high_resolution_clock::now();
        proverTime += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

        totalProofSize += proof.GetBytes();
        totalQueryComplexity += 2;
    }

    // Prover
    auto start = std::chrono::high_resolution_clock::now();
    Proof<Int> proof = InnerProductCircuit<Int>::MakeProof(op0.data(), op1.data(), op0.size(), op0.size());
    auto end = std::chrono::high_resolution_clock::now();
    proverTime += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

    // Communication : Prover give the verifier a final round proof
    LANTime += Network::GetLANDelay(op0.size() * 2 * sizeof(Int) + proof.GetBytes());
    WANTime += Network::GetWANDelay(op0.size() * 2 * sizeof(Int) + proof.GetBytes());

    // Verifier
    start = std::chrono::high_resolution_clock::now();
    std::vector<Query<Int>> queries =
        InnerProductCircuit<Int>::MakeQuery(Int::GenerateRandom(), op0.size(), op0.size());
    const size_t nInputQueriesHalf = (queries.size() - 2u) / 2u;
    Int gR((uint64_t)0);
    for (size_t i = 0; i < nInputQueriesHalf; ++i)
    {
        gR += proof.GetQueryAnswer(queries[i]) * proof.GetQueryAnswer(queries[i + nInputQueriesHalf]);
    }
    isValid = isValid && (proof.GetQueryAnswer(queries[queries.size() - 2u]) == gR) &&
              (proof.GetQueryAnswer(queries[queries.size() - 1u]) == out);

    end = std::chrono::high_resolution_clock::now();
    verifierTime += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

    totalProofSize += proof.GetBytes();
    totalQueryComplexity += queries.size();

    return FLIOPMeasurement(totalProofSize, totalQueryComplexity, proverTime * 1e-6, verifierTime * 1e-6,
                            LANTime * 1e-6, WANTime * 1e-6, isValid);
}

template <typename Int> FLIOPMeasurement TwoPC<Int>::FLIOPWithPrecompute(const size_t inputLength, const size_t compressFactor)
{
    Int::SetSeed(10u);

    std::vector<Int> op0(inputLength);
    for (size_t i = 0; i < inputLength; ++i)
    {
        op0[i] = Int::GenerateRandom();
    }
    std::vector<Int> op1(inputLength);
    for (size_t i = 0; i < inputLength; ++i)
    {
        op1[i] = Int::GenerateRandom();
    }

    double proverTime = 0.;
    double verifierTime = 0.;
    double LANTime = 0.;
    double WANTime = 0.;
    size_t totalProofSize = 0u;
    size_t totalQueryComplexity = 0u;

    // Precompute a Vandermonde matrix
    Int* xs = new Int[compressFactor];
    for (size_t i = 0; i < compressFactor; ++i)
    {
        xs[i] = Int(i);
    }
    SquareMatrix<Int> evalToCoeff = SquareMatrix<Int>::GetVandermonde(xs, compressFactor);
    evalToCoeff.Inverse();
    delete[] xs;
    xs = (Int*)0;

    bool isValid = true;
    Int out = InnerProductCircuit<Int>::Forward(op0.data(), op1.data(), inputLength);
    while (ceil(op0.size() / (double)compressFactor) > 1)
    {
        // Prover
        auto start = std::chrono::high_resolution_clock::now();
        InteractiveProof<Int> proof = InnerProductCircuit<Int>::MakeRoundProofWithPrecompute(
            op0.data(), op1.data(), op0.size(), compressFactor, evalToCoeff);
        auto end = std::chrono::high_resolution_clock::now();
        proverTime += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

        // Communication : Prover give the verifier a single round proof
        LANTime += Network::GetLANDelay(proof.GetBytes());
        WANTime += Network::GetWANDelay(proof.GetBytes());

        // Verifier
        start = std::chrono::high_resolution_clock::now();
        Int random = Int::GenerateRandom();
        std::vector<Query<Int>> roundQueries = InnerProductCircuit<Int>::MakeRoundQuery(random, compressFactor);
        isValid = isValid && (out == proof.GetQueryAnswer(roundQueries[0]));
        out = proof.GetQueryAnswer(roundQueries[1]);
        end = std::chrono::high_resolution_clock::now();
        verifierTime += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

        // Communication : Verifier give the prover a single round randomness
        LANTime += Network::GetLANDelay(sizeof(Int));
        WANTime += Network::GetWANDelay(sizeof(Int));

        // Prover
        start = std::chrono::high_resolution_clock::now();
        op0 = proof.EvaluatePolyPs(random);
        op1 = proof.EvaluatePolyQs(random);
        end = std::chrono::high_resolution_clock::now();
        proverTime += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

        totalProofSize += proof.GetBytes();
        totalQueryComplexity += 2;
    }

    // Prover
    auto start = std::chrono::high_resolution_clock::now();
    Proof<Int> proof = InnerProductCircuit<Int>::MakeProof(op0.data(), op1.data(), op0.size(), op0.size());
    auto end = std::chrono::high_resolution_clock::now();
    proverTime += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

    // Communication : Prover give the verifier a final round proof
    LANTime += Network::GetLANDelay(op0.size() * 2 * sizeof(Int) + proof.GetBytes());
    WANTime += Network::GetWANDelay(op0.size() * 2 * sizeof(Int) + proof.GetBytes());

    // Verifier
    start = std::chrono::high_resolution_clock::now();
    std::vector<Query<Int>> queries =
        InnerProductCircuit<Int>::MakeQuery(Int::GenerateRandom(), op0.size(), op0.size());
    const size_t nInputQueriesHalf = (queries.size() - 2u) / 2u;
    Int gR((uint64_t)0);
    for (size_t i = 0; i < nInputQueriesHalf; ++i)
    {
        gR += proof.GetQueryAnswer(queries[i]) * proof.GetQueryAnswer(queries[i + nInputQueriesHalf]);
    }
    isValid = isValid && (proof.GetQueryAnswer(queries[queries.size() - 2u]) == gR) &&
              (proof.GetQueryAnswer(queries[queries.size() - 1u]) == out);

    end = std::chrono::high_resolution_clock::now();
    verifierTime += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

    totalProofSize += proof.GetBytes();
    totalQueryComplexity += queries.size();

    return FLIOPMeasurement(totalProofSize, totalQueryComplexity, proverTime * 1e-6, verifierTime * 1e-6,
                            LANTime * 1e-6, WANTime * 1e-6, isValid);
}

template <typename Int> void TwoPC<Int>::ExperimentFLIOP()
{
    size_t j = 0;
    FLIOPMeasurement measures[11];
    for (size_t i = 4; i <= 4096; i *= 2)
    {
        measures[j] = TwoPC<Int>::FLIOP(i, 2);
        if (!measures[j++].isVaild)
        {
            std::cout << "Invalid!" << std::endl;
            return;
        }
    }

    std::cout << "Vector Length : ";
    for (size_t i = 4; i <= 4096; i *= 2)
    {
        std::cout << i << ", ";
    }
    std::cout << std::endl;

    std::cout << "Proof Length : ";
    for (size_t i = 0; i < 11; ++i)
    {
        std::cout << measures[i].proofLength << ", ";
    }
    std::cout << std::endl;

    std::cout << "Query Complexity : ";
    for (size_t i = 0; i < 11; ++i)
    {
        std::cout << measures[i].nQueries << ", ";
    }
    std::cout << std::endl;

    std::cout << "Prover Time : ";
    for (size_t i = 0; i < 11; ++i)
    {
        std::cout << std::fixed << measures[i].proverTime << std::setprecision(9) << ", ";
    }
    std::cout << std::endl;

    std::cout << "Verifier Time : ";
    for (size_t i = 0; i < 11; ++i)
    {
        std::cout << std::fixed << measures[i].verifierTime << std::setprecision(9) << ", ";
    }
    std::cout << std::endl;

    std::cout << "LAN Time : ";
    for (size_t i = 0; i < 11; ++i)
    {
        std::cout << std::fixed << measures[i].LANTime << std::setprecision(9) << ", ";
    }
    std::cout << std::endl;

    std::cout << "WANTime : ";
    for (size_t i = 0; i < 11; ++i)
    {
        std::cout << std::fixed << measures[i].WANTime << std::setprecision(9) << ", ";
    }
    std::cout << std::endl;
}

template <typename Int> void TwoPC<Int>::ExperimentFLIOPWithPrecompute()
{
    size_t j = 0;
    FLIOPMeasurement measures[11];
    for (size_t i = 4; i <= 4096; i *= 2)
    {
        measures[j] = TwoPC<Int>::FLIOPWithPrecompute(i, 2);
        if (!measures[j++].isVaild)
        {
            std::cout << "Invalid!" << std::endl;
            return;
        }
    }

    std::cout << "Vector Length : ";
    for (size_t i = 4; i <= 4096; i *= 2)
    {
        std::cout << i << ", ";
    }
    std::cout << std::endl;

    std::cout << "Proof Length : ";
    for (size_t i = 0; i < 11; ++i)
    {
        std::cout << measures[i].proofLength << ", ";
    }
    std::cout << std::endl;

    std::cout << "Query Complexity : ";
    for (size_t i = 0; i < 11; ++i)
    {
        std::cout << measures[i].nQueries << ", ";
    }
    std::cout << std::endl;

    std::cout << "Prover Time : ";
    for (size_t i = 0; i < 11; ++i)
    {
        std::cout << std::fixed << measures[i].proverTime << std::setprecision(9) << ", ";
    }
    std::cout << std::endl;

    std::cout << "Verifier Time : ";
    for (size_t i = 0; i < 11; ++i)
    {
        std::cout << std::fixed << measures[i].verifierTime << std::setprecision(9) << ", ";
    }
    std::cout << std::endl;

    std::cout << "LAN Time : ";
    for (size_t i = 0; i < 11; ++i)
    {
        std::cout << std::fixed << measures[i].LANTime << std::setprecision(9) << ", ";
    }
    std::cout << std::endl;

    std::cout << "WANTime : ";
    for (size_t i = 0; i < 11; ++i)
    {
        std::cout << std::fixed << measures[i].WANTime << std::setprecision(9) << ", ";
    }
    std::cout << std::endl;
}

template <typename Int> FLIOPMeasurement TwoPC<Int>::FLIOPWithRandomOracle(const size_t inputLength, const size_t compressFactor)
{
    Int::SetSeed(10u);

    std::vector<Int> op0(inputLength);
    for (size_t i = 0; i < inputLength; ++i)
    {
        op0[i] = Int::GenerateRandom();
    }
    std::vector<Int> op1(inputLength);
    for (size_t i = 0; i < inputLength; ++i)
    {
        op1[i] = Int::GenerateRandom();
    }

    bool isValid = true;
    Int out = InnerProductCircuit<Int>::Forward(op0.data(), op1.data(), inputLength);

    size_t totalProofSize = 0u;
    size_t totalQueryComplexity = 0u;

    const size_t nTotalRounds = (size_t)ceil(std::log2((double)op0.size()));
    std::vector<InteractiveProof<Int>> interactiveProofs;
    interactiveProofs.reserve(nTotalRounds);
    std::vector<Int> randoms;
    randoms.reserve(nTotalRounds);

    // Prover
    auto start = std::chrono::high_resolution_clock::now();
    while (ceil(op0.size() / (double)compressFactor) > 1)
    {
        InteractiveProof<Int> proof =
            InnerProductCircuit<Int>::MakeRoundProof(op0.data(), op1.data(), op0.size(), compressFactor);
        Int random = proof.GetRandomFromOracle();
        op0 = proof.EvaluatePolyPs(random);
        op1 = proof.EvaluatePolyQs(random);
        interactiveProofs.emplace_back(proof);
        randoms.push_back(random);

        totalProofSize += proof.GetBytes();
    }
    Proof<Int> finalProof = InnerProductCircuit<Int>::MakeProof(op0.data(), op1.data(), op0.size(), op0.size());
    auto end = std::chrono::high_resolution_clock::now();
    double proverTime = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    totalProofSize += finalProof.GetBytes();

    assert(interactiveProofs.size() == randoms.size());

    // Communication
    double LANTime = Network::GetLANDelay(totalProofSize + nTotalRounds * sizeof(Int) + op0.size() * 2 * sizeof(Int));
    double WANTime = Network::GetWANDelay(totalProofSize + nTotalRounds * sizeof(Int) + op0.size() * 2 * sizeof(Int));

    // Verifier
    start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < interactiveProofs.size(); ++i)
    {
        isValid = isValid && (randoms[i] == interactiveProofs[i].GetRandomFromOracle());

        std::vector<Query<Int>> queries = InnerProductCircuit<Int>::MakeRoundQuery(randoms[i], compressFactor);
        isValid = isValid && (out == interactiveProofs[i].GetQueryAnswer(queries[0]));
        out = interactiveProofs[i].GetQueryAnswer(queries[1]);

        totalQueryComplexity += 2;
    }

    std::vector<Query<Int>> queries =
        InnerProductCircuit<Int>::MakeQuery(Int::GenerateRandom(), op0.size(), op0.size());
    const size_t nInputQueriesHalf = (queries.size() - 2u) / 2u;
    Int gR((uint64_t)0);
    for (size_t i = 0; i < nInputQueriesHalf; ++i)
    {
        gR += finalProof.GetQueryAnswer(queries[i]) * finalProof.GetQueryAnswer(queries[i + nInputQueriesHalf]);
    }
    isValid = isValid && (finalProof.GetQueryAnswer(queries[queries.size() - 2u]) == gR) &&
              (finalProof.GetQueryAnswer(queries[queries.size() - 1u]) == out);

    end = std::chrono::high_resolution_clock::now();
    double verifierTime = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

    totalQueryComplexity += queries.size();

    return FLIOPMeasurement(totalProofSize, totalQueryComplexity, proverTime * 1e-6, verifierTime * 1e-6,
                            LANTime * 1e-6, WANTime * 1e-6, isValid);
}

template <typename Int>
FLIOPMeasurement TwoPC<Int>::FLIOPWithRandomOracleAndPrecompute(const size_t inputLength, const size_t compressFactor)
{
    Int::SetSeed(10u);

    std::vector<Int> op0(inputLength);
    for (size_t i = 0; i < inputLength; ++i)
    {
        op0[i] = Int::GenerateRandom();
    }
    std::vector<Int> op1(inputLength);
    for (size_t i = 0; i < inputLength; ++i)
    {
        op1[i] = Int::GenerateRandom();
    }

    bool isValid = true;
    Int out = InnerProductCircuit<Int>::Forward(op0.data(), op1.data(), inputLength);

    size_t totalProofSize = 0u;
    size_t totalQueryComplexity = 0u;

    const size_t nTotalRounds = (size_t)ceil(std::log2((double)op0.size()));
    std::vector<InteractiveProof<Int>> interactiveProofs;
    interactiveProofs.reserve(nTotalRounds);
    std::vector<Int> randoms;
    randoms.reserve(nTotalRounds);

    // Precompute a Vandermonde matrix
    Int* xs = new Int[compressFactor];
    for (size_t i = 0; i < compressFactor; ++i)
    {
        xs[i] = Int(i);
    }
    SquareMatrix<Int> evalToCoeff = SquareMatrix<Int>::GetVandermonde(xs, compressFactor);
    evalToCoeff.Inverse();
    delete[] xs;
    xs = (Int*)0;

    // Prover
    auto start = std::chrono::high_resolution_clock::now();
    while (ceil(op0.size() / (double)compressFactor) > 1)
    {
        InteractiveProof<Int> proof =
            InnerProductCircuit<Int>::MakeRoundProofWithPrecompute(op0.data(), op1.data(), op0.size(), compressFactor, evalToCoeff);
        Int random = proof.GetRandomFromOracle();
        op0 = proof.EvaluatePolyPs(random);
        op1 = proof.EvaluatePolyQs(random);
        interactiveProofs.emplace_back(proof);
        randoms.push_back(random);

        totalProofSize += proof.GetBytes();
    }
    Proof<Int> finalProof = InnerProductCircuit<Int>::MakeProof(op0.data(), op1.data(), op0.size(), op0.size());
    auto end = std::chrono::high_resolution_clock::now();
    double proverTime = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    totalProofSize += finalProof.GetBytes();

    assert(interactiveProofs.size() == randoms.size());

    // Communication
    double LANTime = Network::GetLANDelay(totalProofSize + nTotalRounds * sizeof(Int) + op0.size() * 2 * sizeof(Int));
    double WANTime = Network::GetWANDelay(totalProofSize + nTotalRounds * sizeof(Int) + op0.size() * 2 * sizeof(Int));

    // Verifier
    start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < interactiveProofs.size(); ++i)
    {
        isValid = isValid && (randoms[i] == interactiveProofs[i].GetRandomFromOracle());

        std::vector<Query<Int>> queries = InnerProductCircuit<Int>::MakeRoundQuery(randoms[i], compressFactor);
        isValid = isValid && (out == interactiveProofs[i].GetQueryAnswer(queries[0]));
        out = interactiveProofs[i].GetQueryAnswer(queries[1]);

        totalQueryComplexity += 2;
    }

    std::vector<Query<Int>> queries =
        InnerProductCircuit<Int>::MakeQuery(Int::GenerateRandom(), op0.size(), op0.size());
    const size_t nInputQueriesHalf = (queries.size() - 2u) / 2u;
    Int gR((uint64_t)0);
    for (size_t i = 0; i < nInputQueriesHalf; ++i)
    {
        gR += finalProof.GetQueryAnswer(queries[i]) * finalProof.GetQueryAnswer(queries[i + nInputQueriesHalf]);
    }
    isValid = isValid && (finalProof.GetQueryAnswer(queries[queries.size() - 2u]) == gR) &&
              (finalProof.GetQueryAnswer(queries[queries.size() - 1u]) == out);

    end = std::chrono::high_resolution_clock::now();
    double verifierTime = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

    totalQueryComplexity += queries.size();

    return FLIOPMeasurement(totalProofSize, totalQueryComplexity, proverTime * 1e-6, verifierTime * 1e-6,
                            LANTime * 1e-6, WANTime * 1e-6, isValid);
}

template <typename Int> void TwoPC<Int>::ExperimentFLIOPWithRandomOracle()
{
    size_t j = 0;
    FLIOPMeasurement measures[11];
    for (size_t i = 4; i <= 4096; i *= 2)
    {
        measures[j] = TwoPC<Int>::FLIOPWithRandomOracle(i, 2);
        if (!measures[j++].isVaild)
        {
            std::cout << "Invalid!" << std::endl;
            return;
        }
    }

    std::cout << "Vector Length : ";
    for (size_t i = 4; i <= 4096; i *= 2)
    {
        std::cout << i << ", ";
    }
    std::cout << std::endl;

    std::cout << "Proof Length : ";
    for (size_t i = 0; i < 11; ++i)
    {
        std::cout << measures[i].proofLength << ", ";
    }
    std::cout << std::endl;

    std::cout << "Query Complexity : ";
    for (size_t i = 0; i < 11; ++i)
    {
        std::cout << measures[i].nQueries << ", ";
    }
    std::cout << std::endl;

    std::cout << "Prover Time : ";
    for (size_t i = 0; i < 11; ++i)
    {
        std::cout << std::fixed << measures[i].proverTime << std::setprecision(9) << ", ";
    }
    std::cout << std::endl;

    std::cout << "Verifier Time : ";
    for (size_t i = 0; i < 11; ++i)
    {
        std::cout << std::fixed << measures[i].verifierTime << std::setprecision(9) << ", ";
    }
    std::cout << std::endl;

    std::cout << "LAN Time : ";
    for (size_t i = 0; i < 11; ++i)
    {
        std::cout << std::fixed << measures[i].LANTime << std::setprecision(9) << ", ";
    }
    std::cout << std::endl;

    std::cout << "WANTime : ";
    for (size_t i = 0; i < 11; ++i)
    {
        std::cout << std::fixed << measures[i].WANTime << std::setprecision(9) << ", ";
    }
    std::cout << std::endl;
}

template <typename Int> void TwoPC<Int>::ExperimentFLIOPWithRandomOracleAndPrecompute()
{
    size_t j = 0;
    FLIOPMeasurement measures[11];
    for (size_t i = 4; i <= 4096; i *= 2)
    {
        measures[j] = TwoPC<Int>::FLIOPWithRandomOracleAndPrecompute(i, 2);
        if (!measures[j++].isVaild)
        {
            std::cout << "Invalid!" << std::endl;
            return;
        }
    }

    std::cout << "Vector Length : ";
    for (size_t i = 4; i <= 4096; i *= 2)
    {
        std::cout << i << ", ";
    }
    std::cout << std::endl;

    std::cout << "Proof Length : ";
    for (size_t i = 0; i < 11; ++i)
    {
        std::cout << measures[i].proofLength << ", ";
    }
    std::cout << std::endl;

    std::cout << "Query Complexity : ";
    for (size_t i = 0; i < 11; ++i)
    {
        std::cout << measures[i].nQueries << ", ";
    }
    std::cout << std::endl;

    std::cout << "Prover Time : ";
    for (size_t i = 0; i < 11; ++i)
    {
        std::cout << std::fixed << measures[i].proverTime << std::setprecision(9) << ", ";
    }
    std::cout << std::endl;

    std::cout << "Verifier Time : ";
    for (size_t i = 0; i < 11; ++i)
    {
        std::cout << std::fixed << measures[i].verifierTime << std::setprecision(9) << ", ";
    }
    std::cout << std::endl;

    std::cout << "LAN Time : ";
    for (size_t i = 0; i < 11; ++i)
    {
        std::cout << std::fixed << measures[i].LANTime << std::setprecision(9) << ", ";
    }
    std::cout << std::endl;

    std::cout << "WANTime : ";
    for (size_t i = 0; i < 11; ++i)
    {
        std::cout << std::fixed << measures[i].WANTime << std::setprecision(9) << ", ";
    }
    std::cout << std::endl;
}

template <typename Int>
FLIOPMeasurement TwoPC<Int>::FLIOPCoefficient(const size_t inputLength, const size_t compressFactor)
{
    Int::SetSeed(10u);

    std::vector<Int> op0(inputLength);
    for (size_t i = 0; i < inputLength; ++i)
    {
        op0[i] = Int::GenerateRandom();
    }
    std::vector<Int> op1(inputLength);
    for (size_t i = 0; i < inputLength; ++i)
    {
        op1[i] = Int::GenerateRandom();
    }

    double proverTime = 0.;
    double verifierTime = 0.;
    double LANTime = 0.;
    double WANTime = 0.;
    size_t totalProofSize = 0u;
    size_t totalQueryComplexity = 0u;

    bool isValid = true;
    Int out = InnerProductCircuit<Int>::Forward(op0.data(), op1.data(), inputLength);
    while (ceil(op0.size() / (double)compressFactor) > 1)
    {
        // Prover
        auto start = std::chrono::high_resolution_clock::now();
        InteractiveProof<Int> proof =
            InnerProductCircuit<Int>::MakeRoundCoefficientProof(op0.data(), op1.data(), op0.size(), compressFactor);
        auto end = std::chrono::high_resolution_clock::now();
        proverTime += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

        // Communication : Prover give the verifier a single round proof
        LANTime += Network::GetLANDelay(proof.GetBytes());
        WANTime += Network::GetWANDelay(proof.GetBytes());

        // Verifier
        start = std::chrono::high_resolution_clock::now();
        Int random = Int::GenerateRandom();
        std::vector<Query<Int>> queries = InnerProductCircuit<Int>::MakeRoundCoefficientQuery(random, compressFactor);
        isValid = isValid && (out == proof.GetQueryAnswer(queries[0]));
        out = proof.GetQueryAnswer(queries[1]);
        end = std::chrono::high_resolution_clock::now();
        verifierTime += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

        // Communication : Verifier give the prover a one round randomness
        LANTime += Network::GetLANDelay(sizeof(Int));
        WANTime += Network::GetWANDelay(sizeof(Int));

        // Prover
        start = std::chrono::high_resolution_clock::now();
        op0 = proof.EvaluatePolyPs(random);
        op1 = proof.EvaluatePolyQs(random);
        end = std::chrono::high_resolution_clock::now();
        proverTime += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

        totalProofSize += proof.GetBytes();
        totalQueryComplexity += 2;
    }

    // Prover
    auto start = std::chrono::high_resolution_clock::now();
    Proof<Int> proof = InnerProductCircuit<Int>::MakeCoefficientProof(op0.data(), op1.data(), op0.size(), 1);
    auto end = std::chrono::high_resolution_clock::now();
    proverTime += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

    // Communication : Prover give the verifier a final round proof
    LANTime += Network::GetLANDelay(op0.size() * 2 * sizeof(Int) + proof.GetBytes());
    WANTime += Network::GetWANDelay(op0.size() * 2 * sizeof(Int) + proof.GetBytes());

    // Verifier
    start = std::chrono::high_resolution_clock::now();
    std::vector<Query<Int>> queries =
        InnerProductCircuit<Int>::MakeCoefficientQuery(Int::GenerateRandom(), op0.size(), 1);
    isValid =
        isValid &&
        (proof.GetQueryAnswer(queries[0]) * proof.GetQueryAnswer(queries[1]) == proof.GetQueryAnswer(queries[2])) &&
        (proof.GetQueryAnswer(queries[3]) == out);
    end = std::chrono::high_resolution_clock::now();
    verifierTime += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

    totalProofSize += proof.GetBytes();
    totalQueryComplexity += 4;

    return FLIOPMeasurement(totalProofSize, totalQueryComplexity, proverTime * 1e-6, verifierTime * 1e-6,
                            LANTime * 1e-6, WANTime * 1e-6, isValid);
}

template <typename Int> void TwoPC<Int>::ExperimentFLIOPCoefficient()
{
    size_t j = 0;
    FLIOPMeasurement measures[11];
    for (size_t i = 4; i <= 4096; i *= 2)
    {
        measures[j] = TwoPC<Int>::FLIOPCoefficient(i, 2);
        if (!measures[j++].isVaild)
        {
            std::cout << "Invalid!" << std::endl;
            return;
        }
    }

    std::cout << "Vector Length : ";
    for (size_t i = 4; i <= 4096; i *= 2)
    {
        std::cout << i << ", ";
    }
    std::cout << std::endl;

    std::cout << "Proof Length : ";
    for (size_t i = 0; i < 11; ++i)
    {
        std::cout << measures[i].proofLength << ", ";
    }
    std::cout << std::endl;

    std::cout << "Query Complexity : ";
    for (size_t i = 0; i < 11; ++i)
    {
        std::cout << measures[i].nQueries << ", ";
    }
    std::cout << std::endl;

    std::cout << "Prover Time : ";
    for (size_t i = 0; i < 11; ++i)
    {
        std::cout << std::fixed << measures[i].proverTime << std::setprecision(9) << ", ";
    }
    std::cout << std::endl;

    std::cout << "Verifier Time : ";
    for (size_t i = 0; i < 11; ++i)
    {
        std::cout << std::fixed << measures[i].verifierTime << std::setprecision(9) << ", ";
    }
    std::cout << std::endl;

    std::cout << "LAN Time : ";
    for (size_t i = 0; i < 11; ++i)
    {
        std::cout << std::fixed << measures[i].LANTime << std::setprecision(9) << ", ";
    }
    std::cout << std::endl;

    std::cout << "WAN Time : ";
    for (size_t i = 0; i < 11; ++i)
    {
        std::cout << std::fixed << measures[i].WANTime << std::setprecision(9) << ", ";
    }
    std::cout << std::endl;
}

template <typename Int>
FLIOPMeasurement TwoPC<Int>::FLIOPCoefficientWithRandomOracle(const size_t inputLength, const size_t compressFactor)
{
    Int::SetSeed(10u);

    std::vector<Int> op0(inputLength);
    for (size_t i = 0; i < inputLength; ++i)
    {
        op0[i] = Int::GenerateRandom();
    }
    std::vector<Int> op1(inputLength);
    for (size_t i = 0; i < inputLength; ++i)
    {
        op1[i] = Int::GenerateRandom();
    }

    bool isValid = true;
    Int out = InnerProductCircuit<Int>::Forward(op0.data(), op1.data(), inputLength);

    size_t totalProofSize = 0u;
    size_t totalQueryComplexity = 0u;

    const size_t nTotalRounds = (size_t)ceil(std::log2((double)op0.size()));
    std::vector<InteractiveProof<Int>> interactiveProofs;
    interactiveProofs.reserve(nTotalRounds);
    std::vector<Int> randoms;
    randoms.reserve(nTotalRounds);

    // Prover
    auto start = std::chrono::high_resolution_clock::now();
    while (ceil(op0.size() / (double)compressFactor) > 1)
    {
        InteractiveProof<Int> proof =
            InnerProductCircuit<Int>::MakeRoundCoefficientProof(op0.data(), op1.data(), op0.size(), compressFactor);
        Int random = proof.GetRandomFromOracle();
        op0 = proof.EvaluatePolyPs(random);
        op1 = proof.EvaluatePolyQs(random);
        interactiveProofs.emplace_back(proof);
        randoms.push_back(random);

        totalProofSize += proof.GetBytes();
    }
    Proof<Int> finalProof = InnerProductCircuit<Int>::MakeCoefficientProof(op0.data(), op1.data(), op0.size(), 1);
    auto end = std::chrono::high_resolution_clock::now();
    double proverTime = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    totalProofSize += finalProof.GetBytes();

    assert(interactiveProofs.size() == randoms.size());

    // Communication
    double LANTime = Network::GetLANDelay(totalProofSize + nTotalRounds * sizeof(Int) + op0.size() * 2 * sizeof(Int));
    double WANTime = Network::GetWANDelay(totalProofSize + nTotalRounds * sizeof(Int) + op0.size() * 2 * sizeof(Int));

    // Verifier
    start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < interactiveProofs.size(); ++i)
    {
        isValid = isValid && (randoms[i] == interactiveProofs[i].GetRandomFromOracle());

        std::vector<Query<Int>> queries =
            InnerProductCircuit<Int>::MakeRoundCoefficientQuery(randoms[i], compressFactor);
        isValid = isValid && (out == interactiveProofs[i].GetQueryAnswer(queries[0]));
        out = interactiveProofs[i].GetQueryAnswer(queries[1]);

        totalQueryComplexity += 2;
    }

    std::vector<Query<Int>> queries =
        InnerProductCircuit<Int>::MakeCoefficientQuery(Int::GenerateRandom(), op0.size(), 1);
    isValid = isValid &&
              (finalProof.GetQueryAnswer(queries[0]) * finalProof.GetQueryAnswer(queries[1]) ==
               finalProof.GetQueryAnswer(queries[2])) &&
              (finalProof.GetQueryAnswer(queries[3]) == out);

    end = std::chrono::high_resolution_clock::now();
    double verifierTime = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

    totalQueryComplexity += 4;

    return FLIOPMeasurement(totalProofSize, totalQueryComplexity, proverTime * 1e-6, verifierTime * 1e-6,
                            LANTime * 1e-6, WANTime * 1e-6, isValid);
}

template <typename Int> void TwoPC<Int>::ExperimentFLIOPCoefficientWithRandomOracle()
{
    size_t j = 0;
    FLIOPMeasurement measures[11];
    for (size_t i = 4; i <= 4096; i *= 2)
    {
        measures[j] = TwoPC<Int>::FLIOPCoefficientWithRandomOracle(i, 2);
        if (!measures[j++].isVaild)
        {
            std::cout << "Invalid!" << std::endl;
            return;
        }
    }

    std::cout << "Vector Length : ";
    for (size_t i = 4; i <= 4096; i *= 2)
    {
        std::cout << i << ", ";
    }
    std::cout << std::endl;

    std::cout << "Proof Length : ";
    for (size_t i = 0; i < 11; ++i)
    {
        std::cout << measures[i].proofLength << ", ";
    }
    std::cout << std::endl;

    std::cout << "Query Complexity : ";
    for (size_t i = 0; i < 11; ++i)
    {
        std::cout << measures[i].nQueries << ", ";
    }
    std::cout << std::endl;

    std::cout << "Prover Time : ";
    for (size_t i = 0; i < 11; ++i)
    {
        std::cout << std::fixed << measures[i].proverTime << std::setprecision(9) << ", ";
    }
    std::cout << std::endl;

    std::cout << "Verifier Time : ";
    for (size_t i = 0; i < 11; ++i)
    {
        std::cout << std::fixed << measures[i].verifierTime << std::setprecision(9) << ", ";
    }
    std::cout << std::endl;

    std::cout << "LAN Time : ";
    for (size_t i = 0; i < 11; ++i)
    {
        std::cout << std::fixed << measures[i].LANTime << std::setprecision(9) << ", ";
    }
    std::cout << std::endl;

    std::cout << "WAN Time : ";
    for (size_t i = 0; i < 11; ++i)
    {
        std::cout << std::fixed << measures[i].WANTime << std::setprecision(9) << ", ";
    }
    std::cout << std::endl;
}

#endif
