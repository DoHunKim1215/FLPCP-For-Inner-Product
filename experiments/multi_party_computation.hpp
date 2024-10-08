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

template <typename Int> class MPC
{
public:
    MPC(const uint32_t seed, const size_t inputLength, const size_t maxLambda, const size_t nParties);
    ~MPC();

    void FindBestFLIOPSchedule(bool coefficient = false);

private:
    size_t mInputLength;
    size_t mMaxLambda;
    size_t mNParties;
    uint32_t mSeed;

    // (mMaxLambda + 1) x (mInputLength + 1)
    OneRoundMeasurement** mOneRoundMeasures;
    double** mTotalLANTimes;
    double** mTotalWANTimes;

    OneRoundMeasurement SimulateFLIOPOneRound(size_t inputLength, size_t compressFactor);
    OneRoundMeasurement SimulateFLIOPCoefficientOneRound(size_t inputLength, size_t compressFactor);
    void CalculateOneRoundTimesRecursive(const size_t inputLength);
    void CalculateOneCoefficientRoundTimesRecursive(const size_t inputLength);
    IOPSchedule FindBestLANSchedule(const size_t inputLength);
    IOPSchedule FindBestLANScheduleRecursive(const size_t inputLength);
    IOPSchedule FindBestWANSchedule(const size_t inputLength);
    IOPSchedule FindBestWANScheduleRecursive(const size_t inputLength);
    IOPSchedule FindLANDelay(const size_t inputLength);
    IOPSchedule FindLANDelayRecursive(const size_t inputLength);
    IOPSchedule FindWANDelay(const size_t inputLength);
    IOPSchedule FindWANDelayRecursive(const size_t inputLength);
};

template <typename Int>
MPC<Int>::MPC(const uint32_t seed, const size_t inputLength, const size_t maxLambda, const size_t nParties)
{
    assert(inputLength >= 2 && maxLambda >= 2 && nParties >= 3);

    mSeed = seed;
    mInputLength = inputLength;
    mMaxLambda = maxLambda;
    mNParties = nParties;

    mOneRoundMeasures = new OneRoundMeasurement*[mMaxLambda + 1];
    mTotalLANTimes = new double*[mMaxLambda + 1];
    mTotalWANTimes = new double*[mMaxLambda + 1];
    for (size_t i = 0; i < mMaxLambda + 1; ++i)
    {
        mOneRoundMeasures[i] = new OneRoundMeasurement[mInputLength + 1];
        std::memset(mOneRoundMeasures[i], 0, (mInputLength + 1) * sizeof(OneRoundMeasurement));
        mTotalLANTimes[i] = new double[mInputLength + 1];
        std::memset(mTotalLANTimes[i], 0, (mInputLength + 1) * sizeof(double));
        mTotalWANTimes[i] = new double[mInputLength + 1];
        std::memset(mTotalWANTimes[i], 0, (mInputLength + 1) * sizeof(double));
    }
}

template <typename Int> MPC<Int>::~MPC()
{
    for (size_t i = 0; i < mMaxLambda + 1; ++i)
    {
        delete[] mOneRoundMeasures[i];
        delete[] mTotalLANTimes[i];
        delete[] mTotalWANTimes[i];
    }
    delete[] mOneRoundMeasures;
    delete[] mTotalLANTimes;
    delete[] mTotalWANTimes;
}

