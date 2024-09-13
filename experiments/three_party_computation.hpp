#ifndef THREE_PARTY_COMPUTATION_H
#define THREE_PARTY_COMPUTATION_H

#include <algorithm>
#include <cassert>
#include <cfloat>
#include <chrono>
#include <iostream>
#include <vector>

#include "network.hpp"
#include "..\circuit\inner_product_circuit.hpp"
#include "..\unit\proof.hpp"

struct OneRoundMeasurement
{
    double proverTimeNs;
    double verifierTimeNs;
    double communicationTimeNsInLAN;
    double communicationTimeNsInWAN;
};

struct BestSchedule
{
    double time;
    std::vector<size_t> lambdas;
};

template <typename Int> class ThreePC
{
public:
    static void FindBestFLIOPSchedule(const size_t inputLength, const size_t maxLambda);
    static void FindBestFLIOPCoefficientSchedule(const size_t inputLength, const size_t maxLambda);

private:
    static OneRoundMeasurement SimulateFLIOPOneRound(size_t inputLength, size_t compressFactor);
    static OneRoundMeasurement SimulateFLIOPCoefficientOneRound(size_t inputLength, size_t compressFactor);
    static BestSchedule FindBestFLIOPScheduleRecursive(const size_t inputLength, double* totalTimes,
                                                       const size_t maxLambda, const size_t maxLength);
    static double FindFLIOPDelayRecursive(const size_t inputLength, double* totalTimes, const size_t maxLength);
};

