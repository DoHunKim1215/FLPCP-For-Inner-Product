#include <algorithm>
#include <cassert>
#include <cfloat>
#include <chrono>
#include <iostream>

#include "three_party_computation.h"
#include "..\math\mpint32.h"
#include "..\unit\proof.h"
#include "..\circuit\inner_product_circuit.h"

using namespace std;

double ThreePC::GetLANDelay(size_t payloadBytes)
{
    return 0.3 * 1e+5 + 1.3 * 1e+2 * payloadBytes;
}

double ThreePC::GetWANDelay(size_t payloadBytes)
{
    return 0.4 * 1e+8 + 1.7 * 1e+5 * payloadBytes;
}

void ThreePC::FLPCP(size_t inputLength, size_t nGGate)
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
    Mpint32 result = InnerProductCircuit::Forward(op0.data(), op1.data(), op0.size());

    size_t totalCommunicationBytes = 0u;

    auto start = chrono::high_resolution_clock::now();
    Proof proof = InnerProductCircuit::MakeProof(op0.data(), op1.data(), op0.size(), nGGate);
    std::vector<Proof> proofShares = proof.GetShares(2);
    auto end = chrono::high_resolution_clock::now();
    double time_taken_proof = chrono::duration_cast<chrono::nanoseconds>(end - start).count();
    time_taken_proof *= 1e-9;

    totalCommunicationBytes += proof.GetBytes();
    totalCommunicationBytes += 2u * sizeof(Mpint32); // Exchange Random Value

    start = chrono::high_resolution_clock::now();
    std::vector<Query> queries = InnerProductCircuit::MakeQuery(Mpint32::GenerateRandom(), nGGate, inputLength);
    end = chrono::high_resolution_clock::now();
    double time_taken_query = chrono::duration_cast<chrono::nanoseconds>(end - start).count() * 1e-9;

    start = chrono::high_resolution_clock::now();
    std::vector<Mpint32> fR0;
    fR0.reserve(queries.size() - 2u);
    for (size_t i = 0; i < queries.size() - 2u; ++i)
    {
        fR0.push_back(proofShares[0].GetQueryAnswer(queries[i]));
    }
    Mpint32 varificationShare0 = proofShares[0].GetQueryAnswer(queries[queries.size() - 2u]);
    Mpint32 resultShare0 = proofShares[0].GetQueryAnswer(queries[queries.size() - 1u]);
    end = chrono::high_resolution_clock::now();
    time_taken_query += chrono::duration_cast<chrono::nanoseconds>(end - start).count() * 1e-9;

    totalCommunicationBytes += (queries.size() - 2u) * sizeof(Mpint32); // fR0

    std::vector<Mpint32> fR1;
    fR1.reserve(queries.size() - 2u);
    for (size_t i = 0; i < queries.size() - 2u; ++i)
    {
        fR1.push_back(proofShares[1].GetQueryAnswer(queries[i]));
    }
    Mpint32 varificationShare1 = proofShares[1].GetQueryAnswer(queries[queries.size() - 2u]);
    Mpint32 resultShare1 = proofShares[1].GetQueryAnswer(queries[queries.size() - 1u]);

    totalCommunicationBytes += (queries.size() - 2u) * sizeof(Mpint32); // fR1
    totalCommunicationBytes += 4u * sizeof(Mpint32); // var, res

    start = chrono::high_resolution_clock::now();
    const size_t nInputQueriesHalf = (queries.size() - 2u) / 2u;
    Mpint32 gR(0u);
    for (size_t i = 0; i < nInputQueriesHalf; ++i)
    {
        gR += (fR0[i] + fR1[i]) * (fR0[i + nInputQueriesHalf] + fR1[i + nInputQueriesHalf]);
    }
    end = chrono::high_resolution_clock::now();
    time_taken_query += chrono::duration_cast<chrono::nanoseconds>(end - start).count() * 1e-9;

    bool isValid = (gR == varificationShare0 + varificationShare1) && (result == resultShare0 + resultShare1);

    cout << "----------------------------------------------" << endl;
    cout << "Input vector size : " << inputLength << endl;
    cout << "The number of G-gate : " << nGGate << endl;
    cout << "Proof length : " << proof.GetBytes() << endl;
    cout << "Communication cost : " << totalCommunicationBytes << endl;
    cout << "Time taken by making proof is : " << fixed << time_taken_proof << setprecision(9);
    cout << " sec" << endl;
    cout << "Time taken by verifier computation is : " << fixed << time_taken_query << setprecision(9);
    cout << " sec" << endl;
    cout << "Time taken by LAN communication is : " << fixed << GetLANDelay(totalCommunicationBytes) * 1e-9
         << setprecision(9);
    cout << " sec" << endl;
    cout << "Time taken by WAN communication is : " << fixed << GetWANDelay(totalCommunicationBytes) * 1e-9
         << setprecision(9);
    cout << " sec" << endl;
    cout << "Total time on LAN env is : " << fixed
         << time_taken_proof + time_taken_query + GetLANDelay(totalCommunicationBytes) * 1e-9
         << setprecision(9);
    cout << " sec" << endl;
    cout << "Total time on WAN env is : " << fixed
         << time_taken_proof + time_taken_query + GetWANDelay(totalCommunicationBytes) * 1e-9
         << setprecision(9);
    cout << " sec" << endl;
    cout << (isValid ? "Valid!" : "Invalid!") << endl;
    cout << "----------------------------------------------" << endl;
}

