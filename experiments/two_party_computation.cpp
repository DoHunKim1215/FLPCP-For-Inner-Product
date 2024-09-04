#include <cassert>
#include <chrono>
#include <iostream>

#include "two_party_computation.h"

#include "..\math\mpint32.h"
#include "..\math\polynomial.h"
#include "..\circuit\inner_product_circuit.h"

using namespace std;

// Fully Linear PCP between one prover and one verifier
FLPCPMeasurement TwoPC::FLPCP(size_t inputLength, size_t nGGate)
{
    Mpint32::SetSeed(10);

    Mpint32* const op0 = new Mpint32[inputLength];
    for (size_t i = 0; i < inputLength; ++i)
    {
        op0[i] = Mpint32::GenerateRandom();
    }

    Mpint32* const op1 = new Mpint32[inputLength];
    for (size_t i = 0; i < inputLength; ++i)
    {
        op1[i] = Mpint32::GenerateRandom();
    }

    const Mpint32 trueResult = InnerProductCircuit::Forward(op0, op1, inputLength);

    // Prover make proof vector : (inputs || constant terms || coefficients)
    auto start_proof = chrono::high_resolution_clock::now();
    Proof proof = InnerProductCircuit::MakeProof(op0, op1, inputLength, nGGate);
    auto end_proof = chrono::high_resolution_clock::now();
    double time_taken_proof = chrono::duration_cast<chrono::nanoseconds>(end_proof - start_proof).count();

    // Verifier make queries and perform inner products between proof and queries.
    // Assumption : Verifier only has linear access on proof vector.
    auto start_query = chrono::high_resolution_clock::now();
    std::vector<Query> queries =
        InnerProductCircuit::MakeQuery(Mpint32::GenerateRandomAbove(nGGate + 1), nGGate, inputLength);
    
    const size_t nInputQueriesHalf = (queries.size() - 2u) / 2u;
    Mpint32 gR(0u);
    for (size_t i = 0; i < nInputQueriesHalf; ++i)
    {
        gR += proof.GetQueryAnswer(queries[i]) * proof.GetQueryAnswer(queries[i + nInputQueriesHalf]);
    }

    bool isValid = (proof.GetQueryAnswer(queries[queries.size() - 2u]) == gR) &&
                   (proof.GetQueryAnswer(queries[queries.size() - 1u]) == trueResult);

    auto end_query = chrono::high_resolution_clock::now();
    double time_taken_query = chrono::duration_cast<chrono::nanoseconds>(end_query - start_query).count();

    delete[] op0;
    delete[] op1;

    return FLPCPMeasurement(proof.GetBytes(), queries.size(), time_taken_proof * 1e-6, time_taken_query * 1e-6,
                            isValid);
}

void TwoPC::ExperimentFLPCP()
{
    size_t j = 0;
    FLPCPMeasurement measures[10];
    for (size_t i = 1; i <= 10; ++i)
    {
        measures[j] = TwoPC::FLPCP(i * i * 10, i * i * 10);
        if (!measures[j++].isVaild)
        {
            std::cout << "Invalid!" << std::endl;
            return;
        }
    }

    std::cout << "Vector Length : ";
    for (size_t i = 1; i <= 10; ++i)
    {
        std::cout << i * i * 10 << ", ";
    }
    std::cout << std::endl;

    std::cout << "# of G-gates : ";
    for (size_t i = 1; i <= 10; ++i)
    {
        std::cout << i * i * 10 << ", ";
    }
    std::cout << std::endl;

    std::cout << "Proof Length : ";
    for (size_t i = 0; i < 10; ++i)
    {
        std::cout << measures[i].proofLength << ", ";
    }
    std::cout << std::endl;

    std::cout << "Query Complexity : ";
    for (size_t i = 0; i < 10; ++i)
    {
        std::cout << measures[i].nQueries << ", ";
    }
    std::cout << std::endl;

    std::cout << "Prover Time : ";
    for (size_t i = 0; i < 10; ++i)
    {
        std::cout << std::fixed << measures[i].proverTime << std::setprecision(9) << ", ";
    }
    std::cout << std::endl;

    std::cout << "Verifier Time : ";
    for (size_t i = 0; i < 10; ++i)
    {
        std::cout << std::fixed << measures[i].verifierTime << std::setprecision(9) << ", ";
    }
    std::cout << std::endl;
}