template <typename Int>
OneRoundMeasurement ThreePC<Int>::SimulateFLIOPOneRound(size_t inputLength, size_t compressFactor)
{
    Int::SetSeed(10u);

    std::vector<Int> op0(inputLength);
    std::fill(op0.begin(), op0.end(), Int(0u));
    std::vector<Int> op1(inputLength);
    std::fill(op1.begin(), op1.end(), Int(0u));

    double proverTime = 0.;
    double verifierTime = 0.;
    double LANTime = 0.;
    double WANTime = 0.;

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
    Int out0 = InnerProductCircuit<Int>::Forward(op0.data(), op1.data(), op0.size());
    Int out1 = Int(0u);
    if (ceil(op0.size() / (double)compressFactor) > 1)
    {
        // Prover
        auto startProver = chrono::high_resolution_clock::now();
        InteractiveProof<Int> proof = InnerProductCircuit<Int>::MakeRoundProofWithPrecompute(
            op0.data(), op1.data(), op0.size(), compressFactor, evalToCoeff);
        std::vector<Proof<Int>> proofShares = proof.GetShares(2);
        auto endProver = chrono::high_resolution_clock::now();
        proverTime += chrono::duration_cast<chrono::nanoseconds>(endProver - startProver).count();

        // Communication
        // Prover send two shares of proof to two verifiers respectively.
        LANTime += Network::GetLANPayloadDelay(proof.GetBytes());
        WANTime += Network::GetWANPayloadDelay(proof.GetBytes());
        // Exchange random value between verifiers.
        LANTime += 2u * Network::GetLANPayloadDelay(sizeof(Int));
        WANTime += 2u * Network::GetWANPayloadDelay(sizeof(Int));

        // Verifier 1
        auto startVerifier = chrono::high_resolution_clock::now();
        Int random = Int::GenerateRandom(); // Actually it's combination of two exchanged random values.
        std::vector<Query<Int>> queries = InnerProductCircuit<Int>::MakeRoundQuery(random, compressFactor);
        Int outShare0 = proofShares[0].GetQueryAnswer(queries[0]) - out0;
        auto endVerifier = chrono::high_resolution_clock::now();
        verifierTime += chrono::duration_cast<chrono::nanoseconds>(endVerifier - startVerifier).count();

        // Verifier 2
        Int outShare1 = proofShares[1].GetQueryAnswer(queries[0]) - out1;

        // Communication - Verifiers send their own shares of 'out' value.
        LANTime += 2u * Network::GetLANDelay(sizeof(Int));
        WANTime += 2u * Network::GetWANDelay(sizeof(Int));

        // Verifier 1
        startVerifier = chrono::high_resolution_clock::now();
        isValid = isValid && (outShare0 + outShare1 == Int(0u));
        out0 = proofShares[0].GetQueryAnswer(queries[1]);

        // Verifier compress their own input vector.
        const size_t nPoly = ceil(op0.size() / (double)compressFactor);
        Int* const resizedInput = new Int[nPoly * compressFactor];
        std::memset(resizedInput, 0, (nPoly * compressFactor) * sizeof(Int));
        std::memcpy(resizedInput, op0.data(), op0.size() * sizeof(Int));
        std::vector<Polynomial<Int>> poly0s;
        poly0s.reserve(nPoly);
        for (size_t i = 0; i < nPoly; ++i)
        {
            poly0s.emplace_back(
                Polynomial<Int>::LagrangeInterpolation(resizedInput + compressFactor * i, compressFactor));
        }
        op0 = proof.EvaluatePolyPs(random);
        delete[] resizedInput;

        endVerifier = chrono::high_resolution_clock::now();
        verifierTime += chrono::duration_cast<chrono::nanoseconds>(endVerifier - startVerifier).count();

        // Verifier 2
        out1 = proofShares[1].GetQueryAnswer(queries[1]);

        // Communication - Verifier sends random value to prover.
        LANTime += Network::GetLANDelay(sizeof(Int));
        WANTime += Network::GetWANDelay(sizeof(Int));

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
        Proof proof = InnerProductCircuit<Int>::MakeProof(op0.data(), op1.data(), op0.size(), op0.size());
        std::vector<Proof<Int>> proofShares = proof.GetShares(2);
        auto end = chrono::high_resolution_clock::now();
        proverTime += chrono::duration_cast<chrono::nanoseconds>(end - start).count();

        // Communication
        // Prover send two shares of proof to two verifiers respectively.
        LANTime += Network::GetLANDelay(proof.GetBytes());
        WANTime += Network::GetWANDelay(proof.GetBytes());
        // Exchange random value between verifiers.
        LANTime += 2u * Network::GetLANDelay(sizeof(Int));
        WANTime += 2u * Network::GetWANDelay(sizeof(Int));

        std::vector<Int> randomsInConstantTerms = proof.GetRandoms(2);

        // Verifier 1
        start = chrono::high_resolution_clock::now();
        Int random = Int::GenerateRandom();
        std::vector<Query<Int>> queries = InnerProductCircuit<Int>::MakeQuery(random, op0.size(), op0.size());

        Int* const resizedInput0 = new Int[op0.size() + 1u];
        std::memset(resizedInput0, 0, (op0.size() + 1u) * sizeof(Int));
        *resizedInput0 = randomsInConstantTerms[0];
        std::memcpy(resizedInput0 + 1u, op0.data(), op0.size() * sizeof(Int));
        Polynomial<Int> poly0;
        poly0 = Polynomial<Int>::LagrangeInterpolation(resizedInput0, op0.size() + 1u);
        Int pR = poly0.Evaluate(random);
        delete[] resizedInput0;

        Int varificationShare0 = proofShares[0].GetQueryAnswer(queries[queries.size() - 2u]);
        Int resultShare0 = proofShares[0].GetQueryAnswer(queries[queries.size() - 1u]) - out0;
        end = chrono::high_resolution_clock::now();
        verifierTime += chrono::duration_cast<chrono::nanoseconds>(end - start).count();

        // Verifier 2
        Int* const resizedInput1 = new Int[op0.size() + 1u];
        std::memset(resizedInput1, 0, (op0.size() + 1u) * sizeof(Int));
        *resizedInput1 = randomsInConstantTerms[1];
        std::memcpy(resizedInput1 + 1u, op1.data(), op1.size() * sizeof(Int));
        Polynomial<Int> poly1;
        poly1 = Polynomial<Int>::LagrangeInterpolation(resizedInput1, op0.size() + 1u);
        Int qR = poly1.Evaluate(random);
        delete[] resizedInput1;

        Int varificationShare1 = proofShares[1].GetQueryAnswer(queries[queries.size() - 2u]);
        Int resultShare1 = proofShares[1].GetQueryAnswer(queries[queries.size() - 1u]) - out1;

        // Communication - Verifiers share their pR, qR, res0, res1, var0, var1
        LANTime += 2u * Network::GetLANDelay(3 * sizeof(Int));
        WANTime += 2u * Network::GetWANDelay(3 * sizeof(Int));

        // Verifiers
        start = chrono::high_resolution_clock::now();
        isValid =
            isValid && (Int(0) == resultShare0 + resultShare1) && (varificationShare0 + varificationShare1 == pR * qR);
        end = chrono::high_resolution_clock::now();
        verifierTime += chrono::duration_cast<chrono::nanoseconds>(end - start).count();
    }

    if (!isValid)
    {
        exit(-1); // FLIOP is aborted!!
    }

    return OneRoundMeasurement(proverTime, verifierTime, LANTime, WANTime);
}