void ThreePC::FLPCPCoefficient(size_t inputLength, size_t nGGate)
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
    Mpint32 result = InnerProductCircuit::Forward(op0.data(), op1.data(), op0.size());

    size_t totalCommunicationBytes = 0u;

    auto start = chrono::high_resolution_clock::now();
    Proof proof = InnerProductCircuit::MakeCoefficientProof(op0.data(), op1.data(), op0.size(), nGGate);
    std::vector<Proof> proofShares = proof.GetShares(2);
    auto end = chrono::high_resolution_clock::now();
    double time_taken_proof = chrono::duration_cast<chrono::nanoseconds>(end - start).count();
    time_taken_proof *= 1e-9;

    totalCommunicationBytes += proof.GetBytes();
    totalCommunicationBytes += 2u * sizeof(Mpint32); // Exchange Random Value

    start = chrono::high_resolution_clock::now();
    std::vector<Query> queries = InnerProductCircuit::MakeCoefficientQuery(Mpint32::GenerateRandom(), inputLength, nGGate);
    end = chrono::high_resolution_clock::now();
    double time_taken_query = chrono::duration_cast<chrono::nanoseconds>(end - start).count() * 1e-9;

    start = chrono::high_resolution_clock::now();
    std::vector<Mpint32> fR0;
    fR0.reserve(queries.size() - 2u);
    for (size_t i = 0; i < queries.size() - 2u; ++i)
    {
        fR0.push_back(proofShares[0].GetQueryAnswer(queries[i]));
    }
    Mpint32 varificationShare0 = proofShares[0].GetQueryAnswer(queries[queries.size() - 2u]);
    Mpint32 resultShare0 = proofShares[0].GetQueryAnswer(queries[queries.size() - 1u]);
    end = chrono::high_resolution_clock::now();
    time_taken_query += chrono::duration_cast<chrono::nanoseconds>(end - start).count() * 1e-9;

    totalCommunicationBytes += (queries.size() - 2u) * sizeof(Mpint32); // fR0

    std::vector<Mpint32> fR1;
    fR1.reserve(queries.size() - 2u);
    for (size_t i = 0; i < queries.size() - 2u; ++i)
    {
        fR1.push_back(proofShares[1].GetQueryAnswer(queries[i]));
    }
    Mpint32 varificationShare1 = proofShares[1].GetQueryAnswer(queries[queries.size() - 2u]);
    Mpint32 resultShare1 = proofShares[1].GetQueryAnswer(queries[queries.size() - 1u]);

    totalCommunicationBytes += (queries.size() - 2u) * sizeof(Mpint32); // fR1
    totalCommunicationBytes += 4u * sizeof(Mpint32);                    // var, res

    start = chrono::high_resolution_clock::now();
    const size_t nInputQueriesHalf = (queries.size() - 2u) / 2u;
    Mpint32 gR(0u);
    for (size_t i = 0; i < nInputQueriesHalf; ++i)
    {
        gR += (fR0[i] + fR1[i]) * (fR0[i + nInputQueriesHalf] + fR1[i + nInputQueriesHalf]);
    }
    end = chrono::high_resolution_clock::now();
    time_taken_query += chrono::duration_cast<chrono::nanoseconds>(end - start).count() * 1e-9;

    bool isValid = (gR == varificationShare0 + varificationShare1) && (result == resultShare0 + resultShare1);

    cout << "----------------------------------------------" << endl;
    cout << "Input vector size : " << inputLength << endl;
    cout << "Proof length : " << proof.GetBytes() << endl;
    cout << "Communication cost : " << totalCommunicationBytes << endl;
    cout << "Time taken by making proof is : " << fixed << time_taken_proof << setprecision(9);
    cout << " sec" << endl;
    cout << "Time taken by getting query answers is : " << fixed << time_taken_query << setprecision(9);
    cout << " sec" << endl;
    cout << "Time taken by LAN communication is : " << fixed << GetLANDelay(totalCommunicationBytes) * 1e-9
         << setprecision(9);
    cout << " sec" << endl;
    cout << "Time taken by WAN communication is : " << fixed << GetWANDelay(totalCommunicationBytes) * 1e-9
         << setprecision(9);
    cout << " sec" << endl;
    cout << "Total time on LAN env is : " << fixed
         << time_taken_proof + time_taken_query + GetLANDelay(totalCommunicationBytes) * 1e-9
         << setprecision(9);
    cout << " sec" << endl;
    cout << "Total time on WAN env is : " << fixed
         << time_taken_proof + time_taken_query + GetWANDelay(totalCommunicationBytes) * 1e-9
         << setprecision(9);
    cout << " sec" << endl;
    cout << (isValid ? "Valid!" : "Invalid!") << endl;
    cout << "----------------------------------------------" << endl;
}