void TwoPC::ExperimentFLPCPSqrt()
{
    size_t j = 0;
    FLPCPMeasurement measures[10];
    for (size_t i = 10; i <= 100; i += 10)
    {
        measures[j] = TwoPC::FLPCP(i * i, i);
        if (!measures[j++].isVaild)
        {
            std::cout << "Invalid!" << std::endl;
            return;
        }
    }

    std::cout << "Vector Length : ";
    for (size_t i = 10; i <= 100; i += 10)
    {
        std::cout << i * i << ", ";
    }
    std::cout << std::endl;

    std::cout << "# of G-gates : ";
    for (size_t i = 10; i <= 100; i += 10)
    {
        std::cout << i << ", ";
    }
    std::cout << std::endl;

    std::cout << "Proof Length : ";
    for (size_t i = 0; i < 10; ++i)
    {
        std::cout << measures[i].proofLength << ", ";
    }
    std::cout << std::endl;

    std::cout << "Query Complexity : ";
    for (size_t i = 0; i < 10; ++i)
    {
        std::cout << measures[i].nQueries << ", ";
    }
    std::cout << std::endl;

    std::cout << "Prover Time : ";
    for (size_t i = 0; i < 10; ++i)
    {
        std::cout << std::fixed << measures[i].proverTime << std::setprecision(9) << ", ";
    }
    std::cout << std::endl;

    std::cout << "Verifier Time : ";
    for (size_t i = 0; i < 10; ++i)
    {
        std::cout << std::fixed << measures[i].verifierTime << std::setprecision(9) << ", ";
    }
    std::cout << std::endl;
}

// Fully Linear PCP using input as coefficient of polynomials
FLPCPMeasurement TwoPC::FLPCPCoefficient(size_t inputLength, size_t nPoly)
{
    Mpint32::SetSeed(10);

    Mpint32* const op0 = new Mpint32[inputLength];
    for (size_t i = 0; i < inputLength; ++i)
    {
        op0[i] = Mpint32::GenerateRandom();
    }

    Mpint32* const op1 = new Mpint32[inputLength];
    for (size_t i = 0; i < inputLength; ++i)
    {
        op1[i] = Mpint32::GenerateRandom();
    }

    const Mpint32 trueResult = InnerProductCircuit::Forward(op0, op1, inputLength);

    auto start_proof = chrono::high_resolution_clock::now();
    Proof proof = InnerProductCircuit::MakeCoefficientProof(op0, op1, inputLength, nPoly);
    auto end_proof = chrono::high_resolution_clock::now();
    double time_taken_proof = chrono::duration_cast<chrono::nanoseconds>(end_proof - start_proof).count();

    auto start_query = chrono::high_resolution_clock::now();
    std::vector<Query> queries =
        InnerProductCircuit::MakeCoefficientQuery(Mpint32::GenerateRandom(), inputLength, nPoly);
    
    const size_t nInputQueriesHalf = (queries.size() - 2u) / 2u;
    Mpint32 gR(0u);
    for (size_t i = 0; i < nInputQueriesHalf; ++i)
    {
        gR += proof.GetQueryAnswer(queries[i]) * proof.GetQueryAnswer(queries[i + nInputQueriesHalf]);
    }

    bool isValid = (proof.GetQueryAnswer(queries[queries.size() - 2u]) == gR) &&
                   (proof.GetQueryAnswer(queries[queries.size() - 1u]) == trueResult);

    auto end_query = chrono::high_resolution_clock::now();
    double time_taken_query = chrono::duration_cast<chrono::nanoseconds>(end_query - start_query).count();

    delete[] op0;
    delete[] op1;

    return FLPCPMeasurement(proof.GetBytes(), queries.size(), time_taken_proof * 1e-6, time_taken_query * 1e-6,
                            isValid);
}

