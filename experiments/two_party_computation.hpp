#ifndef TWO_PARTY_COMPUTATION_H
#define TWO_PARTY_COMPUTATION_H

#include <chrono>
#include <iostream>

#include "network.hpp"
#include "../circuit/inner_product_circuit.hpp"
#include "../math/square_matrix.hpp"
#include "../unit/proof.hpp"

template <typename Int> class TwoPC
{
public:
    // Fully Linear PCP
    static FLPCPMeasurement FLPCP(const uint32_t seed, const size_t inputLength, const size_t nGGate);
    static FLPCPMeasurement FLPCPWithPrecompute(const uint32_t seed, const size_t inputLength, const size_t nGGate);
    static FLPCPMeasurement FLPCPCoefficient(const uint32_t seed, const size_t inputLength, const size_t nGGate);
    static void ExperimentFLPCP(size_t nCases, size_t nExperiments);

    // Fully Linear IOP
    static FLIOPMeasurement FLIOP(const size_t seed, const size_t inputLength, const size_t compressFactor);
    static FLIOPMeasurement FLIOPWithPrecompute(const size_t seed, const size_t inputLength,
                                                const size_t compressFactor);
    static FLIOPMeasurement FLIOPCoefficient(const size_t seed, const size_t inputLength, const size_t compressFactor);
    static void ExperimentFLIOP(size_t nCases, size_t nExperiments);
};

template <typename Int>
FLPCPMeasurement TwoPC<Int>::FLPCP(const uint32_t seed, const size_t inputLength, const size_t nGGate)
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
    auto start = std::chrono::high_resolution_clock::now();
    Proof<Int> proof = InnerProductCircuit<Int>::MakeProof(op0, op1, inputLength, nGGate);
    auto end = std::chrono::high_resolution_clock::now();
    double proverTime = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

    // Verifier make queries and perform inner products between proof and queries.
    // Assumption : Verifier only has linear access on proof vector.
    start = std::chrono::high_resolution_clock::now();
    std::vector<Query<Int>> queries =
        InnerProductCircuit<Int>::MakeQuery(Int::GenerateRandomAbove(nGGate + 1), nGGate, inputLength);

    const size_t nInputQueriesHalf = (queries.size() - 2u) / 2u;
    Int gR((uint32_t)0);
    for (size_t i = 0; i < nInputQueriesHalf; ++i)
    {
        gR += proof.GetQueryAnswer(queries[i]) * proof.GetQueryAnswer(queries[i + nInputQueriesHalf]);
    }

    bool isValid = (proof.GetQueryAnswer(queries[queries.size() - 2u]) == gR) &&
                   (proof.GetQueryAnswer(queries[queries.size() - 1u]) == circuitOutput);

    end = std::chrono::high_resolution_clock::now();
    double verifierTime = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

    delete[] op0;
    delete[] op1;

    return FLPCPMeasurement(proof.GetBytes(), queries.size(), proverTime * 1e-6, verifierTime * 1e-6, isValid);
}

template <typename Int>
FLPCPMeasurement TwoPC<Int>::FLPCPWithPrecompute(const uint32_t seed, const size_t inputLength, const size_t nGGate)
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
    SquareMatrix<Int> vandermondeInv = SquareMatrix<Int>::GetVandermondeInverse(nGGate + 1);

    // Precompute a process to make linear queries
    std::vector<Query<Int>> queries =
        InnerProductCircuit<Int>::MakeQuery(Int::GenerateRandomAbove(nGGate + 1), nGGate, inputLength);
    const size_t nInputQueriesHalf = (queries.size() - 2u) / 2u;

    // Prover make proof vector : (inputs || constant terms || coefficients)
    auto start = std::chrono::high_resolution_clock::now();
    Proof<Int> proof = InnerProductCircuit<Int>::MakeProofWithPrecompute(op0, op1, inputLength, nGGate, vandermondeInv);
    auto end = std::chrono::high_resolution_clock::now();
    double proverTime = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

    // Verifier make queries and perform inner products between proof and queries.
    // Assumption : Verifier only has linear access on proof vector.
    start = std::chrono::high_resolution_clock::now();
    Int gR((uint32_t)0);
    for (size_t i = 0; i < nInputQueriesHalf; ++i)
    {
        gR += proof.GetQueryAnswer(queries[i]) * proof.GetQueryAnswer(queries[i + nInputQueriesHalf]);
    }

    bool isValid = (proof.GetQueryAnswer(queries[queries.size() - 2u]) == gR) &&
                   (proof.GetQueryAnswer(queries[queries.size() - 1u]) == circuitOutput);

    end = std::chrono::high_resolution_clock::now();
    double verifierTime = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

    delete[] op0;
    delete[] op1;

    return FLPCPMeasurement(proof.GetBytes(), queries.size(), proverTime * 1e-6, verifierTime * 1e-6, isValid);
}