void ThreePC::FLIOP(const size_t inputLength, const size_t compressFactor)
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
    size_t totalCommunicationBytes = 0u;

    bool isValid = true;
    Mpint32 out0 = InnerProductCircuit::Forward(op0.data(), op1.data(), op0.size());
    Mpint32 out1 = Mpint32(0u);
    while (ceil(op0.size() / (double)compressFactor) > 1)
    {
        auto start_proof = chrono::high_resolution_clock::now();
        InteractiveProof proof =
            InnerProductCircuit::MakeRoundProof(op0.data(), op1.data(), op0.size(), compressFactor);
        std::vector<Proof> proofShares = proof.GetShares(2);
        auto end_proof = chrono::high_resolution_clock::now();
        time_taken_proof += chrono::duration_cast<chrono::nanoseconds>(end_proof - start_proof).count();

        totalCommunicationBytes += proof.GetBytes();
        totalCommunicationBytes += 2u * sizeof(Mpint32); // Exchange Random Value

        auto start_query = chrono::high_resolution_clock::now();
        Mpint32 random = Mpint32::GenerateRandom();
        std::vector<Query> queries = InnerProductCircuit::MakeRoundQuery(random, compressFactor);
        Mpint32 outShare0 = proofShares[0].GetQueryAnswer(queries[0]) - out0;
        auto end_query = chrono::high_resolution_clock::now();
        time_taken_query += chrono::duration_cast<chrono::nanoseconds>(end_proof - start_proof).count();

        Mpint32 outShare1 = proofShares[1].GetQueryAnswer(queries[0]) - out1;

        totalCommunicationBytes += 2u * sizeof(Mpint32); // outShare

        start_query = chrono::high_resolution_clock::now();
        isValid = isValid && (outShare0 + outShare1 == Mpint32(0u));
        out0 = proofShares[0].GetQueryAnswer(queries[1]);

        const size_t nPoly = ceil(op0.size() / (double)compressFactor);
        Mpint32* const resizedInput = new Mpint32[nPoly * compressFactor];
        std::memset(resizedInput, 0, (nPoly * compressFactor) * sizeof(Mpint32));
        std::memcpy(resizedInput, op0.data(), op0.size() * sizeof(Mpint32));
        std::vector<Mpint32Poly> poly0s;
        poly0s.reserve(nPoly);
        for (size_t i = 0; i < nPoly; ++i)
        {
            poly0s.emplace_back(
                Mpint32Poly::LagrangeInterpolation(resizedInput + compressFactor * i, compressFactor));
        }
        op0 = proof.EvaluatePolyPs(random);
        delete[] resizedInput;

        end_query = chrono::high_resolution_clock::now();
        time_taken_query += chrono::duration_cast<chrono::nanoseconds>(end_proof - start_proof).count();

        out1 = proofShares[1].GetQueryAnswer(queries[1]);

        totalCommunicationBytes += sizeof(Mpint32); // V -> P (random)

        start_proof = chrono::high_resolution_clock::now();
        op0 = proof.EvaluatePolyPs(random);
        op1 = proof.EvaluatePolyQs(random);
        end_proof = chrono::high_resolution_clock::now();
        time_taken_proof += chrono::duration_cast<chrono::nanoseconds>(end_proof - start_proof).count();

        ++nRounds;
        totalProofSize += proof.GetBytes();
    }

    auto start = chrono::high_resolution_clock::now();
    Proof proof = InnerProductCircuit::MakeProof(op0.data(), op1.data(), op0.size(), op0.size());
    std::vector<Proof> proofShares = proof.GetShares(2);
    auto end = chrono::high_resolution_clock::now();
    time_taken_proof += chrono::duration_cast<chrono::nanoseconds>(end - start).count();

    totalCommunicationBytes += proof.GetBytes();
    totalCommunicationBytes += 2u * sizeof(Mpint32); // Exchange Random Value
    totalCommunicationBytes += 2u * sizeof(Mpint32); // P -> V (randoms)

    Mpint32* randomQueries = new Mpint32[proof.GetLength() * 2u];
    std::memset(randomQueries, 0, proof.GetLength() * 2u * sizeof(Mpint32));
    randomQueries[op0.size() * 2u] = Mpint32(1u);
    randomQueries[proof.GetLength() + op0.size() * 2u + 1u] = Mpint32(1u);
    Query randomQuery0(randomQueries, proof.GetLength());
    Query randomQuery1(randomQueries + proof.GetLength(), proof.GetLength());
    delete[] randomQueries;

    start = chrono::high_resolution_clock::now();
    Mpint32 random = Mpint32::GenerateRandom();
    std::vector<Query> queries = InnerProductCircuit::MakeQuery(random, op0.size(), op0.size());

    Mpint32* const resizedInput0 = new Mpint32[op0.size() + 1u];
    std::memset(resizedInput0, 0, (op0.size() + 1u) * sizeof(Mpint32));
    *resizedInput0 = proof.GetQueryAnswer(randomQuery0);
    std::memcpy(resizedInput0 + 1u, op0.data(), op0.size() * sizeof(Mpint32));
    Mpint32Poly poly0;
    poly0 = Mpint32Poly::LagrangeInterpolation(resizedInput0, op0.size() + 1u);
    Mpint32 pR = poly0.Evaluate(random);
    delete[] resizedInput0;

    Mpint32 varificationShare0 = proofShares[0].GetQueryAnswer(queries[queries.size() - 2u]);
    Mpint32 resultShare0 = proofShares[0].GetQueryAnswer(queries[queries.size() - 1u]) - out0;
    end = chrono::high_resolution_clock::now();
    time_taken_query += chrono::duration_cast<chrono::nanoseconds>(end - start).count();

    Mpint32* const resizedInput1 = new Mpint32[op0.size() + 1u];
    std::memset(resizedInput1, 0, (op0.size() + 1u) * sizeof(Mpint32));
    *resizedInput1 = proof.GetQueryAnswer(randomQuery1);
    std::memcpy(resizedInput1 + 1u, op1.data(), op1.size() * sizeof(Mpint32));
    Mpint32Poly poly1;
    poly1 = Mpint32Poly::LagrangeInterpolation(resizedInput1, op0.size() + 1u);
    Mpint32 qR = poly1.Evaluate(random);
    delete[] resizedInput1;

    Mpint32 varificationShare1 = proofShares[1].GetQueryAnswer(queries[queries.size() - 2u]) ;
    Mpint32 resultShare1 = proofShares[1].GetQueryAnswer(queries[queries.size() - 1u]) - out1;

    totalCommunicationBytes += 6u * sizeof(Mpint32); // pR, qR, res, var

    isValid = isValid && (Mpint32(0) == resultShare0 + resultShare1) &&
              (varificationShare0 + varificationShare1 == pR * qR);

    ++nRounds;
    totalProofSize += proof.GetBytes();

    cout << "----------------------------------------------" << endl;
    cout << "Input vector size : " << inputLength << endl;
    cout << "Compression factor : " << compressFactor << endl;
    cout << "The number of rounds : " << nRounds << endl;
    cout << "Proof length : " << totalProofSize << endl;
    cout << "Time taken by making proof is : " << fixed << time_taken_proof * 1e-9 << setprecision(9);
    cout << " sec" << endl;
    cout << "Time taken by making queries is : " << fixed << time_taken_query * 1e-9 << setprecision(9);
    cout << " sec" << endl;
    cout << "Time taken by LAN communication is : " << fixed << GetLANDelay(totalCommunicationBytes) * 1e-9
         << setprecision(9);
    cout << " sec" << endl;
    cout << "Time taken by WAN communication is : " << fixed << GetWANDelay(totalCommunicationBytes) * 1e-9
         << setprecision(9);
    cout << " sec" << endl;
    cout << "Total time on LAN env is : " << fixed
         << (time_taken_proof + time_taken_query + GetLANDelay(totalCommunicationBytes)) * 1e-9 << setprecision(9);
    cout << " sec" << endl;
    cout << "Total time on WAN env is : " << fixed
         << (time_taken_proof + time_taken_query + GetWANDelay(totalCommunicationBytes)) * 1e-9 << setprecision(9);
    cout << " sec" << endl;
    cout << (isValid ? "Valid!" : "Invalid!") << endl;
    cout << "----------------------------------------------" << endl;
}