template <typename Int>
OneRoundMeasurement MPC<Int>::SimulateFLIOPOneRound(size_t inputLength, size_t compressFactor)
{
    Int::SetSeed(mSeed);

    std::vector<Int> op0(inputLength);
    std::vector<Int> op1(inputLength);
    std::memset(op0.data(), 1, inputLength * sizeof(Int));
    std::memset(op1.data(), 1, inputLength * sizeof(Int));

    std::vector<Int> op0Share(inputLength);
    std::vector<Int> op1Share(inputLength);
    std::memset(op0Share.data(), 0, inputLength * sizeof(Int));
    std::memset(op1Share.data(), 0, inputLength * sizeof(Int));

    double proverTime = 0.;
    double verifierTime = 0.;
    double LANTime = 0.;
    double WANTime = 0.;

    // In practice, the verifiers may have different keys.
    // But this simulation assume all secret keys are 0's for simplicity.
    // Assumption : this secret keys are already shared before the verification.
    unsigned char secretKey[64] = {0};

    const size_t nVerifiers = mNParties - 1;

    bool isValid = true;
    Int* outShares = new Int[nVerifiers];
    outShares[0] = InnerProductCircuit<Int>::Forward(op0.data(), op1.data(), op0.size());
    for (size_t i = 1; i < nVerifiers; ++i)
    {
        outShares[i] = Int((uint64_t)0);
    }

    if (ceil(op0.size() / (double)compressFactor) > 1)
    {
        // Precompute a Vandermonde matrix
        SquareMatrix<Int> evalToCoeff = SquareMatrix<Int>::GetVandermondeInverse(compressFactor);

        // Prover
        auto start = std::chrono::high_resolution_clock::now();
        InteractiveProof<Int> proof = InnerProductCircuit<Int>::MakeRoundProofWithPrecompute(
            op0.data(), op1.data(), op0.size(), compressFactor, evalToCoeff);
        std::vector<Proof<Int>> proofShares = proof.GetShares(nVerifiers);

        // Verifier-specific random value generation
        Int* randoms = new Int[nVerifiers];
        for (size_t i = 0; i < nVerifiers; ++i)
        {
            randoms[i] = proofShares[i].GetRandomFromOracle(secretKey, 64);
        }
        SHA512_CTX ctx;
        unsigned char digest[SHA512_DIGEST_LENGTH];
        SHA512_Init(&ctx);
        SHA512_Update(&ctx, randoms, nVerifiers * sizeof(Int));
        SHA512_Final(digest, &ctx);
        Int commonRandom = Int(digest);

        auto end = std::chrono::high_resolution_clock::now();
        proverTime += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();


        // Communication : (proof, verifier-specific random, common random)
        // Assumption : the transmissions to multiple verifiers simultaneously occur.
        LANTime += Network::GetLANPayloadDelay(proof.GetBytes() + 2 * sizeof(Int));
        WANTime += Network::GetWANPayloadDelay(proof.GetBytes() + 2 * sizeof(Int));

        Int* verificationShares = new Int[nVerifiers];

        // First verifier
        start = std::chrono::high_resolution_clock::now();
        std::vector<Query<Int>> queries = InnerProductCircuit<Int>::MakeRoundQuery(commonRandom, compressFactor);
        verificationShares[0] = proofShares[0].GetQueryAnswer(queries[0]) - outShares[0];
        outShares[0] = proofShares[0].GetQueryAnswer(queries[1]);
        isValid = isValid && (randoms[0] == proofShares[0].GetRandomFromOracle(secretKey, 64));
        end = std::chrono::high_resolution_clock::now();
        verifierTime += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

        // The other verifiers
        for (size_t i = 1; i < nVerifiers; ++i)
        {
            verificationShares[i] = proofShares[i].GetQueryAnswer(queries[0]) - outShares[i];
            outShares[i] = proofShares[i].GetQueryAnswer(queries[1]);
            isValid = isValid && (randoms[i] == proofShares[i].GetRandomFromOracle(secretKey, 64));
        }
        

        // Communication - Verifiers send their own 'verificationShares' value, its own random.
        LANTime += Network::GetLANPayloadDelay(2 * sizeof(Int));
        WANTime += Network::GetWANPayloadDelay(2 * sizeof(Int));


        // Collector (one of the verifiers)
        start = std::chrono::high_resolution_clock::now();
        Int verificationValue((uint64_t)0);
        for (size_t i = 0; i < nVerifiers; ++i)
        {
            verificationValue += verificationShares[i];
        }
        isValid = isValid && (verificationValue == Int((uint64_t)0));

        SHA512_CTX vctx;
        unsigned char vdigest[SHA512_DIGEST_LENGTH];
        SHA512_Init(&vctx);
        SHA512_Update(&vctx, randoms, nVerifiers * sizeof(Int));
        SHA512_Final(vdigest, &vctx);
        Int totalRandomOfVerifiers = Int(vdigest);
        isValid = isValid && (commonRandom == totalRandomOfVerifiers);

        // Verifier compress their own input vector.
        const size_t nPoly0 = ceil(op0.size() / (double)compressFactor);
        Int* const resizedInput0 = new Int[nPoly0 * compressFactor];
        std::memset(resizedInput0, 0, (nPoly0 * compressFactor) * sizeof(Int));
        std::memcpy(resizedInput0, op0.data(), op0.size() * sizeof(Int));
        std::vector<Polynomial<Int>> poly0s;
        poly0s.reserve(nPoly0);
        for (size_t i = 0; i < nPoly0; ++i)
        {
            poly0s.emplace_back(Polynomial<Int>::VandermondeInterpolation(resizedInput0 + compressFactor * i,
                                                                          compressFactor, evalToCoeff));
        }
        op0 = proof.EvaluatePolyPs(commonRandom);
        delete[] resizedInput0;

        const size_t nPoly1 = ceil(op1.size() / (double)compressFactor);
        Int* const resizedInput1 = new Int[nPoly1 * compressFactor];
        std::memset(resizedInput1, 0, (nPoly1 * compressFactor) * sizeof(Int));
        std::memcpy(resizedInput1, op1.data(), op1.size() * sizeof(Int));
        std::vector<Polynomial<Int>> poly1s;
        poly1s.reserve(nPoly1);
        for (size_t i = 0; i < nPoly1; ++i)
        {
            poly1s.emplace_back(Polynomial<Int>::VandermondeInterpolation(resizedInput1 + compressFactor * i,
                                                                          compressFactor, evalToCoeff));
        }
        op1 = proof.EvaluatePolyPs(commonRandom);
        delete[] resizedInput1;

        end = std::chrono::high_resolution_clock::now();
        verifierTime += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

        // Prover
        start = std::chrono::high_resolution_clock::now();
        op0 = proof.EvaluatePolyPs(commonRandom);
        op1 = proof.EvaluatePolyQs(commonRandom);
        end = std::chrono::high_resolution_clock::now();
        proverTime += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
        
        delete[] verificationShares;
        delete[] randoms;
    }
    else
    {
        // Precompute a Vandermonde matrix
        SquareMatrix<Int> evalToCoeff = SquareMatrix<Int>::GetVandermondeInverse(op0.size() + 1);

        // Prover
        auto start = std::chrono::high_resolution_clock::now();
        Proof proof = InnerProductCircuit<Int>::MakeProofWithPrecompute(op0.data(), op1.data(), op0.size(), op0.size(),
                                                                        evalToCoeff);
        std::vector<Proof<Int>> proofShares = proof.GetShares(nVerifiers);

        Int* randoms = new Int[nVerifiers];
        for (size_t i = 0; i < nVerifiers; ++i)
        {
            randoms[i] = proofShares[i].GetRandomFromOracle(secretKey, 64);
        }
        SHA512_CTX ctx;
        unsigned char digest[SHA512_DIGEST_LENGTH];
        SHA512_Init(&ctx);
        SHA512_Update(&ctx, randoms, nVerifiers * sizeof(Int));
        SHA512_Final(digest, &ctx);
        Int commonRandom = Int(digest);

        auto end = std::chrono::high_resolution_clock::now();
        proverTime += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();


        // Communication
        LANTime += Network::GetLANPayloadDelay(proof.GetBytes() + 4 * sizeof(Int));
        WANTime += Network::GetWANPayloadDelay(proof.GetBytes() + 4 * sizeof(Int));

        std::vector<Int> randomsInConstantTerms = proof.GetRandoms(2);
        Int* verificationShares = new Int[nVerifiers];
        Int* resultShares = new Int[nVerifiers];
        Int* evaluationShares = new Int[nVerifiers * 2];


        // First verifier
        start = std::chrono::high_resolution_clock::now();
        std::vector<Query<Int>> queries = InnerProductCircuit<Int>::MakeQuery(commonRandom, op0.size(), op0.size());
        isValid = isValid && (randoms[0] == proofShares[0].GetRandomFromOracle(secretKey, 64));

        Int* const resizedInput0 = new Int[op0.size() + 1];
        std::memset(resizedInput0, 0, (op0.size() + 1) * sizeof(Int));
        *resizedInput0 = randomsInConstantTerms[0];
        std::memcpy(resizedInput0 + 1, op0.data(), op0.size() * sizeof(Int));
        Polynomial<Int> poly0;
        poly0 = Polynomial<Int>::VandermondeInterpolation(resizedInput0, op0.size() + 1, evalToCoeff);
        evaluationShares[0] = poly0.Evaluate(commonRandom);
        delete[] resizedInput0;

        Int* const resizedInput1 = new Int[op1.size() + 1];
        std::memset(resizedInput1, 0, (op1.size() + 1) * sizeof(Int));
        *resizedInput1 = randomsInConstantTerms[1];
        std::memcpy(resizedInput1 + 1, op1.data(), op1.size() * sizeof(Int));
        Polynomial<Int> poly1;
        poly1 = Polynomial<Int>::VandermondeInterpolation(resizedInput1, op1.size() + 1, evalToCoeff);
        evaluationShares[nVerifiers] = poly1.Evaluate(commonRandom);
        delete[] resizedInput1;

        verificationShares[0] = proofShares[0].GetQueryAnswer(queries[queries.size() - 2]);
        resultShares[0] = proofShares[0].GetQueryAnswer(queries[queries.size() - 1]) - outShares[0];
        end = std::chrono::high_resolution_clock::now();
        verifierTime += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

        // The other verifiers
        for (size_t i = 1; i < nVerifiers; ++i)
        {
            isValid = isValid && (randoms[i] == proofShares[i].GetRandomFromOracle(secretKey, 64));

            Int* const resizedInput0 = new Int[op0Share.size() + 1];
            std::memset(resizedInput0, 0, (op0Share.size() + 1) * sizeof(Int));
            *resizedInput0 = Int((uint64_t)0);
            std::memcpy(resizedInput0 + 1, op0Share.data(), op0Share.size() * sizeof(Int));
            Polynomial<Int> poly0;
            poly0 = Polynomial<Int>::VandermondeInterpolation(resizedInput0, op0Share.size() + 1, evalToCoeff);
            evaluationShares[i] = poly0.Evaluate(commonRandom);
            delete[] resizedInput0;

            Int* const resizedInput1 = new Int[op1Share.size() + 1];
            std::memset(resizedInput1, 0, (op1Share.size() + 1) * sizeof(Int));
            *resizedInput1 = Int((uint64_t)0);
            std::memcpy(resizedInput1 + 1, op1Share.data(), op1Share.size() * sizeof(Int));
            Polynomial<Int> poly1;
            poly1 = Polynomial<Int>::VandermondeInterpolation(resizedInput1, op1Share.size() + 1, evalToCoeff);
            evaluationShares[i + nVerifiers] = poly1.Evaluate(commonRandom);
            delete[] resizedInput1;

            verificationShares[i] = proofShares[i].GetQueryAnswer(queries[queries.size() - 2]);
            resultShares[i] = proofShares[i].GetQueryAnswer(queries[queries.size() - 1]) - outShares[i];
        }

        // Communication - Verifiers share two evaluationShares, one varificationShare,
        // one resultShare, private random.
        LANTime += Network::GetLANPayloadDelay(5 * sizeof(Int));
        WANTime += Network::GetWANPayloadDelay(5 * sizeof(Int));

        // Collector
        start = std::chrono::high_resolution_clock::now();
        Int verificationValue((uint64_t)0);
        Int ps((uint64_t)0);
        Int qs((uint64_t)0);
        for (size_t i = 0; i < nVerifiers; ++i)
        {
            verificationValue += verificationShares[i];
        }
        for (size_t i = 0; i < nVerifiers; ++i)
        {
            ps += evaluationShares[i];
        }
        for (size_t i = 0; i < nVerifiers; ++i)
        {
            qs += evaluationShares[i + nVerifiers];
        }
        isValid = isValid && (verificationValue == ps * qs);

        Int resultValue((uint64_t)0);
        for (size_t i = 0; i < nVerifiers; ++i)
        {
            resultValue += resultShares[i];
        }
        isValid = isValid && (Int((uint64_t)0) == resultValue);

        SHA512_CTX vctx;
        unsigned char vdigest[SHA512_DIGEST_LENGTH];
        SHA512_Init(&vctx);
        SHA512_Update(&vctx, randoms, nVerifiers * sizeof(Int));
        SHA512_Final(vdigest, &vctx);
        Int totalRandomOfVerifiers = Int(vdigest);
        isValid = isValid && (commonRandom == totalRandomOfVerifiers);

        end = std::chrono::high_resolution_clock::now();
        verifierTime += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

        delete[] verificationShares;
        delete[] resultShares;
        delete[] evaluationShares;
        delete[] randoms;
    }

    delete[] outShares;

    if (!isValid)
    {
        std::cerr << "FLIOP is aborted | input length : " << inputLength << " / lambda : " << compressFactor
                  << std::endl;
        exit(-1);
    }

    return OneRoundMeasurement(proverTime, verifierTime, LANTime, WANTime);
}