// Fully Linear PCP using input as coefficient of polynomials
template <typename Int>
FLPCPMeasurement TwoPC<Int>::FLPCPCoefficient(const uint32_t seed, const size_t inputLength, const size_t nPoly)
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

    auto start = std::chrono::high_resolution_clock::now();
    Proof<Int> proof = InnerProductCircuit<Int>::MakeCoefficientProof(op0, op1, inputLength, nPoly);
    auto end = std::chrono::high_resolution_clock::now();
    double proverTime = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

    start = std::chrono::high_resolution_clock::now();
    std::vector<Query<Int>> queries =
        InnerProductCircuit<Int>::MakeCoefficientQuery(Int::GenerateRandom(), inputLength, nPoly);
    const size_t nInputQueriesHalf = (queries.size() - 2u) / 2u;

    Int gR((uint32_t)0);
    for (size_t i = 0; i < nInputQueriesHalf; ++i)
    {
        gR += proof.GetQueryAnswer(queries[i]) * proof.GetQueryAnswer(queries[i + nInputQueriesHalf]);
    }
    bool isValid = (proof.GetQueryAnswer(queries[queries.size() - 2u]) == gR) &&
                   (proof.GetQueryAnswer(queries[queries.size() - 1u]) == circuitOutput);
    end = std::chrono::high_resolution_clock::now();
    double verifierTime = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

    delete[] op0;
    delete[] op1;

    return FLPCPMeasurement(proof.GetBytes(), queries.size(), proverTime * 1e-6, verifierTime * 1e-6, isValid);
}

