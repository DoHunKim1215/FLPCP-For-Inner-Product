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

template <typename Int> class ThreePC
{
public:
    static void FindBestFLIOPSchedule(const size_t inputLength, const size_t maxLambda);
    static void FindBestFLIOPCoefficientSchedule(const size_t inputLength, const size_t maxLambda);

private:
    static OneRoundMeasurement SimulateFLIOPOneRound(size_t inputLength, size_t compressFactor);
    static OneRoundMeasurement SimulateFLIOPCoefficientOneRound(size_t inputLength, size_t compressFactor);
    static IOPSchedule FindBestLANScheduleRecursive(const size_t inputLength, double* totalTimes,
                                                       const size_t maxLambda, const size_t maxLength);
    static double FindFLIOPDelayRecursive(const size_t inputLength, double* totalTimes, const size_t maxLength);
};

template <typename Int>
OneRoundMeasurement ThreePC<Int>::SimulateFLIOPOneRound(size_t inputLength, size_t compressFactor)
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

    unsigned char secretKey[64] = {0};

    bool isValid = true;
    Int out0 = InnerProductCircuit<Int>::Forward(op0.data(), op1.data(), op0.size());
    Int out1 = Int((uint64_t)0);
    if (ceil(op0.size() / (double)compressFactor) > 1)
    {
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
        InteractiveProof<Int> proof = InnerProductCircuit<Int>::MakeRoundProofWithPrecompute(
            op0.data(), op1.data(), op0.size(), compressFactor, evalToCoeff);
        std::vector<Proof<Int>> proofShares = proof.GetShares(2);

        Int* randoms = new Int[2];
        *randoms = proofShares[0].GetRandomFromOracle(secretKey, 64);
        *(randoms + 1) = proofShares[1].GetRandomFromOracle(secretKey, 64);
        SHA512_CTX ctx;
        unsigned char digest[SHA512_DIGEST_LENGTH];
        SHA512_Init(&ctx);
        SHA512_Update(&ctx, randoms, 2 * sizeof(Int));
        SHA512_Final(digest, &ctx);
        Int totalRandom = Int(digest);

        auto end = std::chrono::high_resolution_clock::now();
        proverTime += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();


        // Communication
        LANTime += Network::GetLANPayloadDelay(proof.GetBytes() + 2 * sizeof(Int));
        WANTime += Network::GetWANPayloadDelay(proof.GetBytes() + 2 * sizeof(Int));


        // Verifier 1
        start = std::chrono::high_resolution_clock::now();
        std::vector<Query<Int>> queries = InnerProductCircuit<Int>::MakeRoundQuery(totalRandom, compressFactor);
        Int outShare0 = proofShares[0].GetQueryAnswer(queries[0]) - out0;
        isValid = isValid && (*randoms == proofShares[0].GetRandomFromOracle(secretKey, 64));
        end = std::chrono::high_resolution_clock::now();
        verifierTime += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

        // Verifier 2
        Int outShare1 = proofShares[1].GetQueryAnswer(queries[0]) - out1;
        isValid = isValid && (*(randoms + 1) == proofShares[1].GetRandomFromOracle(secretKey, 64));

        // Communication - Verifiers send their own shares of 'out' value.
        LANTime += 2u * Network::GetLANPayloadDelay(sizeof(Int));
        WANTime += 2u * Network::GetWANPayloadDelay(sizeof(Int));

        // Verifier 1
        start = std::chrono::high_resolution_clock::now();
        isValid = isValid && (outShare0 + outShare1 == Int((uint64_t)0));

        SHA512_CTX vctx;
        unsigned char vdigest[SHA512_DIGEST_LENGTH];
        SHA512_Init(&vctx);
        SHA512_Update(&vctx, randoms, 2 * sizeof(Int));
        SHA512_Final(vdigest, &vctx);
        Int totalRandomOfVerifiers = Int(vdigest);
        isValid = isValid && (totalRandom == totalRandomOfVerifiers);

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
            poly0s.emplace_back(Polynomial<Int>::VandermondeInterpolation(resizedInput + compressFactor * i,
                                                                          compressFactor, evalToCoeff));
        }
        op0 = proof.EvaluatePolyPs(totalRandom);
        delete[] resizedInput;

        end = std::chrono::high_resolution_clock::now();
        verifierTime += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

        // Verifier 2
        out1 = proofShares[1].GetQueryAnswer(queries[1]);

        // Prover
        start = std::chrono::high_resolution_clock::now();
        op0 = proof.EvaluatePolyPs(totalRandom);
        op1 = proof.EvaluatePolyQs(totalRandom);
        end = std::chrono::high_resolution_clock::now();
        proverTime += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

        delete[] randoms;
    }
    else
    {
        // Precompute a Vandermonde matrix
        Int* xs = new Int[op0.size() + 1];
        for (size_t i = 0; i < op0.size() + 1; ++i)
        {
            xs[i] = Int(i);
        }
        SquareMatrix<Int> evalToCoeff = SquareMatrix<Int>::GetVandermonde(xs, op0.size() + 1);
        evalToCoeff.Inverse();
        delete[] xs;
        xs = (Int*)0;

        // Prover
        auto start = std::chrono::high_resolution_clock::now();
        Proof proof = InnerProductCircuit<Int>::MakeProofWithPrecompute(op0.data(), op1.data(), op0.size(), op0.size(),
                                                                        evalToCoeff);
        std::vector<Proof<Int>> proofShares = proof.GetShares(2);

        Int* randoms = new Int[2];
        *randoms = proofShares[0].GetRandomFromOracle(secretKey, 64);
        *(randoms + 1) = proofShares[1].GetRandomFromOracle(secretKey, 64);
        SHA512_CTX ctx;
        unsigned char digest[SHA512_DIGEST_LENGTH];
        SHA512_Init(&ctx);
        SHA512_Update(&ctx, randoms, 2 * sizeof(Int));
        SHA512_Final(digest, &ctx);
        Int totalRandom = Int(digest);

        auto end = std::chrono::high_resolution_clock::now();
        proverTime += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();


        // Communication
        LANTime += Network::GetLANPayloadDelay(proof.GetBytes() + 2u * sizeof(Int));
        WANTime += Network::GetWANPayloadDelay(proof.GetBytes() + 2u * sizeof(Int));

        std::vector<Int> randomsInConstantTerms = proof.GetRandoms(2);


        // Verifier 1
        start = std::chrono::high_resolution_clock::now();
        std::vector<Query<Int>> queries = InnerProductCircuit<Int>::MakeQuery(totalRandom, op0.size(), op0.size());
        isValid = isValid && (*randoms == proofShares[0].GetRandomFromOracle(secretKey, 64));

        Int* const resizedInput0 = new Int[op0.size() + 1u];
        std::memset(resizedInput0, 0, (op0.size() + 1u) * sizeof(Int));
        *resizedInput0 = randomsInConstantTerms[0];
        std::memcpy(resizedInput0 + 1u, op0.data(), op0.size() * sizeof(Int));
        Polynomial<Int> poly0;
        poly0 = Polynomial<Int>::VandermondeInterpolation(resizedInput0, op0.size() + 1u, evalToCoeff);
        Int pR = poly0.Evaluate(totalRandom);
        delete[] resizedInput0;

        Int varificationShare0 = proofShares[0].GetQueryAnswer(queries[queries.size() - 2u]);
        Int resultShare0 = proofShares[0].GetQueryAnswer(queries[queries.size() - 1u]) - out0;
        end = std::chrono::high_resolution_clock::now();
        verifierTime += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

        // Verifier 2
        isValid = isValid && (*(randoms + 1) == proofShares[1].GetRandomFromOracle(secretKey, 64));

        Int* const resizedInput1 = new Int[op0.size() + 1u];
        std::memset(resizedInput1, 0, (op0.size() + 1u) * sizeof(Int));
        *resizedInput1 = randomsInConstantTerms[1];
        std::memcpy(resizedInput1 + 1u, op1.data(), op1.size() * sizeof(Int));
        Polynomial<Int> poly1;
        poly1 = Polynomial<Int>::VandermondeInterpolation(resizedInput1, op0.size() + 1u, evalToCoeff);
        Int qR = poly1.Evaluate(totalRandom);
        delete[] resizedInput1;

        Int varificationShare1 = proofShares[1].GetQueryAnswer(queries[queries.size() - 2u]);
        Int resultShare1 = proofShares[1].GetQueryAnswer(queries[queries.size() - 1u]) - out1;

        // Communication - Verifiers share their pR, qR, res0, res1, var0, var1
        LANTime += Network::GetLANPayloadDelay(4 * sizeof(Int));
        WANTime += Network::GetWANPayloadDelay(4 * sizeof(Int));

        // Verifiers
        start = std::chrono::high_resolution_clock::now();

        isValid = isValid && (Int((uint64_t)0) == resultShare0 + resultShare1) &&
                  (varificationShare0 + varificationShare1 == pR * qR);

        SHA512_CTX vctx;
        unsigned char vdigest[SHA512_DIGEST_LENGTH];
        SHA512_Init(&vctx);
        SHA512_Update(&vctx, randoms, 2 * sizeof(Int));
        SHA512_Final(vdigest, &vctx);
        Int totalRandomOfVerifiers = Int(vdigest);
        isValid = isValid && (totalRandom == totalRandomOfVerifiers);

        end = std::chrono::high_resolution_clock::now();
        verifierTime += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

        delete[] randoms;
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
    std::fill(op0.begin(), op0.end(), Int((uint64_t)0));
    std::vector<Int> op1(inputLength);
    std::fill(op1.begin(), op1.end(), Int((uint64_t)0));

    double proverTime = 0.;
    double verifierTime = 0.;
    double LANTime = 0.;
    double WANTime = 0.;
    
    unsigned char secretKey[64] = {0};

    bool isValid = true;
    Int out0 = InnerProductCircuit<Int>::Forward(op0.data(), op1.data(), op0.size());
    Int out1 = Int((uint64_t)0);
    if (ceil(op0.size() / (double)compressFactor) > 1)
    {
        // Prover
        auto start_prover = std::chrono::high_resolution_clock::now();
        InteractiveProof<Int> proof =
            InnerProductCircuit<Int>::MakeRoundCoefficientProof(op0.data(), op1.data(), op0.size(), compressFactor);
        std::vector<Proof<Int>> proofShares = proof.GetShares(2);

        Int* randoms = new Int[2];
        *randoms = proofShares[0].GetRandomFromOracle(secretKey, 64);
        *(randoms + 1) = proofShares[1].GetRandomFromOracle(secretKey, 64);
        SHA512_CTX ctx;
        unsigned char digest[SHA512_DIGEST_LENGTH];
        SHA512_Init(&ctx);
        SHA512_Update(&ctx, randoms, 2 * sizeof(Int));
        SHA512_Final(digest, &ctx);
        Int totalRandom = Int(digest);

        auto end_prover = std::chrono::high_resolution_clock::now();
        proverTime += std::chrono::duration_cast<std::chrono::nanoseconds>(end_prover - start_prover).count();

        // Communication
        LANTime += Network::GetLANPayloadDelay(proof.GetBytes() + 2u * sizeof(Int));
        WANTime += Network::GetWANPayloadDelay(proof.GetBytes() + 2u * sizeof(Int));

        // Verifier 1
        auto start_verifier = std::chrono::high_resolution_clock::now();
        std::vector<Query<Int>> queries =
            InnerProductCircuit<Int>::MakeRoundCoefficientQuery(totalRandom, compressFactor);
        Int outShare0 = proofShares[0].GetQueryAnswer(queries[0]) - out0;
        isValid = isValid && (*randoms == proofShares[0].GetRandomFromOracle(secretKey, 64));
        auto end_verifier = std::chrono::high_resolution_clock::now();
        verifierTime += std::chrono::duration_cast<std::chrono::nanoseconds>(end_verifier - start_verifier).count();

        // Verifier 2
        Int outShare1 = proofShares[1].GetQueryAnswer(queries[0]) - out1;
        isValid = isValid && (*(randoms + 1) == proofShares[1].GetRandomFromOracle(secretKey, 64));

        // Communication - Verifiers send their own shares of 'out' value.
        LANTime += Network::GetLANPayloadDelay(2u * sizeof(Int));
        WANTime += Network::GetWANPayloadDelay(2u * sizeof(Int));

        // Verifier 1
        start_verifier = std::chrono::high_resolution_clock::now();
        isValid = isValid && (outShare0 + outShare1 == Int((uint64_t)0));
        out0 = proofShares[0].GetQueryAnswer(queries[1]);

        SHA512_CTX vctx;
        unsigned char vdigest[SHA512_DIGEST_LENGTH];
        SHA512_Init(&vctx);
        SHA512_Update(&vctx, randoms, 2 * sizeof(Int));
        SHA512_Final(vdigest, &vctx);
        Int totalRandomOfVerifiers = Int(vdigest);
        isValid = isValid && (totalRandom == totalRandomOfVerifiers);

        // Verifiers compress their own input vector.
        const size_t nPoly = (size_t)ceil(op0.size() / (double)compressFactor);
        Int* const resizedInput = new Int[nPoly * compressFactor];
        std::memset(resizedInput, 0, (nPoly * compressFactor) * sizeof(Int));
        std::memcpy(resizedInput, op0.data(), op0.size() * sizeof(Int));
        std::vector<Polynomial<Int>> poly0s;
        poly0s.reserve(nPoly);
        for (size_t i = 0; i < nPoly; ++i)
        {
            poly0s.emplace_back(resizedInput + i * compressFactor, compressFactor, true);
        }
        op0 = proof.EvaluatePolyPs(totalRandom);
        delete[] resizedInput;

        end_verifier = std::chrono::high_resolution_clock::now();
        verifierTime += std::chrono::duration_cast<std::chrono::nanoseconds>(end_verifier - start_verifier).count();

        // Verifier 2
        out1 = proofShares[1].GetQueryAnswer(queries[1]);

        // Prover
        start_prover = std::chrono::high_resolution_clock::now();
        op0 = proof.EvaluatePolyPs(totalRandom);
        op1 = proof.EvaluatePolyQs(totalRandom);
        end_prover = std::chrono::high_resolution_clock::now();
        proverTime += std::chrono::duration_cast<std::chrono::nanoseconds>(end_prover - start_prover).count();

        delete[] randoms;
    }
    else
    {
        // Prover
        auto start = std::chrono::high_resolution_clock::now();
        Proof<Int> proof = InnerProductCircuit<Int>::MakeCoefficientProof(op0.data(), op1.data(), op0.size(), 1);
        std::vector<Proof<Int>> proofShares = proof.GetShares(2);

        Int* randoms = new Int[2];
        *randoms = proofShares[0].GetRandomFromOracle(secretKey, 64);
        *(randoms + 1) = proofShares[1].GetRandomFromOracle(secretKey, 64);
        SHA512_CTX ctx;
        unsigned char digest[SHA512_DIGEST_LENGTH];
        SHA512_Init(&ctx);
        SHA512_Update(&ctx, randoms, 2 * sizeof(Int));
        SHA512_Final(digest, &ctx);
        Int totalRandom = Int(digest);

        auto end = std::chrono::high_resolution_clock::now();
        proverTime += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();


        // Communication
        LANTime += Network::GetLANPayloadDelay(proof.GetBytes() + 2 * sizeof(Int));
        WANTime += Network::GetWANPayloadDelay(proof.GetBytes() + 2 * sizeof(Int));

        std::vector<Int> randomsInConstantTerms = proof.GetRandoms(2);


        // Verifier 1
        start = std::chrono::high_resolution_clock::now();
        std::vector<Query<Int>> queries = InnerProductCircuit<Int>::MakeCoefficientQuery(totalRandom, op0.size(), 1);
        isValid = isValid && (*randoms == proofShares[0].GetRandomFromOracle(secretKey, 64));

        Int* const resizedInput0 = new Int[op0.size() + 1u];
        std::memset(resizedInput0, 0, (op0.size() + 1u) * sizeof(Int));
        *resizedInput0 = randomsInConstantTerms[0];
        std::memcpy(resizedInput0 + 1u, op0.data(), op0.size() * sizeof(Int));
        Polynomial<Int> poly0(resizedInput0, op0.size() + 1u, true);
        Int pR = poly0.Evaluate(totalRandom);
        delete[] resizedInput0;

        Int varificationShare0 = proofShares[0].GetQueryAnswer(queries[queries.size() - 2u]);
        Int resultShare0 = proofShares[0].GetQueryAnswer(queries[queries.size() - 1u]) - out0;
        end = std::chrono::high_resolution_clock::now();
        verifierTime += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

        // Verifier 2
        isValid = isValid && (*(randoms + 1) == proofShares[1].GetRandomFromOracle(secretKey, 64));

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
        Int qR = poly1.Evaluate(totalRandom);
        delete[] resizedInput1;

        Int varificationShare1 = proofShares[1].GetQueryAnswer(queries[queries.size() - 2u]);
        Int resultShare1 = proofShares[1].GetQueryAnswer(queries[queries.size() - 1u]) - out1;

        // Communication - Verifiers share their pR, qR, res0, res1, var0, var1
        LANTime += Network::GetLANPayloadDelay(4 * sizeof(Int));
        WANTime += Network::GetWANPayloadDelay(4 * sizeof(Int));

        // Verifiers
        start = std::chrono::high_resolution_clock::now();

        isValid = isValid && (Int((uint64_t)0) == resultShare0 + resultShare1) &&
                  (varificationShare0 + varificationShare1 == pR * qR);

        SHA512_CTX vctx;
        unsigned char vdigest[SHA512_DIGEST_LENGTH];
        SHA512_Init(&vctx);
        SHA512_Update(&vctx, randoms, 2 * sizeof(Int));
        SHA512_Final(vdigest, &vctx);
        Int totalRandomOfVerifiers = Int(vdigest);
        isValid = isValid && (totalRandom == totalRandomOfVerifiers);

        end = std::chrono::high_resolution_clock::now();
        verifierTime += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

        delete[] randoms;
    }

    if (!isValid)
    {
        exit(-1); // FLIOP is aborted!!
    }

    return OneRoundMeasurement(proverTime, verifierTime, LANTime, WANTime);
}