void ThreePC::FLIOPCoefficient(const size_t inputLength, const size_t compressFactor)
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
    size_t totalCommunicationBytes = 0u;

    bool isValid = true;
    Mpint32 out0 = InnerProductCircuit::Forward(op0.data(), op1.data(), op0.size());
    Mpint32 out1 = Mpint32(0u);
    while (ceil(op0.size() / (double)compressFactor) > 1)
    {
        auto start_proof = chrono::high_resolution_clock::now();
        InteractiveProof proof =
            InnerProductCircuit::MakeRoundCoefficientProof(op0.data(), op1.data(), op0.size(), compressFactor);
        std::vector<Proof> proofShares = proof.GetShares(2);
        auto end_proof = chrono::high_resolution_clock::now();
        time_taken_proof += chrono::duration_cast<chrono::nanoseconds>(end_proof - start_proof).count();

        totalCommunicationBytes += proof.GetBytes();
        totalCommunicationBytes += 2u * sizeof(Mpint32); // Exchange Random Value

        auto start_query = chrono::high_resolution_clock::now();
        Mpint32 random = Mpint32::GenerateRandom();
        std::vector<Query> queries = InnerProductCircuit::MakeRoundCoefficientQuery(random, compressFactor);
        Mpint32 outShare0 = proofShares[0].GetQueryAnswer(queries[0]) - out0;
        auto end_query = chrono::high_resolution_clock::now();
        time_taken_query += chrono::duration_cast<chrono::nanoseconds>(end_proof - start_proof).count();

        Mpint32 outShare1 = proofShares[1].GetQueryAnswer(queries[0]) - out1;

        totalCommunicationBytes += 2u * sizeof(Mpint32); // outShare

        start_query = chrono::high_resolution_clock::now();
        isValid = isValid && (outShare0 + outShare1 == Mpint32(0u));
        out0 = proofShares[0].GetQueryAnswer(queries[1]);

        const size_t nPoly = ceil(op0.size() / (double)compressFactor);
        std::vector<Mpint32Poly> poly0s;
        poly0s.reserve(nPoly);
        for (size_t i = 0; i < nPoly; ++i)
        {
            poly0s.emplace_back(op0.data() + i * compressFactor, compressFactor, true);
        }
        op0 = proof.EvaluatePolyPs(random);

        end_query = chrono::high_resolution_clock::now();
        time_taken_query += chrono::duration_cast<chrono::nanoseconds>(end_proof - start_proof).count();

        out1 = proofShares[1].GetQueryAnswer(queries[1]);

        totalCommunicationBytes += sizeof(Mpint32); // V -> P (random)

        start_proof = chrono::high_resolution_clock::now();
        op0 = proof.EvaluatePolyPs(random);
        op1 = proof.EvaluatePolyQs(random);
        end_proof = chrono::high_resolution_clock::now();
        time_taken_proof += chrono::duration_cast<chrono::nanoseconds>(end_proof - start_proof).count();

        ++nRounds;
        totalProofSize += proof.GetBytes();
    }

    auto start = chrono::high_resolution_clock::now();
    Proof proof = InnerProductCircuit::MakeCoefficientProof(op0.data(), op1.data(), op0.size(), 1);
    std::vector<Proof> proofShares = proof.GetShares(2);
    auto end = chrono::high_resolution_clock::now();
    time_taken_proof += chrono::duration_cast<chrono::nanoseconds>(end - start).count();

    totalCommunicationBytes += proof.GetBytes();
    totalCommunicationBytes += 2u * sizeof(Mpint32); // Exchange Random Value
    totalCommunicationBytes += 2u * sizeof(Mpint32); // P -> V (randoms)

    Mpint32* randomQueries = new Mpint32[proof.GetLength() * 2u];
    std::memset(randomQueries, 0, proof.GetLength() * 2u * sizeof(Mpint32));
    randomQueries[op0.size() * 2u] = Mpint32(1u);
    randomQueries[proof.GetLength() + op0.size() * 2u + 1u] = Mpint32(1u);
    Query randomQuery0(randomQueries, proof.GetLength());
    Query randomQuery1(randomQueries + proof.GetLength(), proof.GetLength());
    delete[] randomQueries;

    start = chrono::high_resolution_clock::now();
    Mpint32 random = Mpint32::GenerateRandom();
    std::vector<Query> queries = InnerProductCircuit::MakeCoefficientQuery(random, op0.size(), 1);

    Mpint32* const resizedInput0 = new Mpint32[op0.size() + 1u];
    std::memset(resizedInput0, 0, (op0.size() + 1u) * sizeof(Mpint32));
    *resizedInput0 = proof.GetQueryAnswer(randomQuery0);
    std::memcpy(resizedInput0 + 1u, op0.data(), op0.size() * sizeof(Mpint32));
    Mpint32Poly poly0(resizedInput0, op0.size() + 1u, true);
    Mpint32 pR = poly0.Evaluate(random);
    delete[] resizedInput0;

    Mpint32 varificationShare0 = proofShares[0].GetQueryAnswer(queries[queries.size() - 2u]);
    Mpint32 resultShare0 = proofShares[0].GetQueryAnswer(queries[queries.size() - 1u]) - out0;
    end = chrono::high_resolution_clock::now();
    time_taken_query += chrono::duration_cast<chrono::nanoseconds>(end - start).count();

    Mpint32* const resizedInput1 = new Mpint32[op0.size() + 1u];
    std::memset(resizedInput1, 0, (op0.size() + 1u) * sizeof(Mpint32));
    *resizedInput1 = proof.GetQueryAnswer(randomQuery1);
    std::memcpy(resizedInput1 + 1u, op1.data(), op1.size() * sizeof(Mpint32));
    for (size_t i = 0; i < op0.size() / 2u; ++i)
    {
        Mpint32 temp = resizedInput1[i + 1];
        resizedInput1[i + 1] = resizedInput1[op0.size() - i];
        resizedInput1[op0.size() - i] = temp;
    }
    Mpint32Poly poly1(resizedInput1, op1.size() + 1u, true);
    Mpint32 qR = poly1.Evaluate(random);
    delete[] resizedInput1;

    Mpint32 varificationShare1 = proofShares[1].GetQueryAnswer(queries[queries.size() - 2u]);
    Mpint32 resultShare1 = proofShares[1].GetQueryAnswer(queries[queries.size() - 1u]) - out1;

    totalCommunicationBytes += 6u * sizeof(Mpint32); // pR, qR, res, var

    isValid =
        isValid && (Mpint32(0) == resultShare0 + resultShare1) && (varificationShare0 + varificationShare1 == pR * qR);

    ++nRounds;
    totalProofSize += proof.GetBytes();

    cout << "----------------------------------------------" << endl;
    cout << "Input vector size : " << inputLength << endl;
    cout << "Compression factor : " << compressFactor << endl;
    cout << "The number of rounds : " << nRounds << endl;
    cout << "Proof length : " << totalProofSize << endl;
    cout << "Time taken by making proof is : " << fixed << time_taken_proof * 1e-9 << setprecision(9);
    cout << " sec" << endl;
    cout << "Time taken by making queries is : " << fixed << time_taken_query * 1e-9 << setprecision(9);
    cout << " sec" << endl;
    cout << "Time taken by LAN communication is : " << fixed << GetLANDelay(totalCommunicationBytes) * 1e-9
         << setprecision(9);
    cout << " sec" << endl;
    cout << "Time taken by WAN communication is : " << fixed << GetWANDelay(totalCommunicationBytes) * 1e-9
         << setprecision(9);
    cout << " sec" << endl;
    cout << "Total time on LAN env is : " << fixed
         << (time_taken_proof + time_taken_query + GetLANDelay(totalCommunicationBytes)) * 1e-9 << setprecision(9);
    cout << " sec" << endl;
    cout << "Total time on WAN env is : " << fixed
         << (time_taken_proof + time_taken_query + GetWANDelay(totalCommunicationBytes)) * 1e-9 << setprecision(9);
    cout << " sec" << endl;
    cout << (isValid ? "Valid!" : "Invalid!") << endl;
    cout << "----------------------------------------------" << endl;
}