template <typename Int> void TwoPC<Int>::ExperimentFLPCP(size_t nCases, size_t nExperiments)
{
    constexpr uint32_t seed = 23571113;
    const size_t last = pow(2, nCases);

    std::cout << "Simulating FLPCP (baseline, precomputed, coefficient) " << nExperiments
              << " times for input vector lengths from 2 to " << last << "." << std::endl;

    FLPCPMeasurement* baseline = new FLPCPMeasurement[nCases];
    FLPCPMeasurement* precomputedBaseline = new FLPCPMeasurement[nCases];
    FLPCPMeasurement* coefficientVersion = new FLPCPMeasurement[nCases];

    std::cout << "Starting warm up..." << std::endl;

    // Warm up
    TwoPC<Int>::FLPCP(seed, 1024, 1024);
    TwoPC<Int>::FLPCPWithPrecompute(seed, 1024, 1024);
    TwoPC<Int>::FLPCPCoefficient(seed, 1024, 1);

    std::cout << "Finished!" << std::endl;
    std::cout << std::endl;
    std::cout << "Start the simulations!" << std::endl;

    for (size_t i = 1; i <= nCases; ++i)
    {
        size_t vectorLength = pow(2, i);

        baseline[i - 1] = TwoPC<Int>::FLPCP(seed, vectorLength, vectorLength);
        precomputedBaseline[i - 1] = TwoPC<Int>::FLPCPWithPrecompute(seed, vectorLength, vectorLength);
        coefficientVersion[i - 1] = TwoPC<Int>::FLPCPCoefficient(seed, vectorLength, 1);

        for (size_t j = 0; j < nExperiments - 1; ++j)
        {
            baseline[i - 1] += TwoPC<Int>::FLPCP(seed, vectorLength, vectorLength);
            precomputedBaseline[i - 1] += TwoPC<Int>::FLPCPWithPrecompute(seed, vectorLength, vectorLength);
            coefficientVersion[i - 1] += TwoPC<Int>::FLPCPCoefficient(seed, vectorLength, 1);
        }

        baseline[i - 1] /= nExperiments;
        precomputedBaseline[i - 1] /= nExperiments;
        coefficientVersion[i - 1] /= nExperiments;

        if (!baseline[i - 1].isVaild)
        {
            std::cout << "Invalid in FLPCP baseline (" << vectorLength << ")" << std::endl;
            return;
        }
        if (!precomputedBaseline[i - 1].isVaild)
        {
            std::cout << "Invalid in FLPCP precomputed (" << vectorLength << ")" << std::endl;
            return;
        }
        if (!coefficientVersion[i - 1].isVaild)
        {
            std::cout << "Invalid in FLPCP coefficient (" << vectorLength << ")" << std::endl;
            return;
        }

        std::cout << "Finished input length (" << vectorLength << ")" << std::endl;
    }

    std::cout << std::endl;
    std::cout << "[Simulation Results]" << std::endl;
    std::cout << "* Common info" << std::endl;
    std::cout << "Vector Length : ";
    for (size_t i = 1; i <= nCases; ++i)
    {
        std::cout << (size_t)pow(2, i) << ", ";
    }
    std::cout << std::endl;

    std::cout << "# of G-gates : ";
    for (size_t i = 1; i <= nCases; ++i)
    {
        std::cout << (size_t)pow(2, i) << ", ";
    }
    std::cout << std::endl;

    std::cout << "Proof Length : ";
    for (size_t i = 0; i < nCases; ++i)
    {
        std::cout << baseline[i].proofLength << ", ";
    }
    std::cout << std::endl;

    std::cout << "Query Complexity : ";
    for (size_t i = 0; i < nCases; ++i)
    {
        std::cout << baseline[i].nQueries << ", ";
    }
    std::cout << std::endl;
    std::cout << std::endl;

    std::cout << "* Prover Time" << std::endl;
    std::cout << "Baseline : ";
    for (size_t i = 0; i < nCases; ++i)
    {
        std::cout << std::fixed << baseline[i].proverTime << std::setprecision(4) << ", ";
    }
    std::cout << std::endl;

    std::cout << "Precomputation : ";
    for (size_t i = 0; i < nCases; ++i)
    {
        std::cout << std::fixed << precomputedBaseline[i].proverTime << std::setprecision(4) << ", ";
    }
    std::cout << std::endl;

    std::cout << "Coefficient : ";
    for (size_t i = 0; i < nCases; ++i)
    {
        std::cout << std::fixed << coefficientVersion[i].proverTime << std::setprecision(4) << ", ";
    }
    std::cout << std::endl;
    std::cout << std::endl;

    std::cout << "* Verifier Time" << std::endl;
    std::cout << "Baseline : ";
    for (size_t i = 0; i < nCases; ++i)
    {
        std::cout << std::fixed << baseline[i].verifierTime << std::setprecision(4) << ", ";
    }
    std::cout << std::endl;

    std::cout << "Precomputation : ";
    for (size_t i = 0; i < nCases; ++i)
    {
        std::cout << std::fixed << precomputedBaseline[i].verifierTime << std::setprecision(4) << ", ";
    }
    std::cout << std::endl;

    std::cout << "Coefficient : ";
    for (size_t i = 0; i < nCases; ++i)
    {
        std::cout << std::fixed << coefficientVersion[i].verifierTime << std::setprecision(4) << ", ";
    }
    std::cout << std::endl;
    std::cout << "------------------------------------------------------" << std::endl;

    delete[] baseline;
    delete[] precomputedBaseline;
    delete[] coefficientVersion;
}