template <typename Int>
OneRoundMeasurement MPC<Int>::SimulateFLIOPCoefficientOneRound(size_t inputLength, size_t compressFactor)
{
    Int::SetSeed(mSeed);

    std::vector<Int> op0(inputLength);
    std::vector<Int> op1(inputLength);
    std::memset(op0.data(), 1, inputLength * sizeof(Int));
    std::memset(op1.data(), 1, inputLength * sizeof(Int));

    std::vector<Int> op0Share(inputLength);
    std::vector<Int> op1Share(inputLength);
    std::memset(op0Share.data(), 0, inputLength * sizeof(Int));
    std::memset(op1Share.data(), 0, inputLength * sizeof(Int));

    double proverTime = 0.;
    double verifierTime = 0.;
    double LANTime = 0.;
    double WANTime = 0.;
    
    unsigned char secretKey[64] = {0};

    const size_t nVerifiers = mNParties - 1;

    bool isValid = true;
    Int* outShares = new Int[nVerifiers];
    outShares[0] = InnerProductCircuit<Int>::Forward(op0.data(), op1.data(), op0.size());
    for (size_t i = 1; i < nVerifiers; ++i)
    {
        outShares[i] = Int((uint64_t)0);
    }

    if (ceil(op0.size() / (double)compressFactor) > 1)
    {
        // Prover
        auto start = std::chrono::high_resolution_clock::now();
        InteractiveProof<Int> proof =
            InnerProductCircuit<Int>::MakeRoundCoefficientProof(op0.data(), op1.data(), op0.size(), compressFactor);
        std::vector<Proof<Int>> proofShares = proof.GetShares(nVerifiers);

        Int* randoms = new Int[nVerifiers];
        for (size_t i = 0; i < nVerifiers; ++i)
        {
            randoms[i] = proofShares[i].GetRandomFromOracle(secretKey, 64);
        }
        SHA512_CTX ctx;
        unsigned char digest[SHA512_DIGEST_LENGTH];
        SHA512_Init(&ctx);
        SHA512_Update(&ctx, randoms, nVerifiers * sizeof(Int));
        SHA512_Final(digest, &ctx);
        Int commonRandom = Int(digest);

        auto end = std::chrono::high_resolution_clock::now();
        proverTime += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

        // Communication
        LANTime += Network::GetLANPayloadDelay(proof.GetBytes() + 2 * sizeof(Int));
        WANTime += Network::GetWANPayloadDelay(proof.GetBytes() + 2 * sizeof(Int));

        Int* verificationShares = new Int[nVerifiers];

        // First verifier
        start = std::chrono::high_resolution_clock::now();
        std::vector<Query<Int>> queries =
            InnerProductCircuit<Int>::MakeRoundCoefficientQuery(commonRandom, compressFactor);
        verificationShares[0] = proofShares[0].GetQueryAnswer(queries[0]) - outShares[0];
        outShares[0] = proofShares[0].GetQueryAnswer(queries[1]);
        isValid = isValid && (randoms[0] == proofShares[0].GetRandomFromOracle(secretKey, 64));
        end = std::chrono::high_resolution_clock::now();
        verifierTime += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

        // The other verifiers
        for (size_t i = 1; i < nVerifiers; ++i)
        {
            verificationShares[i] = proofShares[i].GetQueryAnswer(queries[0]) - outShares[i];
            outShares[i] = proofShares[i].GetQueryAnswer(queries[1]);
            isValid = isValid && (randoms[i] == proofShares[i].GetRandomFromOracle(secretKey, 64));
        }


        // Communication - Verifiers send their own 'verificationShares' value, its own random.
        LANTime += Network::GetLANPayloadDelay(2 * sizeof(Int));
        WANTime += Network::GetWANPayloadDelay(2 * sizeof(Int));


        // Collector (one of the verifiers)
        start = std::chrono::high_resolution_clock::now();
        Int verificationValue((uint64_t)0);
        for (size_t i = 0; i < nVerifiers; ++i)
        {
            verificationValue += verificationShares[i];
        }
        isValid = isValid && (verificationValue == Int((uint64_t)0));

        SHA512_CTX vctx;
        unsigned char vdigest[SHA512_DIGEST_LENGTH];
        SHA512_Init(&vctx);
        SHA512_Update(&vctx, randoms, nVerifiers * sizeof(Int));
        SHA512_Final(vdigest, &vctx);
        Int totalRandomOfVerifiers = Int(vdigest);
        isValid = isValid && (commonRandom == totalRandomOfVerifiers);

        // Verifiers compress their own input vector.
        const size_t nPoly0 = ceil(op0.size() / (double)compressFactor);
        Int* const resizedInput0 = new Int[nPoly0 * compressFactor];
        std::memset(resizedInput0, 0, (nPoly0 * compressFactor) * sizeof(Int));
        std::memcpy(resizedInput0, op0.data(), op0.size() * sizeof(Int));
        std::vector<Polynomial<Int>> poly0s;
        poly0s.reserve(nPoly0);
        for (size_t i = 0; i < nPoly0; ++i)
        {
            poly0s.emplace_back(resizedInput0 + i * compressFactor, compressFactor, true);
        }
        op0 = proof.EvaluatePolyPs(commonRandom);
        delete[] resizedInput0;

        const size_t nPoly1 = ceil(op1.size() / (double)compressFactor);
        Int* const resizedInput1 = new Int[nPoly1 * compressFactor];
        std::memset(resizedInput1, 0, (nPoly1 * compressFactor) * sizeof(Int));
        std::memcpy(resizedInput1, op1.data(), op1.size() * sizeof(Int));
        std::vector<Polynomial<Int>> poly1s;
        poly1s.reserve(nPoly1);
        for (size_t i = 0; i < nPoly1; ++i)
        {
            Int::Reverse(resizedInput1 + i * compressFactor, resizedInput1 + i * compressFactor + compressFactor - 1);
            poly1s.emplace_back(resizedInput1 + i * compressFactor, compressFactor, true);
        }
        op1 = proof.EvaluatePolyPs(commonRandom);
        delete[] resizedInput1;

        end = std::chrono::high_resolution_clock::now();
        verifierTime += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

        // Prover
        start = std::chrono::high_resolution_clock::now();
        op0 = proof.EvaluatePolyPs(commonRandom);
        op1 = proof.EvaluatePolyQs(commonRandom);
        end = std::chrono::high_resolution_clock::now();
        proverTime += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

        delete[] verificationShares;
        delete[] randoms;
    }
    else
    {
        // Prover
        auto start = std::chrono::high_resolution_clock::now();
        Proof<Int> proof = InnerProductCircuit<Int>::MakeCoefficientProof(op0.data(), op1.data(), op0.size(), 1);
        std::vector<Proof<Int>> proofShares = proof.GetShares(nVerifiers);

        Int* randoms = new Int[nVerifiers];
        for (size_t i = 0; i < nVerifiers; ++i)
        {
            randoms[i] = proofShares[i].GetRandomFromOracle(secretKey, 64);
        }
        SHA512_CTX ctx;
        unsigned char digest[SHA512_DIGEST_LENGTH];
        SHA512_Init(&ctx);
        SHA512_Update(&ctx, randoms, nVerifiers * sizeof(Int));
        SHA512_Final(digest, &ctx);
        Int commonRandom = Int(digest);

        auto end = std::chrono::high_resolution_clock::now();
        proverTime += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();


        // Communication
        LANTime += Network::GetLANPayloadDelay(proof.GetBytes() + 4 * sizeof(Int));
        WANTime += Network::GetWANPayloadDelay(proof.GetBytes() + 4 * sizeof(Int));

        std::vector<Int> randomsInConstantTerms = proof.GetRandoms(2);
        Int* verificationShares = new Int[nVerifiers];
        Int* resultShares = new Int[nVerifiers];
        Int* evaluationShares = new Int[nVerifiers * 2];


        // First verifier
        start = std::chrono::high_resolution_clock::now();
        std::vector<Query<Int>> queries = InnerProductCircuit<Int>::MakeCoefficientQuery(commonRandom, op0.size(), 1);
        isValid = isValid && (randoms[0] == proofShares[0].GetRandomFromOracle(secretKey, 64));

        Int* const resizedInput0 = new Int[op0.size() + 1];
        std::memset(resizedInput0, 0, (op0.size() + 1) * sizeof(Int));
        *resizedInput0 = randomsInConstantTerms[0];
        std::memcpy(resizedInput0 + 1, op0.data(), op0.size() * sizeof(Int));
        Polynomial<Int> poly0(resizedInput0, op0.size() + 1, true);
        evaluationShares[0] = poly0.Evaluate(commonRandom);
        delete[] resizedInput0;

        Int* const resizedInput1 = new Int[op1.size() + 1];
        std::memset(resizedInput1, 0, (op1.size() + 1u) * sizeof(Int));
        *resizedInput1 = randomsInConstantTerms[1];
        std::memcpy(resizedInput1 + 1, op1.data(), op1.size() * sizeof(Int));
        Int::Reverse(resizedInput1 + 1, resizedInput1 + op1.size());
        Polynomial<Int> poly1(resizedInput1, op1.size() + 1, true);
        evaluationShares[nVerifiers] = poly1.Evaluate(commonRandom);
        delete[] resizedInput1;

        verificationShares[0] = proofShares[0].GetQueryAnswer(queries[queries.size() - 2]);
        resultShares[0] = proofShares[0].GetQueryAnswer(queries[queries.size() - 1]) - outShares[0];
        end = std::chrono::high_resolution_clock::now();
        verifierTime += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

        // The other verifiers
        for (size_t i = 1; i < nVerifiers; ++i)
        {
            isValid = isValid && (randoms[i] == proofShares[i].GetRandomFromOracle(secretKey, 64));

            Int* const resizedInput0 = new Int[op0Share.size() + 1];
            std::memset(resizedInput0, 0, (op0Share.size() + 1) * sizeof(Int));
            *resizedInput0 = Int((uint64_t)0);
            std::memcpy(resizedInput0 + 1, op0Share.data(), op0Share.size() * sizeof(Int));
            Polynomial<Int> poly0(resizedInput0, op0Share.size() + 1, true);
            evaluationShares[i] = poly0.Evaluate(commonRandom);
            delete[] resizedInput0;

            Int* const resizedInput1 = new Int[op1Share.size() + 1];
            std::memset(resizedInput1, 0, (op1Share.size() + 1) * sizeof(Int));
            *resizedInput1 = Int((uint64_t)0);
            std::memcpy(resizedInput1 + 1, op1Share.data(), op1Share.size() * sizeof(Int));
            Int::Reverse(resizedInput1 + 1, resizedInput1 + op1Share.size());
            Polynomial<Int> poly1(resizedInput1, op1Share.size() + 1, true);
            evaluationShares[i + nVerifiers] = poly1.Evaluate(commonRandom);
            delete[] resizedInput1;

            verificationShares[i] = proofShares[i].GetQueryAnswer(queries[queries.size() - 2]);
            resultShares[i] = proofShares[i].GetQueryAnswer(queries[queries.size() - 1]) - outShares[i];
        }

        // Communication - Verifiers share two evaluationShares, one varificationShare,
        // one resultShare, private random.
        LANTime += Network::GetLANPayloadDelay(5 * sizeof(Int));
        WANTime += Network::GetWANPayloadDelay(5 * sizeof(Int));

        // Collector
        start = std::chrono::high_resolution_clock::now();
        Int verificationValue((uint64_t)0);
        Int ps((uint64_t)0);
        Int qs((uint64_t)0);
        for (size_t i = 0; i < nVerifiers; ++i)
        {
            verificationValue += verificationShares[i];
        }
        for (size_t i = 0; i < nVerifiers; ++i)
        {
            ps += evaluationShares[i];
        }
        for (size_t i = 0; i < nVerifiers; ++i)
        {
            qs += evaluationShares[i + nVerifiers];
        }
        isValid = isValid && (verificationValue == ps * qs);

        Int resultValue((uint64_t)0);
        for (size_t i = 0; i < nVerifiers; ++i)
        {
            resultValue += resultShares[i];
        }
        isValid = isValid && (Int((uint64_t)0) == resultValue);

        SHA512_CTX vctx;
        unsigned char vdigest[SHA512_DIGEST_LENGTH];
        SHA512_Init(&vctx);
        SHA512_Update(&vctx, randoms, nVerifiers * sizeof(Int));
        SHA512_Final(vdigest, &vctx);
        Int totalRandomOfVerifiers = Int(vdigest);
        isValid = isValid && (commonRandom == totalRandomOfVerifiers);

        end = std::chrono::high_resolution_clock::now();
        verifierTime += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

        delete[] verificationShares;
        delete[] resultShares;
        delete[] evaluationShares;
        delete[] randoms;
    }

    delete[] outShares;

    if (!isValid)
    {
        std::cerr << "FLIOP is aborted | input length : " << inputLength << " / lambda : " << compressFactor
                  << std::endl;
        exit(-1);
    }

    return OneRoundMeasurement(proverTime, verifierTime, LANTime, WANTime);
}