template <typename Int>
IOPSchedule ThreePC<Int>::FindBestLANScheduleRecursive(const size_t inputLength, double* totalTimes,
                                                          const size_t maxLambda, const size_t maxLength)
{
    if (inputLength == 2u)
    {
        std::vector<size_t> lambdas(1);
        lambdas[0] = 2u;
        return IOPSchedule(totalTimes[inputLength + maxLength * 2u], lambdas);
    }

    double totalMin = DBL_MAX;
    std::vector<size_t> bestLambdas;
    size_t maxCompress = std::min(maxLambda, inputLength);
    for (size_t i = 2u; i <= maxCompress; ++i)
    {
        IOPSchedule best =
            FindBestLANScheduleRecursive(ceil(inputLength / (double)i), totalTimes, maxLambda, maxLength);
        double currMin = totalTimes[inputLength + maxLength * i] + best.time;
        if (totalMin > currMin)
        {
            totalMin = currMin;
            bestLambdas = best.lambdas;
            bestLambdas.push_back(i);
        }
    }

    return IOPSchedule(totalMin, bestLambdas);
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
        size_t maxCompress = std::min(maxLambda, i);
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

    std::cout << "LAN Min schedule" << std::endl;
    for (size_t i = 2u; i <= inputLength; i *= 2u)
    {
        IOPSchedule best = FindBestLANScheduleRecursive(i, totalLANTimes, maxLambda, inputLength);
        std::cout << "Length: " << i;
        std::cout << " / Min time : " << std::fixed << best.time + Network::LANBaseDelayMs * 2 << std::setprecision(9)
                  << " / Best schedule : ";
        outputs[j++] = best.time + Network::LANBaseDelayMs * 2;
        std::reverse(best.lambdas.begin(), best.lambdas.end());
        for (size_t j = 0; j < best.lambdas.size(); ++j)
        {
            std::cout << best.lambdas[j] << " ";
        }
        std::cout << std::endl;
    }
    for (size_t i = 0; i < 10; ++i)
    {
        std::cout << std::fixed << outputs[i] << std::setprecision(9) << ", ";
    }
    std::cout << std::endl;

    j = 0;
    std::cout << "WAN Min schedule" << std::endl;
    for (size_t i = 2u; i <= inputLength; i *= 2u)
    {
        IOPSchedule best = FindBestLANScheduleRecursive(i, totalWANTimes, maxLambda, inputLength);
        std::cout << "Length: " << i;
        std::cout << " / Min time : " << std::fixed << best.time + Network::WANBaseDelayMs * 2 << std::setprecision(9)
                  << " / Best schedule : ";
        outputs[j++] = best.time + Network::WANBaseDelayMs * 2;
        std::reverse(best.lambdas.begin(), best.lambdas.end());
        for (size_t j = 0; j < best.lambdas.size(); ++j)
        {
            std::cout << best.lambdas[j] << " ";
        }
        std::cout << std::endl;
    }
    for (size_t i = 0; i < 10; ++i)
    {
        std::cout << std::fixed << outputs[i] << std::setprecision(9) << ", ";
    }
    std::cout << std::endl;

    j = 0;
    std::cout << "FLIOP LAN Delay" << std::endl;
    for (size_t i = 2u; i <= inputLength; i *= 2u)
    {
        double fliopTime = FindFLIOPDelayRecursive(i, totalLANTimes, inputLength);
        std::cout << "Length: " << i << " / Time : " << std::fixed << fliopTime + Network::LANBaseDelayMs * 2
                  << std::setprecision(9) << std::endl;
        outputs[j++] = fliopTime + Network::LANBaseDelayMs * 2;
    }
    for (size_t i = 0; i < 10; ++i)
    {
        std::cout << std::fixed << outputs[i] << std::setprecision(9) << ", ";
    }
    std::cout << std::endl;

    j = 0;
    std::cout << "FLIOP WAN Delay" << std::endl;
    for (size_t i = 2u; i <= inputLength; i *= 2u)
    {
        double fliopTime = FindFLIOPDelayRecursive(i, totalWANTimes, inputLength);
        std::cout << "Length: " << i << " / Time : " << std::fixed << fliopTime + Network::WANBaseDelayMs * 2
                  << std::setprecision(9) << std::endl;
        outputs[j++] = fliopTime + Network::WANBaseDelayMs * 2;
    }
    for (size_t i = 0; i < 10; ++i)
    {
        std::cout << std::fixed << outputs[i] << std::setprecision(9) << ", ";
    }
    std::cout << std::endl;

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
        size_t maxCompress = std::min(maxLambda, i);
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

    std::cout << "LAN Min schedule" << std::endl;
    for (size_t i = 2u; i <= inputLength; i *= 2u)
    {
        IOPSchedule best = FindBestLANScheduleRecursive(i, totalLANTimes, maxLambda, inputLength);
        std::cout << "Length: " << i;
        std::cout << " / Min time : " << std::fixed << best.time + Network::LANBaseDelayMs * 2 << std::setprecision(9)
                  << " / Best schedule : ";
        outputs[j++] = best.time + Network::LANBaseDelayMs * 2;
        std::reverse(best.lambdas.begin(), best.lambdas.end());
        for (size_t j = 0; j < best.lambdas.size(); ++j)
        {
            std::cout << best.lambdas[j] << " ";
        }
        std::cout << std::endl;
    }
    for (size_t i = 0; i < 10; ++i)
    {
        std::cout << std::fixed << outputs[i] << std::setprecision(9) << ", ";
    }
    std::cout << std::endl;

    j = 0;
    std::cout << "WAN Min schedule" << std::endl;
    for (size_t i = 2u; i <= inputLength; i *= 2u)
    {
        IOPSchedule best = FindBestLANScheduleRecursive(i, totalWANTimes, maxLambda, inputLength);
        std::cout << "Length: " << i;
        std::cout << " / Min time : " << std::fixed << best.time + Network::WANBaseDelayMs * 2 << std::setprecision(9)
                  << " / Best schedule : ";
        outputs[j++] = best.time + Network::WANBaseDelayMs * 2;
        std::reverse(best.lambdas.begin(), best.lambdas.end());
        for (size_t j = 0; j < best.lambdas.size(); ++j)
        {
            std::cout << best.lambdas[j] << " ";
        }
        std::cout << std::endl;
    }
    for (size_t i = 0; i < 10; ++i)
    {
        std::cout << std::fixed << outputs[i] << std::setprecision(9) << ", ";
    }
    std::cout << std::endl;

    j = 0;
    std::cout << "FLIOP LAN Delay" << std::endl;
    for (size_t i = 2u; i <= inputLength; i *= 2u)
    {
        double fliopTime = FindFLIOPDelayRecursive(i, totalLANTimes, inputLength);
        std::cout << "Length: " << i << " / Time : " << std::fixed << fliopTime + Network::LANBaseDelayMs * 2
                  << std::setprecision(9) << std::endl;
        outputs[j++] = fliopTime + Network::LANBaseDelayMs * 2;
    }
    for (size_t i = 0; i < 10; ++i)
    {
        std::cout << std::fixed << outputs[i] << std::setprecision(9) << ", ";
    }
    std::cout << std::endl;

    j = 0;
    std::cout << "FLIOP WAN Delay" << std::endl;
    for (size_t i = 2u; i <= inputLength; i *= 2u)
    {
        double fliopTime = FindFLIOPDelayRecursive(i, totalWANTimes, inputLength);
        std::cout << "Length: " << i << " / Time : " << std::fixed << fliopTime + Network::WANBaseDelayMs * 2
                  << std::setprecision(9) << std::endl;
        outputs[j++] = fliopTime + Network::WANBaseDelayMs * 2;
    }
    for (size_t i = 0; i < 10; ++i)
    {
        std::cout << std::fixed << outputs[i] << std::setprecision(9) << ", ";
    }
    std::cout << std::endl;

    delete[] totalLANTimes;
    delete[] totalWANTimes;
}

#endif