template <typename Int>
FLIOPMeasurement TwoPC<Int>::FLIOP(const size_t seed, const size_t inputLength, const size_t compressFactor)
{
    Int::SetSeed(seed);

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

    std::vector<Int> verOp0 = op0;
    std::vector<Int> verOp1 = op1;

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

        // Compressing
        const size_t nPoly0 = ceil(verOp0.size() / (double)compressFactor);
        Int* const resizedInput0 = new Int[nPoly0 * compressFactor];
        std::memset(resizedInput0, 0, (nPoly0 * compressFactor) * sizeof(Int));
        std::memcpy(resizedInput0, verOp0.data(), verOp0.size() * sizeof(Int));
        std::vector<Polynomial<Int>> poly0s;
        poly0s.reserve(nPoly0);
        for (size_t j = 0; j < nPoly0; ++j)
        {
            poly0s.emplace_back(
                Polynomial<Int>::LagrangeInterpolation(resizedInput0 + compressFactor * j, compressFactor));
        }
        delete[] resizedInput0;

        std::vector<Int> newOp0;
        newOp0.reserve(nPoly0);
        for (size_t j = 0; j < nPoly0; ++j)
        {
            newOp0.push_back(poly0s[j].Evaluate(randoms[i]));
        }
        verOp0 = newOp0;

        const size_t nPoly1 = ceil(verOp1.size() / (double)compressFactor);
        Int* const resizedInput1 = new Int[nPoly1 * compressFactor];
        std::memset(resizedInput1, 0, (nPoly1 * compressFactor) * sizeof(Int));
        std::memcpy(resizedInput1, verOp1.data(), verOp1.size() * sizeof(Int));
        std::vector<Polynomial<Int>> poly1s;
        poly1s.reserve(nPoly1);
        for (size_t j = 0; j < nPoly1; ++j)
        {
            poly1s.emplace_back(
                Polynomial<Int>::LagrangeInterpolation(resizedInput1 + compressFactor * j, compressFactor));
        }
        delete[] resizedInput1;

        std::vector<Int> newOp1;
        newOp1.reserve(nPoly1);
        for (size_t j = 0; j < nPoly1; ++j)
        {
            newOp1.push_back(poly1s[j].Evaluate(randoms[i]));
        }
        verOp1 = newOp1;

        totalQueryComplexity += 2;
    }

    Int finalVerifierRandom = Int::GenerateRandom();
    std::vector<Query<Int>> queries = InnerProductCircuit<Int>::MakeQuery(finalVerifierRandom, op0.size(), op0.size());
    
    std::vector<Int> finalProverRandoms = finalProof.GetRandoms(2);
    verOp0.insert(verOp0.begin(), finalProverRandoms[0]);
    verOp1.insert(verOp1.begin(), finalProverRandoms[1]);
    Polynomial<Int> finalOp0 = Polynomial<Int>::LagrangeInterpolation(verOp0.data(), verOp0.size());
    Polynomial<Int> finalOp1 = Polynomial<Int>::LagrangeInterpolation(verOp1.data(), verOp1.size());
    Int gR = finalOp0.Evaluate(finalVerifierRandom) * finalOp1.Evaluate(finalVerifierRandom);
    isValid = isValid && (finalProof.GetQueryAnswer(queries[queries.size() - 2]) == gR) &&
              (finalProof.GetQueryAnswer(queries[queries.size() - 1]) == out);

    end = std::chrono::high_resolution_clock::now();
    double verifierTime = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

    totalQueryComplexity += 2;

    return FLIOPMeasurement(totalProofSize, totalQueryComplexity, proverTime * 1e-6, verifierTime * 1e-6,
                            LANTime * 1e-6, WANTime * 1e-6, isValid);
}