template <typename Int> void MPC<Int>::CalculateOneRoundTimesRecursive(const size_t inputLength)
{
    const size_t maxCompress = std::min(mMaxLambda, inputLength);
    for (size_t lambda = 2; lambda <= maxCompress; ++lambda)
    {
        if (mOneRoundMeasures[lambda][inputLength].proverTimeNs == 0.)
        {
            mOneRoundMeasures[lambda][inputLength] = SimulateFLIOPOneRound(inputLength, lambda);
        }
        CalculateOneRoundTimesRecursive(ceil(inputLength / (double)lambda));
    }

    if (inputLength >= mInputLength * 0.1)
    {
        std::cout << "Calculated : " << inputLength << std::endl;
    }
}

template <typename Int> void MPC<Int>::CalculateOneCoefficientRoundTimesRecursive(const size_t inputLength)
{
    const size_t maxCompress = std::min(mMaxLambda, inputLength);
    for (size_t lambda = 2; lambda <= maxCompress; ++lambda)
    {
        if (mOneRoundMeasures[lambda][inputLength].proverTimeNs == 0.)
        {
            mOneRoundMeasures[lambda][inputLength] = SimulateFLIOPCoefficientOneRound(inputLength, lambda);
        }
        CalculateOneCoefficientRoundTimesRecursive(ceil(inputLength / (double)lambda));
    }

    if (inputLength >= mInputLength * 0.1)
    {
        std::cout << "Calculated : " << inputLength << std::endl;
    }
}