template <typename Int>
OneRoundMeasurement ThreePC<Int>::SimulateFLIOPCoefficientOneRound(size_t inputLength, size_t compressFactor)
{
    Int::SetSeed(10u);

    std::vector<Int> op0(inputLength);
    std::fill(op0.begin(), op0.end(), Int(0));
    std::vector<Int> op1(inputLength);
    std::fill(op1.begin(), op1.end(), Int(0));

    double proverTime = 0.;
    double verifierTime = 0.;
    double LANTime = 0.;
    double WANTime = 0.;

    bool isValid = true;
    Int out0 = InnerProductCircuit<Int>::Forward(op0.data(), op1.data(), op0.size());
    Int out1 = Int(0u);
    if (ceil(op0.size() / (double)compressFactor) > 1)
    {
        // Prover
        auto start_prover = chrono::high_resolution_clock::now();
        InteractiveProof<Int> proof =
            InnerProductCircuit<Int>::MakeRoundCoefficientProof(op0.data(), op1.data(), op0.size(), compressFactor);
        std::vector<Proof<Int>> proofShares = proof.GetShares(2);
        auto end_prover = chrono::high_resolution_clock::now();
        proverTime += chrono::duration_cast<chrono::nanoseconds>(end_prover - start_prover).count();

        // Communication
        // Prover send two shares of proof to two verifiers respectively.
        LANTime += Network::GetLANDelay(proof.GetBytes());
        WANTime += Network::GetWANDelay(proof.GetBytes());
        // Exchange random value between verifiers.
        LANTime += 2u * Network::GetLANDelay(sizeof(Int));
        WANTime += 2u * Network::GetWANDelay(sizeof(Int));

        // Verifier 1
        auto start_verifier = chrono::high_resolution_clock::now();
        Int random = Int::GenerateRandom(); // Actually it's combination of two exchanged random values.
        std::vector<Query<Int>> queries = InnerProductCircuit<Int>::MakeRoundCoefficientQuery(random, compressFactor);
        Int outShare0 = proofShares[0].GetQueryAnswer(queries[0]) - out0;
        auto end_verifier = chrono::high_resolution_clock::now();
        verifierTime += chrono::duration_cast<chrono::nanoseconds>(end_verifier - start_verifier).count();

        // Verifier 2
        Int outShare1 = proofShares[1].GetQueryAnswer(queries[0]) - out1;

        // Communication - Verifiers send their own shares of 'out' value.
        LANTime += 2u * Network::GetLANDelay(sizeof(Int));
        WANTime += 2u * Network::GetWANDelay(sizeof(Int));

        // Verifier 1
        start_verifier = chrono::high_resolution_clock::now();
        isValid = isValid && (outShare0 + outShare1 == Int(0u));
        out0 = proofShares[0].GetQueryAnswer(queries[1]);

        // Verifiers compress their own input vector.
        const size_t nPoly = ceil(op0.size() / (double)compressFactor);
        Int* const resizedInput = new Int[nPoly * compressFactor];
        std::memset(resizedInput, 0, (nPoly * compressFactor) * sizeof(Int));
        std::memcpy(resizedInput, op0.data(), op0.size() * sizeof(Int));
        std::vector<Polynomial<Int>> poly0s;
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
        LANTime += Network::GetLANDelay(sizeof(Int));
        WANTime += Network::GetWANDelay(sizeof(Int));

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
        Proof<Int> proof = InnerProductCircuit<Int>::MakeCoefficientProof(op0.data(), op1.data(), op0.size(), 1);
        std::vector<Proof<Int>> proofShares = proof.GetShares(2);
        auto end = chrono::high_resolution_clock::now();
        proverTime += chrono::duration_cast<chrono::nanoseconds>(end - start).count();

        // Communication
        // Prover send two shares of proof to two verifiers respectively.
        LANTime += Network::GetLANDelay(proof.GetBytes());
        WANTime += Network::GetWANDelay(proof.GetBytes());
        // Exchange random value between verifiers.
        LANTime += 2u * Network::GetLANDelay(sizeof(Int));
        WANTime += 2u * Network::GetWANDelay(sizeof(Int));

        std::vector<Int> randomsInConstantTerms = proof.GetRandoms(2);

        // Verifier 1
        start = chrono::high_resolution_clock::now();
        Int random = Int::GenerateRandom();
        std::vector<Query<Int>> queries = InnerProductCircuit<Int>::MakeCoefficientQuery(random, op0.size(), 1);

        Int* const resizedInput0 = new Int[op0.size() + 1u];
        std::memset(resizedInput0, 0, (op0.size() + 1u) * sizeof(Int));
        *resizedInput0 = randomsInConstantTerms[0];
        std::memcpy(resizedInput0 + 1u, op0.data(), op0.size() * sizeof(Int));
        Polynomial<Int> poly0(resizedInput0, op0.size() + 1u, true);
        Int pR = poly0.Evaluate(random);
        delete[] resizedInput0;

        Int varificationShare0 = proofShares[0].GetQueryAnswer(queries[queries.size() - 2u]);
        Int resultShare0 = proofShares[0].GetQueryAnswer(queries[queries.size() - 1u]) - out0;
        end = chrono::high_resolution_clock::now();
        verifierTime += chrono::duration_cast<chrono::nanoseconds>(end - start).count();

        // Verifier 2
        Int* const resizedInput1 = new Int[op0.size() + 1u];
        std::memset(resizedInput1, 0, (op0.size() + 1u) * sizeof(Int));
        *resizedInput1 = randomsInConstantTerms[1];
        std::memcpy(resizedInput1 + 1u, op1.data(), op1.size() * sizeof(Int));
        for (size_t i = 0; i < op0.size() / 2u; ++i)
        {
            Int temp = resizedInput1[i + 1];
            resizedInput1[i + 1] = resizedInput1[op0.size() - i];
            resizedInput1[op0.size() - i] = temp;
        }
        Polynomial<Int> poly1(resizedInput1, op1.size() + 1u, true);
        Int qR = poly1.Evaluate(random);
        delete[] resizedInput1;

        Int varificationShare1 = proofShares[1].GetQueryAnswer(queries[queries.size() - 2u]);
        Int resultShare1 = proofShares[1].GetQueryAnswer(queries[queries.size() - 1u]) - out1;

        // Communication - Verifiers share their pR, qR, res0, res1, var0, var1
        LANTime += 2u * Network::GetLANDelay(3 * sizeof(Int));
        WANTime += 2u * Network::GetWANDelay(3 * sizeof(Int));

        // Verifiers
        start = chrono::high_resolution_clock::now();
        isValid =
            isValid && (Int(0) == resultShare0 + resultShare1) && (varificationShare0 + varificationShare1 == pR * qR);
        end = chrono::high_resolution_clock::now();
        verifierTime += chrono::duration_cast<chrono::nanoseconds>(end - start).count();
    }

    if (!isValid)
    {
        exit(-1); // FLIOP is aborted!!
    }

    return OneRoundMeasurement(proverTime, verifierTime, LANTime, WANTime);
}

template <typename Int>
BestSchedule ThreePC<Int>::FindBestFLIOPScheduleRecursive(const size_t inputLength, double* totalTimes,
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

template <typename Int>
double ThreePC<Int>::FindFLIOPDelayRecursive(const size_t inputLength, double* totalTimes, const size_t maxLength)
{
    if (inputLength == 2u)
    {
        return totalTimes[inputLength + maxLength * 2u];
    }
    double best = FindFLIOPDelayRecursive(ceil(inputLength / (double)2), totalTimes, maxLength);
    return totalTimes[inputLength + maxLength * 2] + best;
}

template <typename Int> void ThreePC<Int>::FindBestFLIOPSchedule(const size_t inputLength, const size_t maxLambda)
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

template <typename Int>
void ThreePC<Int>::FindBestFLIOPCoefficientSchedule(const size_t inputLength, const size_t maxLambda)
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


#endif