template <typename Int>
FLIOPMeasurement TwoPC<Int>::FLIOPWithPrecompute(const size_t seed, const size_t inputLength,
                                                 const size_t compressFactor)
{
    Int::SetSeed(seed);

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

    std::vector<Int> verOp0 = op0;
    std::vector<Int> verOp1 = op1;

    bool isValid = true;
    Int out = InnerProductCircuit<Int>::Forward(op0.data(), op1.data(), inputLength);

    size_t totalProofSize = 0u;
    size_t totalQueryComplexity = 0u;

    const size_t nTotalRounds = (size_t)ceil(std::log2((double)op0.size()));
    std::vector<InteractiveProof<Int>> interactiveProofs;
    interactiveProofs.reserve(nTotalRounds);
    std::vector<Int> randoms;
    randoms.reserve(nTotalRounds);

    SquareMatrix<Int> IntermediateVanInv = SquareMatrix<Int>::GetVandermondeInverse(compressFactor);

    size_t finalRoundLength = op0.size();
    while (finalRoundLength > compressFactor)
    {
        finalRoundLength = ceil(finalRoundLength / (double)compressFactor);
    }
    SquareMatrix<Int> finalVanInv = SquareMatrix<Int>::GetVandermondeInverse(finalRoundLength + 1);

    // Prover
    auto start = std::chrono::high_resolution_clock::now();
    while (ceil(op0.size() / (double)compressFactor) > 1)
    {
        InteractiveProof<Int> proof =
            InnerProductCircuit<Int>::MakeRoundProofWithPrecompute(op0.data(), op1.data(), op0.size(), compressFactor, IntermediateVanInv);
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

        // Compressing
        const size_t nPoly0 = ceil(verOp0.size() / (double)compressFactor);
        Int* const resizedInput0 = new Int[nPoly0 * compressFactor];
        std::memset(resizedInput0, 0, (nPoly0 * compressFactor) * sizeof(Int));
        std::memcpy(resizedInput0, verOp0.data(), verOp0.size() * sizeof(Int));
        std::vector<Polynomial<Int>> poly0s;
        poly0s.reserve(nPoly0);
        for (size_t j = 0; j < nPoly0; ++j)
        {
            poly0s.emplace_back(
                Polynomial<Int>::VandermondeInterpolation(resizedInput0 + compressFactor * j, compressFactor, IntermediateVanInv));
        }
        delete[] resizedInput0;

        std::vector<Int> newOp0;
        newOp0.reserve(nPoly0);
        for (size_t j = 0; j < nPoly0; ++j)
        {
            newOp0.push_back(poly0s[j].Evaluate(randoms[i]));
        }
        verOp0 = newOp0;

        const size_t nPoly1 = ceil(verOp1.size() / (double)compressFactor);
        Int* const resizedInput1 = new Int[nPoly1 * compressFactor];
        std::memset(resizedInput1, 0, (nPoly1 * compressFactor) * sizeof(Int));
        std::memcpy(resizedInput1, verOp1.data(), verOp1.size() * sizeof(Int));
        std::vector<Polynomial<Int>> poly1s;
        poly1s.reserve(nPoly1);
        for (size_t j = 0; j < nPoly1; ++j)
        {
            poly1s.emplace_back(Polynomial<Int>::VandermondeInterpolation(resizedInput1 + compressFactor * j,
                                                                          compressFactor, IntermediateVanInv));
        }
        delete[] resizedInput1;

        std::vector<Int> newOp1;
        newOp1.reserve(nPoly1);
        for (size_t j = 0; j < nPoly1; ++j)
        {
            newOp1.push_back(poly1s[j].Evaluate(randoms[i]));
        }
        verOp1 = newOp1;

        totalQueryComplexity += 2;
    }

    Int finalVerifierRandom = Int::GenerateRandom();
    std::vector<Query<Int>> queries = InnerProductCircuit<Int>::MakeQuery(finalVerifierRandom, op0.size(), op0.size());

    std::vector<Int> finalProverRandoms = finalProof.GetRandoms(2);
    verOp0.insert(verOp0.begin(), finalProverRandoms[0]);
    verOp1.insert(verOp1.begin(), finalProverRandoms[1]);
    Polynomial<Int> finalOp0 = Polynomial<Int>::VandermondeInterpolation(verOp0.data(), verOp0.size(), finalVanInv);
    Polynomial<Int> finalOp1 = Polynomial<Int>::VandermondeInterpolation(verOp1.data(), verOp1.size(), finalVanInv);
    Int gR = finalOp0.Evaluate(finalVerifierRandom) * finalOp1.Evaluate(finalVerifierRandom);
    isValid = isValid && (finalProof.GetQueryAnswer(queries[queries.size() - 2]) == gR) &&
              (finalProof.GetQueryAnswer(queries[queries.size() - 1]) == out);

    end = std::chrono::high_resolution_clock::now();
    double verifierTime = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

    totalQueryComplexity += 2;

    return FLIOPMeasurement(totalProofSize, totalQueryComplexity, proverTime * 1e-6, verifierTime * 1e-6,
                            LANTime * 1e-6, WANTime * 1e-6, isValid);
}