template <typename Int> IOPSchedule MPC<Int>::FindBestLANSchedule(const size_t inputLength)
{
    IOPSchedule best = FindBestLANScheduleRecursive(inputLength);
    std::reverse(best.lambdas.begin(), best.lambdas.end());
    std::reverse(best.trace.begin(), best.trace.end());
    return best;
}

template <typename Int> IOPSchedule MPC<Int>::FindBestLANScheduleRecursive(const size_t inputLength)
{
    if (inputLength <= 1)
    {
        std::vector<size_t> lambdas;
        std::vector<OneRoundMeasurement> trace;
        return IOPSchedule(0., lambdas, trace);
    }

    double totalMin = DBL_MAX;
    std::vector<size_t> bestLambdas;
    std::vector<OneRoundMeasurement> newTrace;
    const size_t maxCompress = std::min(mMaxLambda, inputLength);
    for (size_t lambda = 2; lambda <= maxCompress; ++lambda)
    {
        IOPSchedule best = FindBestLANScheduleRecursive(ceil(inputLength / (double)lambda));
        double currMin = mOneRoundMeasures[lambda][inputLength].proverTimeNs +
                         mOneRoundMeasures[lambda][inputLength].verifierTimeNs +
                         mOneRoundMeasures[lambda][inputLength].communicationTimeNsInLAN + best.time;
        if (totalMin > currMin)
        {
            totalMin = currMin;
            bestLambdas = best.lambdas;
            bestLambdas.push_back(lambda);
            newTrace = best.trace;
            newTrace.push_back(mOneRoundMeasures[lambda][inputLength]);
        }
    }

    return IOPSchedule(totalMin, bestLambdas, newTrace);
}