OneRoundMeasurement ThreePC::SimulateFLIOPOneRound(size_t inputLength, size_t compressFactor)
{
    Mpint32::SetSeed(10u);

    std::vector<Mpint32> op0(inputLength);
    std::fill(op0.begin(), op0.end(), Mpint32(0u));
    std::vector<Mpint32> op1(inputLength);
    std::fill(op1.begin(), op1.end(), Mpint32(0u));

    double proverTime = 0.;
    double verifierTime = 0.;
    double LANTime = 0.;
    double WANTime = 0.;

    bool isValid = true;
    Mpint32 out0 = InnerProductCircuit::Forward(op0.data(), op1.data(), op0.size());
    Mpint32 out1 = Mpint32(0u);
    if (ceil(op0.size() / (double)compressFactor) > 1)
    {
        // Prover
        auto startProver = chrono::high_resolution_clock::now();
        InteractiveProof proof =
            InnerProductCircuit::MakeRoundProof(op0.data(), op1.data(), op0.size(), compressFactor);
        std::vector<Proof> proofShares = proof.GetShares(2);
        auto endProver = chrono::high_resolution_clock::now();
        proverTime += chrono::duration_cast<chrono::nanoseconds>(endProver - startProver).count();


        // Communication
        // Prover send two shares of proof to two verifiers respectively.
        LANTime += GetLANDelay(proof.GetBytes());
        WANTime += GetWANDelay(proof.GetBytes());
        // Exchange random value between verifiers.
        LANTime += 2u * GetLANDelay(sizeof(Mpint32));
        WANTime += 2u * GetWANDelay(sizeof(Mpint32));


        // Verifier 1
        auto startVerifier = chrono::high_resolution_clock::now();
        Mpint32 random = Mpint32::GenerateRandom(); // Actually it's combination of two exchanged random values.
        std::vector<Query> queries = InnerProductCircuit::MakeRoundQuery(random, compressFactor);
        Mpint32 outShare0 = proofShares[0].GetQueryAnswer(queries[0]) - out0;
        auto endVerifier = chrono::high_resolution_clock::now();
        verifierTime += chrono::duration_cast<chrono::nanoseconds>(endVerifier - startVerifier).count();


        // Verifier 2
        Mpint32 outShare1 = proofShares[1].GetQueryAnswer(queries[0]) - out1;


        // Communication - Verifiers send their own shares of 'out' value.
        LANTime += 2u * GetLANDelay(sizeof(Mpint32));
        WANTime += 2u * GetWANDelay(sizeof(Mpint32));


        // Verifier 1
        startVerifier = chrono::high_resolution_clock::now();
        isValid = isValid && (outShare0 + outShare1 == Mpint32(0u));
        out0 = proofShares[0].GetQueryAnswer(queries[1]);

        // Verifier compress their own input vector.
        const size_t nPoly = ceil(op0.size() / (double)compressFactor);
        Mpint32* const resizedInput = new Mpint32[nPoly * compressFactor];
        std::memset(resizedInput, 0, (nPoly * compressFactor) * sizeof(Mpint32));
        std::memcpy(resizedInput, op0.data(), op0.size() * sizeof(Mpint32));
        std::vector<Mpint32Poly> poly0s;
        poly0s.reserve(nPoly);
        for (size_t i = 0; i < nPoly; ++i)
        {
            poly0s.emplace_back(
                Mpint32Poly::LagrangeInterpolation(resizedInput + compressFactor * i, compressFactor));
        }
        op0 = proof.EvaluatePolyPs(random);
        delete[] resizedInput;

        endVerifier = chrono::high_resolution_clock::now();
        verifierTime += chrono::duration_cast<chrono::nanoseconds>(endVerifier - startVerifier).count();


        // Verifier 2
        out1 = proofShares[1].GetQueryAnswer(queries[1]);


        // Communication - Verifier sends random value to prover.
        LANTime += GetLANDelay(sizeof(Mpint32));
        WANTime += GetWANDelay(sizeof(Mpint32));


        // Prover
        startProver = chrono::high_resolution_clock::now();
        op0 = proof.EvaluatePolyPs(random);
        op1 = proof.EvaluatePolyQs(random);
        endProver = chrono::high_resolution_clock::now();
        proverTime += chrono::duration_cast<chrono::nanoseconds>(endProver - startProver).count();
    }
    else
    {
        // Prover
        auto start = chrono::high_resolution_clock::now();
        Proof proof = InnerProductCircuit::MakeProof(op0.data(), op1.data(), op0.size(), op0.size());
        std::vector<Proof> proofShares = proof.GetShares(2);
        auto end = chrono::high_resolution_clock::now();
        proverTime += chrono::duration_cast<chrono::nanoseconds>(end - start).count();


        // Communication
        // Prover send two shares of proof to two verifiers respectively.
        LANTime += GetLANDelay(proof.GetBytes());
        WANTime += GetWANDelay(proof.GetBytes());
        // Exchange random value between verifiers.
        LANTime += 2u * GetLANDelay(sizeof(Mpint32));
        WANTime += 2u * GetWANDelay(sizeof(Mpint32));

        std::vector<Mpint32> randomsInConstantTerms = proof.GetRandoms(2);


        // Verifier 1
        start = chrono::high_resolution_clock::now();
        Mpint32 random = Mpint32::GenerateRandom();
        std::vector<Query> queries = InnerProductCircuit::MakeQuery(random, op0.size(), op0.size());

        Mpint32* const resizedInput0 = new Mpint32[op0.size() + 1u];
        std::memset(resizedInput0, 0, (op0.size() + 1u) * sizeof(Mpint32));
        *resizedInput0 = randomsInConstantTerms[0];
        std::memcpy(resizedInput0 + 1u, op0.data(), op0.size() * sizeof(Mpint32));
        Mpint32Poly poly0;
        poly0 = Mpint32Poly::LagrangeInterpolation(resizedInput0, op0.size() + 1u);
        Mpint32 pR = poly0.Evaluate(random);
        delete[] resizedInput0;

        Mpint32 varificationShare0 = proofShares[0].GetQueryAnswer(queries[queries.size() - 2u]);
        Mpint32 resultShare0 = proofShares[0].GetQueryAnswer(queries[queries.size() - 1u]) - out0;
        end = chrono::high_resolution_clock::now();
        verifierTime += chrono::duration_cast<chrono::nanoseconds>(end - start).count();


        // Verifier 2
        Mpint32* const resizedInput1 = new Mpint32[op0.size() + 1u];
        std::memset(resizedInput1, 0, (op0.size() + 1u) * sizeof(Mpint32));
        *resizedInput1 = randomsInConstantTerms[1];
        std::memcpy(resizedInput1 + 1u, op1.data(), op1.size() * sizeof(Mpint32));
        Mpint32Poly poly1;
        poly1 = Mpint32Poly::LagrangeInterpolation(resizedInput1, op0.size() + 1u);
        Mpint32 qR = poly1.Evaluate(random);
        delete[] resizedInput1;

        Mpint32 varificationShare1 = proofShares[1].GetQueryAnswer(queries[queries.size() - 2u]);
        Mpint32 resultShare1 = proofShares[1].GetQueryAnswer(queries[queries.size() - 1u]) - out1;


        // Communication - Verifiers share their pR, qR, res0, res1, var0, var1
        LANTime += 2u * GetLANDelay(3 * sizeof(Mpint32));
        WANTime += 2u * GetWANDelay(3 * sizeof(Mpint32));


        // Verifiers
        start = chrono::high_resolution_clock::now();
        isValid = isValid && (Mpint32(0) == resultShare0 + resultShare1) &&
                  (varificationShare0 + varificationShare1 == pR * qR);
        end = chrono::high_resolution_clock::now();
        verifierTime += chrono::duration_cast<chrono::nanoseconds>(end - start).count();
    }

    if (!isValid)
    {
        exit(-1); // FLIOP is aborted!!
    }

    return OneRoundMeasurement(proverTime, verifierTime, LANTime, WANTime);
}