template <typename Int>
FLIOPMeasurement TwoPC<Int>::FLIOPCoefficient(const size_t seed, const size_t inputLength, const size_t compressFactor)
{
    Int::SetSeed(seed);

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

    std::vector<Int> verOp0 = op0;
    std::vector<Int> verOp1 = op1;

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

        // Compressing
        const size_t nPoly0 = ceil(verOp0.size() / (double)compressFactor);
        Int* const resizedInput0 = new Int[nPoly0 * compressFactor];
        std::memset(resizedInput0, 0, (nPoly0 * compressFactor) * sizeof(Int));
        std::memcpy(resizedInput0, verOp0.data(), verOp0.size() * sizeof(Int));
        std::vector<Polynomial<Int>> poly0s;
        poly0s.reserve(nPoly0);
        for (size_t j = 0; j < nPoly0; ++j)
        {
            poly0s.emplace_back(resizedInput0 + compressFactor * j, compressFactor, true);
        }
        delete[] resizedInput0;

        std::vector<Int> newOp0;
        newOp0.reserve(nPoly0);
        for (size_t j = 0; j < nPoly0; ++j)
        {
            newOp0.push_back(poly0s[j].Evaluate(randoms[i]));
        }
        verOp0 = newOp0;

        const size_t nPoly1 = ceil(verOp1.size() / (double)compressFactor);
        Int* const resizedInput1 = new Int[nPoly1 * compressFactor];
        std::memset(resizedInput1, 0, (nPoly1 * compressFactor) * sizeof(Int));
        std::memcpy(resizedInput1, verOp1.data(), verOp1.size() * sizeof(Int));
        std::vector<Polynomial<Int>> poly1s;
        poly1s.reserve(nPoly1);
        for (size_t j = 0; j < nPoly1; ++j)
        {
            Int::Reverse(resizedInput1 + compressFactor * j, resizedInput1 + compressFactor * j + compressFactor - 1);
            poly1s.emplace_back(resizedInput1 + compressFactor * j, compressFactor, true);
        }
        delete[] resizedInput1;

        std::vector<Int> newOp1;
        newOp1.reserve(nPoly1);
        for (size_t j = 0; j < nPoly1; ++j)
        {
            newOp1.push_back(poly1s[j].Evaluate(randoms[i]));
        }
        verOp1 = newOp1;

        totalQueryComplexity += 2;
    }

    Int finalVerifierRandom = Int::GenerateRandom();
    std::vector<Query<Int>> queries =
        InnerProductCircuit<Int>::MakeCoefficientQuery(finalVerifierRandom, op0.size(), 1);

    std::vector<Int> finalProverRandoms = finalProof.GetRandoms(2);
    verOp0.insert(verOp0.begin(), finalProverRandoms[0]);
    verOp1.insert(verOp1.begin(), finalProverRandoms[1]);
    std::reverse(verOp1.begin() + 1, verOp1.end());
    Polynomial<Int> finalOp0(verOp0.data(), verOp0.size(), true);
    Polynomial<Int> finalOp1(verOp1.data(), verOp1.size(), true);
    Int gR = finalOp0.Evaluate(finalVerifierRandom) * finalOp1.Evaluate(finalVerifierRandom);
    isValid = isValid && (finalProof.GetQueryAnswer(queries[2]) == gR) &&
              (finalProof.GetQueryAnswer(queries[3]) == out);

    end = std::chrono::high_resolution_clock::now();
    double verifierTime = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

    totalQueryComplexity += 2;

    return FLIOPMeasurement(totalProofSize, totalQueryComplexity, proverTime * 1e-6, verifierTime * 1e-6,
                            LANTime * 1e-6, WANTime * 1e-6, isValid);
}