template <typename Int> IOPSchedule MPC<Int>::FindBestWANSchedule(const size_t inputLength)
{
    IOPSchedule best = FindBestWANScheduleRecursive(inputLength);
    std::reverse(best.lambdas.begin(), best.lambdas.end());
    std::reverse(best.trace.begin(), best.trace.end());
    return best;
}

template <typename Int> IOPSchedule MPC<Int>::FindBestWANScheduleRecursive(const size_t inputLength)
{
    if (inputLength <= 1)
    {
        std::vector<size_t> lambdas;
        std::vector<OneRoundMeasurement> trace;
        return IOPSchedule(0., lambdas, trace);
    }

    double totalMin = DBL_MAX;
    std::vector<size_t> bestLambdas;
    std::vector<OneRoundMeasurement> newTrace;
    const size_t maxCompress = std::min(mMaxLambda, inputLength);
    for (size_t lambda = 2; lambda <= maxCompress; ++lambda)
    {
        IOPSchedule best = FindBestWANScheduleRecursive(ceil(inputLength / (double)lambda));
        double currMin = mOneRoundMeasures[lambda][inputLength].proverTimeNs +
                         mOneRoundMeasures[lambda][inputLength].verifierTimeNs +
                         mOneRoundMeasures[lambda][inputLength].communicationTimeNsInWAN + best.time;
        if (totalMin > currMin)
        {
            totalMin = currMin;
            bestLambdas = best.lambdas;
            bestLambdas.push_back(lambda);
            newTrace = best.trace;
            newTrace.push_back(mOneRoundMeasures[lambda][inputLength]);
        }
    }

    return IOPSchedule(totalMin, bestLambdas, newTrace);
}

template <typename Int> IOPSchedule MPC<Int>::FindLANDelay(const size_t inputLength)
{
    IOPSchedule schedule = FindLANDelayRecursive(inputLength);
    std::reverse(schedule.trace.begin(), schedule.trace.end());
    return schedule;
}