OneRoundMeasurement ThreePC::SimulateFLIOPCoefficientOneRound(size_t inputLength, size_t compressFactor)
{
    Mpint32::SetSeed(10u);

    std::vector<Mpint32> op0(inputLength);
    std::fill(op0.begin(), op0.end(), Mpint32(0));
    std::vector<Mpint32> op1(inputLength);
    std::fill(op1.begin(), op1.end(), Mpint32(0));

    double proverTime = 0.;
    double verifierTime = 0.;
    double LANTime = 0.;
    double WANTime = 0.;

    bool isValid = true;
    Mpint32 out0 = InnerProductCircuit::Forward(op0.data(), op1.data(), op0.size());
    Mpint32 out1 = Mpint32(0u);
    if (ceil(op0.size() / (double)compressFactor) > 1)
    {
        // Prover
        auto start_prover = chrono::high_resolution_clock::now();
        InteractiveProof proof =
            InnerProductCircuit::MakeRoundCoefficientProof(op0.data(), op1.data(), op0.size(), compressFactor);
        std::vector<Proof> proofShares = proof.GetShares(2);
        auto end_prover = chrono::high_resolution_clock::now();
        proverTime += chrono::duration_cast<chrono::nanoseconds>(end_prover - start_prover).count();


        // Communication
        // Prover send two shares of proof to two verifiers respectively.
        LANTime += GetLANDelay(proof.GetBytes());
        WANTime += GetWANDelay(proof.GetBytes());
        // Exchange random value between verifiers.
        LANTime += 2u * GetLANDelay(sizeof(Mpint32));
        WANTime += 2u * GetWANDelay(sizeof(Mpint32));


        // Verifier 1
        auto start_verifier = chrono::high_resolution_clock::now();
        Mpint32 random = Mpint32::GenerateRandom(); // Actually it's combination of two exchanged random values.
        std::vector<Query> queries = InnerProductCircuit::MakeRoundCoefficientQuery(random, compressFactor);
        Mpint32 outShare0 = proofShares[0].GetQueryAnswer(queries[0]) - out0;
        auto end_verifier = chrono::high_resolution_clock::now();
        verifierTime += chrono::duration_cast<chrono::nanoseconds>(end_verifier - start_verifier).count();


        // Verifier 2
        Mpint32 outShare1 = proofShares[1].GetQueryAnswer(queries[0]) - out1;


        // Communication - Verifiers send their own shares of 'out' value.
        LANTime += 2u * GetLANDelay(sizeof(Mpint32));
        WANTime += 2u * GetWANDelay(sizeof(Mpint32));


        // Verifier 1
        start_verifier = chrono::high_resolution_clock::now();
        isValid = isValid && (outShare0 + outShare1 == Mpint32(0u));
        out0 = proofShares[0].GetQueryAnswer(queries[1]);

        // Verifiers compress their own input vector.
        const size_t nPoly = ceil(op0.size() / (double)compressFactor);
        Mpint32* const resizedInput = new Mpint32[nPoly * compressFactor];
        std::memset(resizedInput, 0, (nPoly * compressFactor) * sizeof(Mpint32));
        std::memcpy(resizedInput, op0.data(), op0.size() * sizeof(Mpint32));
        std::vector<Mpint32Poly> poly0s;
        poly0s.reserve(nPoly);
        for (size_t i = 0; i < nPoly; ++i)
        {
            poly0s.emplace_back(resizedInput + i * compressFactor, compressFactor, true);
        }
        op0 = proof.EvaluatePolyPs(random);
        delete[] resizedInput;

        end_verifier = chrono::high_resolution_clock::now();
        verifierTime += chrono::duration_cast<chrono::nanoseconds>(end_verifier - start_verifier).count();


        // Verifier 2
        out1 = proofShares[1].GetQueryAnswer(queries[1]);


        // Communication - Verifier sends random value to prover.
        LANTime += GetLANDelay(sizeof(Mpint32));
        WANTime += GetWANDelay(sizeof(Mpint32));


        // Prover
        start_prover = chrono::high_resolution_clock::now();
        op0 = proof.EvaluatePolyPs(random);
        op1 = proof.EvaluatePolyQs(random);
        end_prover = chrono::high_resolution_clock::now();
        proverTime += chrono::duration_cast<chrono::nanoseconds>(end_prover - start_prover).count();
    }
    else
    {
        // Prover
        auto start = chrono::high_resolution_clock::now();
        Proof proof = InnerProductCircuit::MakeCoefficientProof(op0.data(), op1.data(), op0.size(), 1);
        std::vector<Proof> proofShares = proof.GetShares(2);
        auto end = chrono::high_resolution_clock::now();
        proverTime += chrono::duration_cast<chrono::nanoseconds>(end - start).count();


        // Communication
        // Prover send two shares of proof to two verifiers respectively.
        LANTime += GetLANDelay(proof.GetBytes());
        WANTime += GetWANDelay(proof.GetBytes());
        // Exchange random value between verifiers.
        LANTime += 2u * GetLANDelay(sizeof(Mpint32));
        WANTime += 2u * GetWANDelay(sizeof(Mpint32));

        std::vector<Mpint32> randomsInConstantTerms = proof.GetRandoms(2);


        // Verifier 1
        start = chrono::high_resolution_clock::now();
        Mpint32 random = Mpint32::GenerateRandom();
        std::vector<Query> queries = InnerProductCircuit::MakeCoefficientQuery(random, op0.size(), 1);

        Mpint32* const resizedInput0 = new Mpint32[op0.size() + 1u];
        std::memset(resizedInput0, 0, (op0.size() + 1u) * sizeof(Mpint32));
        *resizedInput0 = randomsInConstantTerms[0];
        std::memcpy(resizedInput0 + 1u, op0.data(), op0.size() * sizeof(Mpint32));
        Mpint32Poly poly0(resizedInput0, op0.size() + 1u, true);
        Mpint32 pR = poly0.Evaluate(random);
        delete[] resizedInput0;

        Mpint32 varificationShare0 = proofShares[0].GetQueryAnswer(queries[queries.size() - 2u]);
        Mpint32 resultShare0 = proofShares[0].GetQueryAnswer(queries[queries.size() - 1u]) - out0;
        end = chrono::high_resolution_clock::now();
        verifierTime += chrono::duration_cast<chrono::nanoseconds>(end - start).count();


        // Verifier 2
        Mpint32* const resizedInput1 = new Mpint32[op0.size() + 1u];
        std::memset(resizedInput1, 0, (op0.size() + 1u) * sizeof(Mpint32));
        *resizedInput1 = randomsInConstantTerms[1];
        std::memcpy(resizedInput1 + 1u, op1.data(), op1.size() * sizeof(Mpint32));
        for (size_t i = 0; i < op0.size() / 2u; ++i)
        {
            Mpint32 temp = resizedInput1[i + 1];
            resizedInput1[i + 1] = resizedInput1[op0.size() - i];
            resizedInput1[op0.size() - i] = temp;
        }
        Mpint32Poly poly1(resizedInput1, op1.size() + 1u, true);
        Mpint32 qR = poly1.Evaluate(random);
        delete[] resizedInput1;

        Mpint32 varificationShare1 = proofShares[1].GetQueryAnswer(queries[queries.size() - 2u]);
        Mpint32 resultShare1 = proofShares[1].GetQueryAnswer(queries[queries.size() - 1u]) - out1;


        // Communication - Verifiers share their pR, qR, res0, res1, var0, var1
        LANTime += 2u * GetLANDelay(3 * sizeof(Mpint32));
        WANTime += 2u * GetWANDelay(3 * sizeof(Mpint32));


        // Verifiers
        start = chrono::high_resolution_clock::now();
        isValid = isValid && (Mpint32(0) == resultShare0 + resultShare1) && (varificationShare0 + varificationShare1 == pR * qR);
        end = chrono::high_resolution_clock::now();
        verifierTime += chrono::duration_cast<chrono::nanoseconds>(end - start).count();
    }

    if (!isValid)
    {
        exit(-1); // FLIOP is aborted!!
    }

    return OneRoundMeasurement(proverTime, verifierTime, LANTime, WANTime);
}