template <typename Int> void TwoPC<Int>::ExperimentFLIOP(size_t nCases, size_t nExperiments)
{
    constexpr uint32_t seed = 23571113;
    size_t inputLength = pow(2, nCases + 1);

    std::cout << "Simulating FLIOP (baseline, precomputed, coefficient) " << nExperiments
              << " times for input vector length " << inputLength << " and compression factor from 2 to "
              << (size_t)pow(2, nCases) << "." << std::endl;

    FLIOPMeasurement* baseline = new FLIOPMeasurement[nCases];
    FLIOPMeasurement* precomputation = new FLIOPMeasurement[nCases];
    FLIOPMeasurement* coefficientVersion = new FLIOPMeasurement[nCases];

    std::cout << "Starting warm up..." << std::endl;

    // Warm up
    TwoPC<Int>::FLIOP(seed, inputLength, 64);
    TwoPC<Int>::FLIOPWithPrecompute(seed, inputLength, 64);
    TwoPC<Int>::FLIOPCoefficient(seed, inputLength, 64);

    std::cout << "Finished!" << std::endl;
    std::cout << std::endl;
    std::cout << "Start the simulations!" << std::endl;

    for (size_t i = 1; i <= nCases; ++i)
    {
        size_t compressionFactor = pow(2, i);

        baseline[i - 1] = TwoPC<Int>::FLIOP(seed, inputLength, compressionFactor);
        precomputation[i - 1] = TwoPC<Int>::FLIOPWithPrecompute(seed, inputLength, compressionFactor);
        coefficientVersion[i - 1] = TwoPC<Int>::FLIOPCoefficient(seed, inputLength, compressionFactor);

        for (size_t j = 0; j < nExperiments - 1; ++j)
        {
            baseline[i - 1] += TwoPC<Int>::FLIOP(seed, inputLength, compressionFactor);
            precomputation[i - 1] += TwoPC<Int>::FLIOPWithPrecompute(seed, inputLength, compressionFactor);
            coefficientVersion[i - 1] += TwoPC<Int>::FLIOPCoefficient(seed, inputLength, compressionFactor);
        }

        baseline[i - 1] /= nExperiments;
        precomputation[i - 1] /= nExperiments;
        coefficientVersion[i - 1] /= nExperiments;

        if (!baseline[i - 1].isVaild)
        {
            std::cout << "Invalid in FLIOP baseline (" << compressionFactor << ")" << std::endl;
            return;
        }
        if (!precomputation[i - 1].isVaild)
        {
            std::cout << "Invalid in FLIOP precomputed (" << compressionFactor << ")" << std::endl;
            return;
        }
        if (!coefficientVersion[i - 1].isVaild)
        {
            std::cout << "Invalid in FLIOP coefficient (" << compressionFactor << ")" << std::endl;
            return;
        }

        std::cout << "Finished compression factor (" << compressionFactor << ")" << std::endl;
    }

    std::cout << std::endl;
    std::cout << "[Simulation Results]" << std::endl;
    std::cout << "* Common info" << std::endl;
    std::cout << "Vector Length : " << inputLength << std::endl;
    std::cout << "Compression Factor : ";
    for (size_t i = 1; i <= nCases; ++i)
    {
        std::cout << (size_t)pow(2, i) << ", ";
    }
    std::cout << std::endl;

    std::cout << "Proof Length : ";
    for (size_t i = 0; i < nCases; ++i)
    {
        std::cout << baseline[i].proofLength << ", ";
    }
    std::cout << std::endl;

    std::cout << "Query Complexity : ";
    for (size_t i = 0; i < nCases; ++i)
    {
        std::cout << baseline[i].nQueries << ", ";
    }
    std::cout << std::endl;
    std::cout << std::endl;
    
    std::cout << "* Prover Time" << std::endl;
    std::cout << "Baseline : ";
    for (size_t i = 0; i < nCases; ++i)
    {
        std::cout << std::fixed << baseline[i].proverTime << std::setprecision(4) << ", ";
    }
    std::cout << std::endl;

    std::cout << "Precomputation : ";
    for (size_t i = 0; i < nCases; ++i)
    {
        std::cout << std::fixed << precomputation[i].proverTime << std::setprecision(4) << ", ";
    }
    std::cout << std::endl;
    
    std::cout << "Coefficient : ";
    for (size_t i = 0; i < nCases; ++i)
    {
        std::cout << std::fixed << coefficientVersion[i].proverTime << std::setprecision(4) << ", ";
    }
    std::cout << std::endl;
    std::cout << std::endl;

    std::cout << "* Verifier Time" << std::endl;
    std::cout << "Baseline : ";
    for (size_t i = 0; i < nCases; ++i)
    {
        std::cout << std::fixed << baseline[i].verifierTime << std::setprecision(4) << ", ";
    }
    std::cout << std::endl;
    
    std::cout << "Precomputation : ";
    for (size_t i = 0; i < nCases; ++i)
    {
        std::cout << std::fixed << precomputation[i].verifierTime << std::setprecision(4) << ", ";
    }
    std::cout << std::endl;

    std::cout << "Coefficient : ";
    for (size_t i = 0; i < nCases; ++i)
    {
        std::cout << std::fixed << coefficientVersion[i].verifierTime << std::setprecision(4) << ", ";
    }
    std::cout << std::endl;
    std::cout << std::endl;

    std::cout << "* Communication Time" << std::endl;
    std::cout << "LAN Time : ";
    for (size_t i = 0; i < nCases; ++i)
    {
        std::cout << std::fixed << baseline[i].LANTime << std::setprecision(4) << ", ";
    }
    std::cout << std::endl;

    std::cout << "WAN Time : ";
    for (size_t i = 0; i < nCases; ++i)
    {
        std::cout << std::fixed << baseline[i].WANTime << std::setprecision(4) << ", ";
    }
    std::cout << std::endl;
    std::cout << "------------------------------------------------------" << std::endl;

    delete[] baseline;
    delete[] precomputation;
    delete[] coefficientVersion;
}

#endif