template <typename Int> IOPSchedule MPC<Int>::FindLANDelayRecursive(const size_t inputLength)
{
    if (inputLength <= 1)
    {
        std::vector<size_t> lambdas;
        std::vector<OneRoundMeasurement> trace;
        return IOPSchedule(0., lambdas, trace);
    }

    IOPSchedule schedule = FindLANDelayRecursive(ceil(inputLength / (double)2));

    double newTotal = mOneRoundMeasures[2][inputLength].proverTimeNs +
                      mOneRoundMeasures[2][inputLength].verifierTimeNs +
                      mOneRoundMeasures[2][inputLength].communicationTimeNsInLAN + schedule.time;
    std::vector<size_t> newLambdas = schedule.lambdas;
    newLambdas.push_back(2);
    std::vector<OneRoundMeasurement> newTrace = schedule.trace;
    newTrace.push_back(mOneRoundMeasures[2][inputLength]);

    return IOPSchedule(newTotal, newLambdas, newTrace);
}

template <typename Int> IOPSchedule MPC<Int>::FindWANDelay(const size_t inputLength)
{
    IOPSchedule schedule = FindWANDelayRecursive(inputLength);
    std::reverse(schedule.trace.begin(), schedule.trace.end());
    return schedule;
}

template <typename Int> IOPSchedule MPC<Int>::FindWANDelayRecursive(const size_t inputLength)
{
    if (inputLength <= 1)
    {
        std::vector<size_t> lambdas;
        std::vector<OneRoundMeasurement> trace;
        return IOPSchedule(0., lambdas, trace);
    }

    IOPSchedule schedule = FindWANDelayRecursive(ceil(inputLength / (double)2));

    double newTotal = mOneRoundMeasures[2][inputLength].proverTimeNs +
                      mOneRoundMeasures[2][inputLength].verifierTimeNs +
                      mOneRoundMeasures[2][inputLength].communicationTimeNsInWAN + schedule.time;
    std::vector<size_t> newLambdas = schedule.lambdas;
    newLambdas.push_back(2);
    std::vector<OneRoundMeasurement> newTrace = schedule.trace;
    newTrace.push_back(mOneRoundMeasures[2][inputLength]);

    return IOPSchedule(newTotal, newLambdas, newTrace);
}