void TwoPC::ExperimentFLPCPCoefficient()
{
    size_t j = 0;
    FLPCPMeasurement measures[10];
    for (size_t i = 1; i <= 10; ++i)
    {
        measures[j] = TwoPC::FLPCPCoefficient(i * i * 10, 1);
        if (!measures[j++].isVaild)
        {
            std::cout << "Invalid!" << std::endl;
            return;
        }
    }

    std::cout << "Vector Length : ";
    for (size_t i = 1; i <= 10; i += 1)
    {
        std::cout << i * i * 10 << ", ";
    }
    std::cout << std::endl;

    std::cout << "# of polynomial : ";
    for (size_t i = 1; i <= 10; i += 1)
    {
        std::cout << 1 << ", ";
    }
    std::cout << std::endl;

    std::cout << "Proof Length : ";
    for (size_t i = 0; i < 10; ++i)
    {
        std::cout << measures[i].proofLength << ", ";
    }
    std::cout << std::endl;

    std::cout << "Query Complexity : ";
    for (size_t i = 0; i < 10; ++i)
    {
        std::cout << measures[i].nQueries << ", ";
    }
    std::cout << std::endl;

    std::cout << "Prover Time : ";
    for (size_t i = 0; i < 10; ++i)
    {
        std::cout << std::fixed << measures[i].proverTime << std::setprecision(9) << ", ";
    }
    std::cout << std::endl;

    std::cout << "Verifier Time : ";
    for (size_t i = 0; i < 10; ++i)
    {
        std::cout << std::fixed << measures[i].verifierTime << std::setprecision(9) << ", ";
    }
    std::cout << std::endl;
}

void TwoPC::ExperimentFLPCPCoefficientSqrt()
{
    size_t j = 0;
    FLPCPMeasurement measures[10];
    for (size_t i = 10; i <= 100; i += 10)
    {
        measures[j] = TwoPC::FLPCPCoefficient(i * i, i);
        if (!measures[j++].isVaild)
        {
            std::cout << "Invalid!" << std::endl;
            return;
        }
    }

    std::cout << "Vector Length : ";
    for (size_t i = 10; i <= 100; i += 10)
    {
        std::cout << i * i << ", ";
    }
    std::cout << std::endl;

    std::cout << "# of G-gates : ";
    for (size_t i = 10; i <= 100; i += 10)
    {
        std::cout << i << ", ";
    }
    std::cout << std::endl;

    std::cout << "Proof Length : ";
    for (size_t i = 0; i < 10; ++i)
    {
        std::cout << measures[i].proofLength << ", ";
    }
    std::cout << std::endl;

    std::cout << "Query Complexity : ";
    for (size_t i = 0; i < 10; ++i)
    {
        std::cout << measures[i].nQueries << ", ";
    }
    std::cout << std::endl;

    std::cout << "Prover Time : ";
    for (size_t i = 0; i < 10; ++i)
    {
        std::cout << std::fixed << measures[i].proverTime << std::setprecision(9) << ", ";
    }
    std::cout << std::endl;

    std::cout << "Verifier Time : ";
    for (size_t i = 0; i < 10; ++i)
    {
        std::cout << std::fixed << measures[i].verifierTime << std::setprecision(9) << ", ";
    }
    std::cout << std::endl;
}