BestSchedule ThreePC::FindBestFLIOPScheduleRecursive(const size_t inputLength, double* totalTimes,
                                               const size_t maxLambda, const size_t maxLength)
{
    if (inputLength == 2u)
    {
        std::vector<size_t> lambdas(1);
        lambdas[0] = 2u;
        return BestSchedule(totalTimes[inputLength + maxLength * 2u], lambdas);
    }

    double totalMin = DBL_MAX;
    std::vector<size_t> bestLambdas;
    size_t maxCompress = min(maxLambda, inputLength);
    for (size_t i = 2u; i <= maxCompress; ++i)
    {
        BestSchedule best =
            FindBestFLIOPScheduleRecursive(ceil(inputLength / (double)i), totalTimes, maxLambda, maxLength);
        double currMin = totalTimes[inputLength + maxLength * i] + best.time;
        if (totalMin > currMin)
        {
            totalMin = currMin;
            bestLambdas = best.lambdas;
            bestLambdas.push_back(i);
        }
    }

    return BestSchedule(totalMin, bestLambdas);
}

double ThreePC::FindFLIOPDelayRecursive(const size_t inputLength, double* totalTimes, const size_t maxLength)
{
    if (inputLength == 2u)
    {
        return totalTimes[inputLength + maxLength * 2u];
    }
    double best = FindFLIOPDelayRecursive(ceil(inputLength / (double)2), totalTimes, maxLength);
    return totalTimes[inputLength + maxLength * 2] + best;
}