template <typename Int> void MPC<Int>::FindBestFLIOPSchedule(bool coefficient)
{
    const size_t nStep = (size_t)std::log2(mInputLength / (double)2) + 1;

    std::cout.sync_with_stdio(false);

    if (coefficient)
    {
        CalculateOneCoefficientRoundTimesRecursive(mInputLength);
    }
    else
    {
        CalculateOneRoundTimesRecursive(mInputLength);
    }
    std::cout << "One round times were successfully measured!" << std::endl;

    size_t j = 0;
    double* outputs = new double[nStep];
    double* proverTimeOutputs = new double[nStep];
    double* verifierTimeOutputs = new double[nStep];
    double* commTimeOutputs = new double[nStep];

    std::cout << "LAN Min schedule" << std::endl;
    for (size_t i = 2; i <= mInputLength; i *= 2)
    {
        IOPSchedule best = FindBestLANSchedule(i);
        outputs[j] = best.time * 1e-6 + 2 * Network::LANBaseDelayMs;

        std::cout << "Length: " << i << " / Min time : ";
        std::cout << std::fixed << outputs[j] << std::setprecision(9);
        std::cout << " / Best schedule : ";
        for (size_t k = 0; k < best.lambdas.size(); ++k)
        {
            std::cout << best.lambdas[k] << " ";
        }
        std::cout << std::endl;

        proverTimeOutputs[j] = 0.;
        verifierTimeOutputs[j] = 0.;
        commTimeOutputs[j] = 0.;
        for (size_t k = 0; k < best.trace.size(); ++k)
        {
            proverTimeOutputs[j] += best.trace[k].proverTimeNs;
            verifierTimeOutputs[j] += best.trace[k].verifierTimeNs;
            commTimeOutputs[j] += best.trace[k].communicationTimeNsInLAN;
        }
        proverTimeOutputs[j] *= 1e-6;
        verifierTimeOutputs[j] *= 1e-6;
        commTimeOutputs[j] = commTimeOutputs[j] * 1e-6 + 2 * Network::LANBaseDelayMs;

        ++j;
    }
    std::cout << "Total: ";
    for (size_t i = 0; i < nStep; ++i)
    {
        std::cout << std::fixed << outputs[i] << std::setprecision(9) << ", ";
    }
    std::cout << std::endl;
    std::cout << "Prover: ";
    for (size_t i = 0; i < nStep; ++i)
    {
        std::cout << std::fixed << proverTimeOutputs[i] << std::setprecision(9) << ", ";
    }
    std::cout << std::endl;
    std::cout << "Verifier: ";
    for (size_t i = 0; i < nStep; ++i)
    {
        std::cout << std::fixed << verifierTimeOutputs[i] << std::setprecision(9) << ", ";
    }
    std::cout << std::endl;
    std::cout << "Communication: ";
    for (size_t i = 0; i < nStep; ++i)
    {
        std::cout << std::fixed << commTimeOutputs[i] << std::setprecision(9) << ", ";
    }
    std::cout << std::endl;

    j = 0;
    std::cout << "WAN Min schedule" << std::endl;
    for (size_t i = 2; i <= mInputLength; i *= 2)
    {
        IOPSchedule best = FindBestWANSchedule(i);
        std::cout << "Length: " << i;
        std::cout << " / Min time : " << std::fixed << best.time * 1e-6 + 2 * Network::WANBaseDelayMs
                  << std::setprecision(9);
        std::cout << " / Best schedule : ";
        for (size_t k = 0; k < best.lambdas.size(); ++k)
        {
            std::cout << best.lambdas[k] << " ";
        }
        std::cout << std::endl;

        outputs[j] = best.time * 1e-6 + 2 * Network::WANBaseDelayMs;

        proverTimeOutputs[j] = 0.;
        verifierTimeOutputs[j] = 0.;
        commTimeOutputs[j] = 0.;
        for (size_t k = 0; k < best.trace.size(); ++k)
        {
            proverTimeOutputs[j] += best.trace[k].proverTimeNs;
            verifierTimeOutputs[j] += best.trace[k].verifierTimeNs;
            commTimeOutputs[j] += best.trace[k].communicationTimeNsInWAN;
        }
        proverTimeOutputs[j] *= 1e-6;
        verifierTimeOutputs[j] *= 1e-6;
        commTimeOutputs[j] = commTimeOutputs[j] * 1e-6 + 2 * Network::WANBaseDelayMs;

        ++j;
    }
    std::cout << "Total: ";
    for (size_t i = 0; i < nStep; ++i)
    {
        std::cout << std::fixed << outputs[i] << std::setprecision(9) << ", ";
    }
    std::cout << std::endl;
    std::cout << "Prover: ";
    for (size_t i = 0; i < nStep; ++i)
    {
        std::cout << std::fixed << proverTimeOutputs[i] << std::setprecision(9) << ", ";
    }
    std::cout << std::endl;
    std::cout << "Verifier: ";
    for (size_t i = 0; i < nStep; ++i)
    {
        std::cout << std::fixed << verifierTimeOutputs[i] << std::setprecision(9) << ", ";
    }
    std::cout << std::endl;
    std::cout << "Communication: ";
    for (size_t i = 0; i < nStep; ++i)
    {
        std::cout << std::fixed << commTimeOutputs[i] << std::setprecision(9) << ", ";
    }
    std::cout << std::endl;

    j = 0;
    std::cout << "FLIOP LAN Delay" << std::endl;
    for (size_t i = 2; i <= mInputLength; i *= 2)
    {
        IOPSchedule schedule = FindLANDelay(i);
        std::cout << "Length: " << i << " / Time : " << std::fixed << schedule.time * 1e-6 + 2 * Network::LANBaseDelayMs
                  << std::setprecision(9) << std::endl;
        outputs[j] = schedule.time * 1e-6 + 2 * Network::LANBaseDelayMs;

        proverTimeOutputs[j] = 0.;
        verifierTimeOutputs[j] = 0.;
        commTimeOutputs[j] = 0.;
        for (size_t k = 0; k < schedule.trace.size(); ++k)
        {
            proverTimeOutputs[j] += schedule.trace[k].proverTimeNs;
            verifierTimeOutputs[j] += schedule.trace[k].verifierTimeNs;
            commTimeOutputs[j] += schedule.trace[k].communicationTimeNsInLAN;
        }
        proverTimeOutputs[j] *= 1e-6;
        verifierTimeOutputs[j] *= 1e-6;
        commTimeOutputs[j] = commTimeOutputs[j] * 1e-6 + 2 * Network::LANBaseDelayMs;

        ++j;
    }
    std::cout << "Total: ";
    for (size_t i = 0; i < nStep; ++i)
    {
        std::cout << std::fixed << outputs[i] << std::setprecision(9) << ", ";
    }
    std::cout << std::endl;
    std::cout << "Prover: ";
    for (size_t i = 0; i < nStep; ++i)
    {
        std::cout << std::fixed << proverTimeOutputs[i] << std::setprecision(9) << ", ";
    }
    std::cout << std::endl;
    std::cout << "Verifier: ";
    for (size_t i = 0; i < nStep; ++i)
    {
        std::cout << std::fixed << verifierTimeOutputs[i] << std::setprecision(9) << ", ";
    }
    std::cout << std::endl;
    std::cout << "Communication: ";
    for (size_t i = 0; i < nStep; ++i)
    {
        std::cout << std::fixed << commTimeOutputs[i] << std::setprecision(9) << ", ";
    }
    std::cout << std::endl;

    j = 0;
    std::cout << "FLIOP WAN Delay" << std::endl;
    for (size_t i = 2; i <= mInputLength; i *= 2)
    {
        IOPSchedule schedule = FindWANDelay(i);
        std::cout << "Length: " << i << " / Time : " << std::fixed << schedule.time * 1e-6 + 2 * Network::WANBaseDelayMs
                  << std::setprecision(9) << std::endl;
        outputs[j] = schedule.time * 1e-6 + 2 * Network::WANBaseDelayMs;

        proverTimeOutputs[j] = 0.;
        verifierTimeOutputs[j] = 0.;
        commTimeOutputs[j] = 0.;
        for (size_t k = 0; k < schedule.trace.size(); ++k)
        {
            proverTimeOutputs[j] += schedule.trace[k].proverTimeNs;
            verifierTimeOutputs[j] += schedule.trace[k].verifierTimeNs;
            commTimeOutputs[j] += schedule.trace[k].communicationTimeNsInWAN;
        }
        proverTimeOutputs[j] *= 1e-6;
        verifierTimeOutputs[j] *= 1e-6;
        commTimeOutputs[j] = commTimeOutputs[j] * 1e-6 + 2 * Network::WANBaseDelayMs;

        ++j;
    }
    std::cout << "Total: ";
    for (size_t i = 0; i < nStep; ++i)
    {
        std::cout << std::fixed << outputs[i] << std::setprecision(9) << ", ";
    }
    std::cout << std::endl;
    std::cout << "Prover: ";
    for (size_t i = 0; i < nStep; ++i)
    {
        std::cout << std::fixed << proverTimeOutputs[i] << std::setprecision(9) << ", ";
    }
    std::cout << std::endl;
    std::cout << "Verifier: ";
    for (size_t i = 0; i < nStep; ++i)
    {
        std::cout << std::fixed << verifierTimeOutputs[i] << std::setprecision(9) << ", ";
    }
    std::cout << std::endl;
    std::cout << "Communication: ";
    for (size_t i = 0; i < nStep; ++i)
    {
        std::cout << std::fixed << commTimeOutputs[i] << std::setprecision(9) << ", ";
    }
    std::cout << std::endl;

    delete[] outputs;
    delete[] proverTimeOutputs;
    delete[] verifierTimeOutputs;
    delete[] commTimeOutputs;
}

#endif