FLPCPMeasurement TwoPC::FLIOP(const size_t inputLength, const size_t compressFactor)
{
    Mpint32::SetSeed(10u);

    std::vector<Mpint32> op0(inputLength); 
    for (size_t i = 0; i < inputLength; ++i)
    {
        op0[i] = Mpint32::GenerateRandom();
    }
    std::vector<Mpint32> op1(inputLength);
    for (size_t i = 0; i < inputLength; ++i)
    {
        op1[i] = Mpint32::GenerateRandom();
    }

    double time_taken_proof = 0.;
    double time_taken_query = 0.;
    size_t totalProofSize = 0u;
    size_t totalQueryComplexity = 0u;

    bool isValid = true;
    Mpint32 out = InnerProductCircuit::Forward(op0.data(), op1.data(), inputLength);
    while (ceil(op0.size() / (double)compressFactor) > 1)
    {
        auto start_proof = chrono::high_resolution_clock::now();
        InteractiveProof proof =
            InnerProductCircuit::MakeRoundProof(op0.data(), op1.data(), op0.size(), compressFactor);
        auto end_proof = chrono::high_resolution_clock::now();
        time_taken_proof += chrono::duration_cast<chrono::nanoseconds>(end_proof - start_proof).count();

        auto start_query = chrono::high_resolution_clock::now();
        Mpint32 random = Mpint32::GenerateRandom();
        std::vector<Query> queries = InnerProductCircuit::MakeRoundQuery(random, compressFactor);
        isValid = isValid && (out == proof.GetQueryAnswer(queries[0]));
        out = proof.GetQueryAnswer(queries[1]);
        auto end_query = chrono::high_resolution_clock::now();
        time_taken_query += chrono::duration_cast<chrono::nanoseconds>(end_query - start_query).count();

        start_proof = chrono::high_resolution_clock::now();
        op0 = proof.EvaluatePolyPs(random);
        op1 = proof.EvaluatePolyQs(random);
        end_proof = chrono::high_resolution_clock::now();
        time_taken_proof += chrono::duration_cast<chrono::nanoseconds>(end_proof - start_proof).count();

        totalProofSize += proof.GetBytes();
        totalQueryComplexity += 2;
    }

    auto start_proof = chrono::high_resolution_clock::now();
    Proof proof = InnerProductCircuit::MakeProof(op0.data(), op1.data(), op0.size(), op0.size());
    auto end_proof = chrono::high_resolution_clock::now();
    time_taken_proof += chrono::duration_cast<chrono::nanoseconds>(end_proof - start_proof).count();

    auto start_query = chrono::high_resolution_clock::now();
    std::vector<Query> queries = InnerProductCircuit::MakeQuery(Mpint32::GenerateRandom(), op0.size(), op0.size());
    
    const size_t nInputQueriesHalf = (queries.size() - 2u) / 2u;
    Mpint32 gR(0u);
    for (size_t i = 0; i < nInputQueriesHalf; ++i)
    {
        gR += proof.GetQueryAnswer(queries[i]) * proof.GetQueryAnswer(queries[i + nInputQueriesHalf]);
    }
    isValid = isValid && (proof.GetQueryAnswer(queries[queries.size() - 2u]) == gR) &&
              (proof.GetQueryAnswer(queries[queries.size() - 1u]) == out);
    
    auto end_query = chrono::high_resolution_clock::now();
    time_taken_query += chrono::duration_cast<chrono::nanoseconds>(end_query - start_query).count();

    totalProofSize += proof.GetBytes();
    totalQueryComplexity += queries.size();

    return FLPCPMeasurement(totalProofSize, totalQueryComplexity, time_taken_proof * 1e-6, time_taken_query * 1e-6,
                            isValid);
}

void TwoPC::ExperimentFLIOP()
{
    size_t j = 0;
    FLPCPMeasurement measures[10];
    for (size_t i = 4; i <= pow(4, 10); i *= 4)
    {
        measures[j] = TwoPC::FLIOP(i, 2);
        if (!measures[j++].isVaild)
        {
            std::cout << "Invalid!" << std::endl;
            return;
        }
    }

    std::cout << "Vector Length : ";
    for (size_t i = 4; i <= pow(4, 10); i *= 4)
    {
        std::cout << i << ", ";
    }
    std::cout << std::endl;

    std::cout << "Proof Length : ";
    for (size_t i = 0; i < 10; ++i)
    {
        std::cout << measures[i].proofLength << ", ";
    }
    std::cout << std::endl;

    std::cout << "Query Complexity : ";
    for (size_t i = 0; i < 10; ++i)
    {
        std::cout << measures[i].nQueries << ", ";
    }
    std::cout << std::endl;

    std::cout << "Prover Time : ";
    for (size_t i = 0; i < 10; ++i)
    {
        std::cout << std::fixed << measures[i].proverTime << std::setprecision(9) << ", ";
    }
    std::cout << std::endl;

    std::cout << "Verifier Time : ";
    for (size_t i = 0; i < 10; ++i)
    {
        std::cout << std::fixed << measures[i].verifierTime << std::setprecision(9) << ", ";
    }
    std::cout << std::endl;
}