void ThreePC::FindBestFLIOPSchedule(const size_t inputLength, const size_t maxLambda)
{
    double* totalLANTimes = new double[(inputLength + 1u) * (maxLambda + 1u)];
    double* totalWANTimes = new double[(inputLength + 1u) * (maxLambda + 1u)];

    for (size_t i = 2u; i <= inputLength; ++i)
    {
        size_t maxCompress = min(maxLambda, i);
        for (size_t j = 2u; j <= maxCompress; ++j)
        {
            OneRoundMeasurement measure = SimulateFLIOPOneRound(i, j);
            double common = measure.proverTimeNs + measure.verifierTimeNs;
            totalLANTimes[i + inputLength * j] = (common + measure.communicationTimeNsInLAN) * 1e-6;
            totalWANTimes[i + inputLength * j] = (common + measure.communicationTimeNsInWAN) * 1e-6;
        }
    }

    size_t j = 0;
    double outputs[10];

    cout << "LAN Min schedule" << endl;
    for (size_t i = 2u; i <= inputLength; i *= 2u)
    {
        BestSchedule best = FindBestFLIOPScheduleRecursive(i, totalLANTimes, maxLambda, inputLength);
        cout << "Length: " << i;
        cout << " / Min time : " << std::fixed << best.time << std::setprecision(9) << " / Best schedule : ";
        outputs[j++] = best.time;
        std::reverse(best.lambdas.begin(), best.lambdas.end());
        for (size_t j = 0; j < best.lambdas.size(); ++j)
        {
            cout << best.lambdas[j] << " ";
        }
        cout << endl;
    }
    for (size_t i = 0; i < 10; ++i)
    {
        cout << std::fixed << outputs[i] << std::setprecision(9) << ", ";
    }
    cout << endl;

    j = 0;
    cout << "WAN Min schedule" << endl;
    for (size_t i = 2u; i <= inputLength; i *= 2u)
    {
        BestSchedule best = FindBestFLIOPScheduleRecursive(i, totalWANTimes, maxLambda, inputLength);
        cout << "Length: " << i;
        cout << " / Min time : " << std::fixed << best.time << std::setprecision(9) << " / Best schedule : ";
        outputs[j++] = best.time;
        std::reverse(best.lambdas.begin(), best.lambdas.end());
        for (size_t j = 0; j < best.lambdas.size(); ++j)
        {
            cout << best.lambdas[j] << " ";
        }
        cout << endl;
    }
    for (size_t i = 0; i < 10; ++i)
    {
        cout << std::fixed << outputs[i] << std::setprecision(9) << ", ";
    }
    cout << endl;

    j = 0;
    cout << "FLIOP LAN Delay" << endl;
    for (size_t i = 2u; i <= inputLength; i *= 2u)
    {
        double fliopTime = FindFLIOPDelayRecursive(i, totalLANTimes, inputLength);
        cout << "Length: " << i << " / Time : " << std::fixed << fliopTime << std::setprecision(9) << endl;
        outputs[j++] = fliopTime;
    }
    for (size_t i = 0; i < 10; ++i)
    {
        cout << std::fixed << outputs[i] << std::setprecision(9) << ", ";
    }
    cout << endl;

    j = 0;
    cout << "FLIOP WAN Delay" << endl;
    for (size_t i = 2u; i <= inputLength; i *= 2u)
    {
        double fliopTime = FindFLIOPDelayRecursive(i, totalWANTimes, inputLength);
        cout << "Length: " << i << " / Time : " << std::fixed << fliopTime << std::setprecision(9) << endl;
        outputs[j++] = fliopTime;
    }
    for (size_t i = 0; i < 10; ++i)
    {
        cout << std::fixed << outputs[i] << std::setprecision(9) << ", ";
    }
    cout << endl;
   
    delete[] totalLANTimes;
    delete[] totalWANTimes;
}


void ThreePC::FindBestFLIOPCoefficientSchedule(const size_t inputLength, const size_t maxLambda)
{
    double* totalLANTimes = new double[(inputLength + 1u) * (maxLambda + 1u)];
    double* totalWANTimes = new double[(inputLength + 1u) * (maxLambda + 1u)];

    for (size_t i = 2u; i <= inputLength; ++i)
    {
        size_t maxCompress = min(maxLambda, i);
        for (size_t j = 2u; j <= maxCompress; ++j)
        {
            OneRoundMeasurement measure = SimulateFLIOPCoefficientOneRound(i, j);
            double common = measure.proverTimeNs + measure.verifierTimeNs;
            totalLANTimes[i + inputLength * j] = (common + measure.communicationTimeNsInLAN) * 1e-6;
            totalWANTimes[i + inputLength * j] = (common + measure.communicationTimeNsInWAN) * 1e-6;
        }
    }

    size_t j = 0;
    double outputs[10];

    cout << "LAN Min schedule" << endl;
    for (size_t i = 2u; i <= inputLength; i *= 2u)
    {
        BestSchedule best = FindBestFLIOPScheduleRecursive(i, totalLANTimes, maxLambda, inputLength);
        cout << "Length: " << i;
        cout << " / Min time : " << std::fixed << best.time << std::setprecision(9) << " / Best schedule : ";
        outputs[j++] = best.time;
        std::reverse(best.lambdas.begin(), best.lambdas.end());
        for (size_t j = 0; j < best.lambdas.size(); ++j)
        {
            cout << best.lambdas[j] << " ";
        }
        cout << endl;
    }
    for (size_t i = 0; i < 10; ++i)
    {
        cout << std::fixed << outputs[i] << std::setprecision(9) << ", ";
    }
    cout << endl;

    j = 0;
    cout << "WAN Min schedule" << endl;
    for (size_t i = 2u; i <= inputLength; i *= 2u)
    {
        BestSchedule best = FindBestFLIOPScheduleRecursive(i, totalWANTimes, maxLambda, inputLength);
        cout << "Length: " << i;
        cout << " / Min time : " << std::fixed << best.time << std::setprecision(9) << " / Best schedule : ";
        outputs[j++] = best.time;
        std::reverse(best.lambdas.begin(), best.lambdas.end());
        for (size_t j = 0; j < best.lambdas.size(); ++j)
        {
            cout << best.lambdas[j] << " ";
        }
        cout << endl;
    }
    for (size_t i = 0; i < 10; ++i)
    {
        cout << std::fixed << outputs[i] << std::setprecision(9) << ", ";
    }
    cout << endl;

    j = 0;
    cout << "FLIOP LAN Delay" << endl;
    for (size_t i = 2u; i <= inputLength; i *= 2u)
    {
        double fliopTime = FindFLIOPDelayRecursive(i, totalLANTimes, inputLength);
        cout << "Length: " << i << " / Time : " << std::fixed << fliopTime << std::setprecision(9) << endl;
        outputs[j++] = fliopTime;
    }
    for (size_t i = 0; i < 10; ++i)
    {
        cout << std::fixed << outputs[i] << std::setprecision(9) << ", ";
    }
    cout << endl;

    j = 0;
    cout << "FLIOP WAN Delay" << endl;
    for (size_t i = 2u; i <= inputLength; i *= 2u)
    {
        double fliopTime = FindFLIOPDelayRecursive(i, totalWANTimes, inputLength);
        cout << "Length: " << i << " / Time : " << std::fixed << fliopTime << std::setprecision(9) << endl;
        outputs[j++] = fliopTime;
    }
    for (size_t i = 0; i < 10; ++i)
    {
        cout << std::fixed << outputs[i] << std::setprecision(9) << ", ";
    }
    cout << endl;

    delete[] totalLANTimes;
    delete[] totalWANTimes;
}