FLPCPMeasurement TwoPC::FLIOPCoefficient(const size_t inputLength, const size_t compressFactor)
{
    Mpint32::SetSeed(10u);

    std::vector<Mpint32> op0(inputLength);
    for (size_t i = 0; i < inputLength; ++i)
    {
        op0[i] = Mpint32::GenerateRandom();
    }
    std::vector<Mpint32> op1(inputLength);
    for (size_t i = 0; i < inputLength; ++i)
    {
        op1[i] = Mpint32::GenerateRandom();
    }

    double time_taken_proof = 0.;
    double time_taken_query = 0.;
    size_t nRounds = 0u;
    size_t totalProofSize = 0u;
    size_t totalQueryComplexity = 0u;

    bool isValid = true;
    Mpint32 out = InnerProductCircuit::Forward(op0.data(), op1.data(), inputLength);
    while (ceil(op0.size() / (double)compressFactor) > 1)
    {
        auto start_proof = chrono::high_resolution_clock::now();
        InteractiveProof proof = InnerProductCircuit::MakeRoundCoefficientProof(op0.data(), op1.data(), op0.size(), compressFactor);
        auto end_proof = chrono::high_resolution_clock::now();
        time_taken_proof += chrono::duration_cast<chrono::nanoseconds>(end_proof - start_proof).count();

        auto start_query = chrono::high_resolution_clock::now();
        Mpint32 random = Mpint32::GenerateRandom();
        std::vector<Query> queries = InnerProductCircuit::MakeRoundCoefficientQuery(random, compressFactor);
        isValid = isValid && (out == proof.GetQueryAnswer(queries[0]));
        out = proof.GetQueryAnswer(queries[1]);
        auto end_query = chrono::high_resolution_clock::now();
        time_taken_query += chrono::duration_cast<chrono::nanoseconds>(end_query - start_query).count();

        start_proof = chrono::high_resolution_clock::now();
        op0 = proof.EvaluatePolyPs(random);
        op1 = proof.EvaluatePolyQs(random);
        end_proof = chrono::high_resolution_clock::now();
        time_taken_proof += chrono::duration_cast<chrono::nanoseconds>(end_proof - start_proof).count();

        ++nRounds;
        totalProofSize += proof.GetBytes();
        totalQueryComplexity += 2;
    }

    auto start_proof = chrono::high_resolution_clock::now();
    Proof proof = InnerProductCircuit::MakeCoefficientProof(op0.data(), op1.data(), op0.size(), 1);
    auto end_proof = chrono::high_resolution_clock::now();
    time_taken_proof += chrono::duration_cast<chrono::nanoseconds>(end_proof - start_proof).count();

    auto start_query = chrono::high_resolution_clock::now();
    std::vector<Query> queries = InnerProductCircuit::MakeCoefficientQuery(Mpint32::GenerateRandom(), op0.size(), 1);
    isValid =
        isValid &&
        (proof.GetQueryAnswer(queries[0]) * proof.GetQueryAnswer(queries[1]) == proof.GetQueryAnswer(queries[2])) &&
        (proof.GetQueryAnswer(queries[3]) == out);
    auto end_query = chrono::high_resolution_clock::now();
    time_taken_query += chrono::duration_cast<chrono::nanoseconds>(end_query - start_query).count();

    ++nRounds;
    totalProofSize += proof.GetBytes();
    totalQueryComplexity += 4;

    return FLPCPMeasurement(totalProofSize, totalQueryComplexity, time_taken_proof * 1e-6, time_taken_query * 1e-6,
                            isValid);
}

void TwoPC::ExperimentFLIOPCoefficient()
{
    size_t j = 0;
    FLPCPMeasurement measures[10];
    for (size_t i = 4; i <= pow(4, 10); i *= 4)
    {
        measures[j] = TwoPC::FLIOPCoefficient(i, 2);
        if (!measures[j++].isVaild)
        {
            std::cout << "Invalid!" << std::endl;
            return;
        }
    }

    std::cout << "Vector Length : ";
    for (size_t i = 4; i <= pow(4, 10); i *= 4)
    {
        std::cout << i << ", ";
    }
    std::cout << std::endl;

    std::cout << "Proof Length : ";
    for (size_t i = 0; i < 10; ++i)
    {
        std::cout << measures[i].proofLength << ", ";
    }
    std::cout << std::endl;

    std::cout << "Query Complexity : ";
    for (size_t i = 0; i < 10; ++i)
    {
        std::cout << measures[i].nQueries << ", ";
    }
    std::cout << std::endl;

    std::cout << "Prover Time : ";
    for (size_t i = 0; i < 10; ++i)
    {
        std::cout << std::fixed << measures[i].proverTime << std::setprecision(9) << ", ";
    }
    std::cout << std::endl;

    std::cout << "Verifier Time : ";
    for (size_t i = 0; i < 10; ++i)
    {
        std::cout << std::fixed << measures[i].verifierTime << std::setprecision(9) << ", ";
    }
    std::cout << std::endl;
}
