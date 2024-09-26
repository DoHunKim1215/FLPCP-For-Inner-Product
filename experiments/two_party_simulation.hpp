#ifndef TWO_PARTY_SIMULATION
#define TWO_PARTY_SIMULATION

#include <chrono>

#include "network.hpp"
#include "../circuit/inner_product_circuit.hpp"
#include "../math/square_matrix.hpp"
#include "../unit/proof.hpp"

template <typename Int> class TwoPartySimulation
{
public:
    TwoPartySimulation(const uint32_t seed, const size_t inputLength, const size_t maxLambda);

    void FindBestFLIOPSchedule();
    void FindBestFLIOPCoefficientSchedule(const size_t inputLength, const size_t maxLambda);

private:
    size_t mInputLength;
    size_t mMaxLambda;
    uint32_t mSeed;
    double* mProverTimes;
    double* mVerifierTimes;
    double* mLANTimes;
    double* mWANTimes;
    double* mTotalLANTimes;
    double* mTotalWANTimes;

    OneRoundMeasurement SimulateFLIOPOneRound(size_t seed, size_t inputLength, size_t compressFactor);
    OneRoundMeasurement SimulateFLIOPCoefficientOneRound(size_t seed, size_t inputLength, size_t compressFactor);
    BestSchedule FindBestFLIOPScheduleRecursive(const size_t inputLength, double* totalTimes);
    double FindFLIOPPartialDelayRecursive(const size_t inputLength, double* totalTimes,
                                          const std::vector<size_t> lambdas);
    double FindFLIOPDelayRecursive(const size_t inputLength, double* totalTimes);
    void CalculateOneRoundTimes(const size_t inputLength);
};

template <typename Int>
TwoPartySimulation<Int>::TwoPartySimulation(const uint32_t seed, const size_t inputLength, const size_t maxLambda)
{
    mSeed = seed;
    mInputLength = inputLength;
    mMaxLambda = maxLambda;
}

template <typename Int>
OneRoundMeasurement TwoPartySimulation<Int>::SimulateFLIOPOneRound(size_t seed, size_t inputLength,
                                                                   size_t compressFactor)
{
    Int::SetSeed(seed);

    std::vector<Int> op0(inputLength);
    std::vector<Int> op1(inputLength);
    std::memset(op0.data(), 1, inputLength * sizeof(Int));
    std::memset(op1.data(), 1, inputLength * sizeof(Int));

    double proverTime = 0.;
    double verifierTime = 0.;
    double LANTime = 0.;
    double WANTime = 0.;

    bool isValid = true;
    Int out = InnerProductCircuit<Int>::Forward(op0.data(), op1.data(), op0.size());
    if (ceil(op0.size() / (double)compressFactor) > 1)
    {
        // Precomputation
        SquareMatrix<Int> evalToCoeff = SquareMatrix<Int>::GetVandermondeInverse(compressFactor);

        // Prover
        auto start = std::chrono::high_resolution_clock::now();
        InteractiveProof<Int> proof = InnerProductCircuit<Int>::MakeRoundProofWithPrecompute(
            op0.data(), op1.data(), op0.size(), compressFactor, evalToCoeff);
        Int random = proof.GetRandomFromOracle();
        op0 = proof.EvaluatePolyPs(random);
        op1 = proof.EvaluatePolyQs(random);
        auto end = std::chrono::high_resolution_clock::now();
        proverTime += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

        // Communication
        LANTime += Network::GetLANPayloadDelay(proof.GetBytes() + sizeof(Int));
        WANTime += Network::GetWANPayloadDelay(proof.GetBytes() + sizeof(Int));

        // Verifier 1
        start = std::chrono::high_resolution_clock::now();
        isValid = isValid && (random == proof.GetRandomFromOracle());
        std::vector<Query<Int>> queries = InnerProductCircuit<Int>::MakeRoundQuery(random, compressFactor);
        isValid = isValid && (out == proof.GetQueryAnswer(queries[0]));
        out = proof.GetQueryAnswer(queries[1]);
        end = std::chrono::high_resolution_clock::now();
        verifierTime += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    }
    else
    {
        // Precomputation
        SquareMatrix<Int> evalToCoeff = SquareMatrix<Int>::GetVandermondeInverse(op0.size() + 1);

        // Prover
        auto start = std::chrono::high_resolution_clock::now();
        Proof proof = InnerProductCircuit<Int>::MakeProofWithPrecompute(op0.data(), op1.data(), op0.size(), op0.size(),
                                                                        evalToCoeff);
        auto end = std::chrono::high_resolution_clock::now();
        proverTime += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

        // Communication
        LANTime += Network::GetLANPayloadDelay(proof.GetBytes() + op0.size() * 2 * sizeof(Int));
        WANTime += Network::GetWANPayloadDelay(proof.GetBytes() + op0.size() * 2 * sizeof(Int));

        // Verifier 1
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
    }

    if (!isValid)
    {
        exit(-1); // FLIOP is aborted!!
    }

    return OneRoundMeasurement(proverTime, verifierTime, LANTime, WANTime);
}

template <typename Int>
OneRoundMeasurement TwoPartySimulation<Int>::SimulateFLIOPCoefficientOneRound(size_t seed, size_t inputLength,
                                                                              size_t compressFactor)
{
    Int::SetSeed(seed);

    std::vector<Int> op0(inputLength);
    std::vector<Int> op1(inputLength);
    std::memset(op0.data(), 1, inputLength * sizeof(Int));
    std::memset(op1.data(), 1, inputLength * sizeof(Int));

    double proverTime = 0.;
    double verifierTime = 0.;
    double LANTime = 0.;
    double WANTime = 0.;

    bool isValid = true;
    Int out = InnerProductCircuit<Int>::Forward(op0.data(), op1.data(), op0.size());
    if (ceil(op0.size() / (double)compressFactor) > 1)
    {
        // Prover
        auto start_prover = std::chrono::high_resolution_clock::now();
        InteractiveProof<Int> proof =
            InnerProductCircuit<Int>::MakeRoundCoefficientProof(op0.data(), op1.data(), op0.size(), compressFactor);
        Int random = proof.GetRandomFromOracle();
        op0 = proof.EvaluatePolyPs(random);
        op1 = proof.EvaluatePolyQs(random);
        auto end_prover = std::chrono::high_resolution_clock::now();
        proverTime += std::chrono::duration_cast<std::chrono::nanoseconds>(end_prover - start_prover).count();

        // Communication
        LANTime += Network::GetLANPayloadDelay(proof.GetBytes() + sizeof(Int));
        WANTime += Network::GetWANPayloadDelay(proof.GetBytes() + sizeof(Int));

        // Verifier
        auto start_verifier = std::chrono::high_resolution_clock::now();
        isValid = isValid && (random == proof.GetRandomFromOracle());
        std::vector<Query<Int>> queries = InnerProductCircuit<Int>::MakeRoundCoefficientQuery(random, compressFactor);
        isValid = isValid && (out == proof.GetQueryAnswer(queries[0]));
        out = proof.GetQueryAnswer(queries[1]);
        auto end_verifier = std::chrono::high_resolution_clock::now();
        verifierTime += std::chrono::duration_cast<std::chrono::nanoseconds>(end_verifier - start_verifier).count();
    }
    else
    {
        // Prover
        auto start = std::chrono::high_resolution_clock::now();
        Proof<Int> proof = InnerProductCircuit<Int>::MakeCoefficientProof(op0.data(), op1.data(), op0.size(), 1);
        auto end = std::chrono::high_resolution_clock::now();
        proverTime += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

        // Communication
        LANTime += Network::GetLANPayloadDelay(proof.GetBytes() + op0.size() * 2 * sizeof(Int));
        WANTime += Network::GetWANPayloadDelay(proof.GetBytes() + op0.size() * 2 * sizeof(Int));

        // Verifier 1
        start = std::chrono::high_resolution_clock::now();
        std::vector<Query<Int>> queries =
            InnerProductCircuit<Int>::MakeCoefficientQuery(Int::GenerateRandom(), op0.size(), 1);
        isValid =
            isValid &&
            (proof.GetQueryAnswer(queries[0]) * proof.GetQueryAnswer(queries[1]) == proof.GetQueryAnswer(queries[2])) &&
            (proof.GetQueryAnswer(queries[3]) == out);
        end = std::chrono::high_resolution_clock::now();
        verifierTime += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    }

    if (!isValid)
    {
        exit(-1); // FLIOP is aborted!!
    }

    return OneRoundMeasurement(proverTime, verifierTime, LANTime, WANTime);
}

template <typename Int>
BestSchedule TwoPartySimulation<Int>::FindBestFLIOPScheduleRecursive(const size_t inputLength, double* totalTimes)
{
    if (inputLength == 2u)
    {
        std::vector<size_t> lambdas(1);
        lambdas[0] = 2u;
        return BestSchedule(totalTimes[inputLength + mInputLength * 2u], lambdas);
    }

    double totalMin = DBL_MAX;
    std::vector<size_t> bestLambdas;
    size_t maxCompress = std::min(mMaxLambda, inputLength);
    for (size_t i = 2u; i <= maxCompress; ++i)
    {
        BestSchedule best = FindBestFLIOPScheduleRecursive(ceil(inputLength / (double)i), totalTimes);
        double currMin = totalTimes[inputLength + mInputLength * i] + best.time;
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
double TwoPartySimulation<Int>::FindFLIOPDelayRecursive(const size_t inputLength, double* totalTimes)
{
    if (inputLength == 2u)
    {
        return totalTimes[inputLength + mInputLength * 2u];
    }
    double best = FindFLIOPDelayRecursive(ceil(inputLength / (double)2), totalTimes);
    return totalTimes[inputLength + mInputLength * 2] + best;
}

template <typename Int>
double TwoPartySimulation<Int>::FindFLIOPPartialDelayRecursive(const size_t inputLength, double* totalTimes,
                                                               const std::vector<size_t> lambdas)
{
    size_t currentInputLength = inputLength;
    double result = 0.;
    for (size_t i = 0; i < lambdas.size(); ++i)
    {
        result += totalTimes[currentInputLength + mInputLength * lambdas[i]];
        currentInputLength = ceil(currentInputLength / (double)lambdas[i]);
    }
    return result;
}

template <typename Int>
void TwoPartySimulation<Int>::CalculateOneRoundTimes(const size_t inputLength)
{
    size_t maxCompress = std::min(mMaxLambda, inputLength);
    for (size_t i = 2; i <= maxCompress; ++i)
    {
        OneRoundMeasurement measure = SimulateFLIOPOneRound(mSeed, inputLength, i);
        mProverTimes[inputLength + mInputLength * i] = measure.proverTimeNs * 1e-6;
        mVerifierTimes[inputLength + mInputLength * i] = measure.verifierTimeNs * 1e-6;
        mLANTimes[inputLength + mInputLength * i] = measure.communicationTimeNsInLAN * 1e-6;
        mWANTimes[inputLength + mInputLength * i] = measure.communicationTimeNsInWAN * 1e-6;
        mTotalLANTimes[inputLength + mInputLength * i] = mProverTimes[inputLength + mInputLength * i] +
                                                         mVerifierTimes[inputLength + mInputLength * i] +
                                                         mLANTimes[inputLength + mInputLength * i];
        mTotalWANTimes[inputLength + mInputLength * i] = mProverTimes[inputLength + mInputLength * i] +
                                                         mVerifierTimes[inputLength + mInputLength * i] +
                                                         mWANTimes[inputLength + mInputLength * i];

        CalculateOneRoundTimes(ceil(inputLength / (double)i));
    }
}

template <typename Int>
void TwoPartySimulation<Int>::FindBestFLIOPSchedule()
{
    mProverTimes = new double[(mInputLength + 1u) * (mMaxLambda + 1u)];
    mVerifierTimes = new double[(mInputLength + 1u) * (mMaxLambda + 1u)];
    mLANTimes = new double[(mInputLength + 1u) * (mMaxLambda + 1u)];
    mWANTimes = new double[(mInputLength + 1u) * (mMaxLambda + 1u)];
    mTotalLANTimes = new double[(mInputLength + 1u) * (mMaxLambda + 1u)];
    mTotalWANTimes = new double[(mInputLength + 1u) * (mMaxLambda + 1u)];
    CalculateOneRoundTimes(mInputLength);

    size_t j = 0;
    double outputs[10];
    double proverTimeOutputs[10];
    double verifierTimeOutputs[10];
    double commTimeOutputs[10];

    std::cout << "LAN Min schedule" << std::endl;
    for (size_t i = 2u; i <= mInputLength; i *= 2u)
    {
        BestSchedule best = FindBestFLIOPScheduleRecursive(i, mTotalLANTimes);
        std::cout << "Length: " << i;
        std::cout << " / Min time : " << std::fixed << best.time + Network::LANBaseDelayMs << std::setprecision(9);
        std::cout << " / Best schedule : ";
        outputs[j] = best.time + Network::LANBaseDelayMs;

        std::reverse(best.lambdas.begin(), best.lambdas.end());
        for (size_t j = 0; j < best.lambdas.size(); ++j)
        {
            std::cout << best.lambdas[j] << " ";
        }
        std::cout << std::endl;

        proverTimeOutputs[j] = FindFLIOPPartialDelayRecursive(i, mProverTimes, best.lambdas);
        verifierTimeOutputs[j] = FindFLIOPPartialDelayRecursive(i, mVerifierTimes, best.lambdas);
        commTimeOutputs[j] =
            FindFLIOPPartialDelayRecursive(i, mLANTimes, best.lambdas) + Network::LANBaseDelayMs;
        ++j;
    }
    std::cout << "Total: ";
    for (size_t i = 0; i < 10; ++i)
    {
        std::cout << std::fixed << outputs[i] << std::setprecision(9) << ", ";
    }
    std::cout << std::endl;
    std::cout << "Prover: ";
    for (size_t i = 0; i < 10; ++i)
    {
        std::cout << std::fixed << proverTimeOutputs[i] << std::setprecision(9) << ", ";
    }
    std::cout << std::endl;
    std::cout << "Verifier: ";
    for (size_t i = 0; i < 10; ++i)
    {
        std::cout << std::fixed << verifierTimeOutputs[i] << std::setprecision(9) << ", ";
    }
    std::cout << std::endl;
    std::cout << "Communication: ";
    for (size_t i = 0; i < 10; ++i)
    {
        std::cout << std::fixed << commTimeOutputs[i] << std::setprecision(9) << ", ";
    }
    std::cout << std::endl;

    j = 0;
    std::cout << "WAN Min schedule" << std::endl;
    for (size_t i = 2u; i <= mInputLength; i *= 2u)
    {
        BestSchedule best = FindBestFLIOPScheduleRecursive(i, mTotalWANTimes);
        std::cout << "Length: " << i;
        std::cout << " / Min time : " << std::fixed << best.time + Network::WANBaseDelayMs << std::setprecision(9);
        std::cout << " / Best schedule : ";
        outputs[j] = best.time + Network::WANBaseDelayMs;

        std::reverse(best.lambdas.begin(), best.lambdas.end());
        for (size_t j = 0; j < best.lambdas.size(); ++j)
        {
            std::cout << best.lambdas[j] << " ";
        }
        std::cout << std::endl;

        proverTimeOutputs[j] = FindFLIOPPartialDelayRecursive(i, mProverTimes, best.lambdas);
        verifierTimeOutputs[j] = FindFLIOPPartialDelayRecursive(i, mVerifierTimes, best.lambdas);
        commTimeOutputs[j] =
            FindFLIOPPartialDelayRecursive(i, mWANTimes, best.lambdas) + Network::WANBaseDelayMs;
        ++j;
    }
    std::cout << "Total: ";
    for (size_t i = 0; i < 10; ++i)
    {
        std::cout << std::fixed << outputs[i] << std::setprecision(9) << ", ";
    }
    std::cout << std::endl;
    std::cout << "Prover: ";
    for (size_t i = 0; i < 10; ++i)
    {
        std::cout << std::fixed << proverTimeOutputs[i] << std::setprecision(9) << ", ";
    }
    std::cout << std::endl;
    std::cout << "Verifier: ";
    for (size_t i = 0; i < 10; ++i)
    {
        std::cout << std::fixed << verifierTimeOutputs[i] << std::setprecision(9) << ", ";
    }
    std::cout << std::endl;
    std::cout << "Communication: ";
    for (size_t i = 0; i < 10; ++i)
    {
        std::cout << std::fixed << commTimeOutputs[i] << std::setprecision(9) << ", ";
    }
    std::cout << std::endl;

    j = 0;
    std::cout << "FLIOP LAN Delay" << std::endl;
    for (size_t i = 2u; i <= mInputLength; i *= 2u)
    {
        double fliopTime = FindFLIOPDelayRecursive(i, mTotalLANTimes);
        std::cout << "Length: " << i << " / Time : " << std::fixed << fliopTime + Network::LANBaseDelayMs
                  << std::setprecision(9) << std::endl;
        outputs[j] = fliopTime + Network::LANBaseDelayMs;

        proverTimeOutputs[j] = FindFLIOPDelayRecursive(i, mProverTimes);
        verifierTimeOutputs[j] = FindFLIOPDelayRecursive(i, mVerifierTimes);
        commTimeOutputs[j] = FindFLIOPDelayRecursive(i, mLANTimes) + Network::LANBaseDelayMs;
        ++j;
    }
    std::cout << "Total: ";
    for (size_t i = 0; i < 10; ++i)
    {
        std::cout << std::fixed << outputs[i] << std::setprecision(9) << ", ";
    }
    std::cout << std::endl;
    std::cout << "Prover: ";
    for (size_t i = 0; i < 10; ++i)
    {
        std::cout << std::fixed << proverTimeOutputs[i] << std::setprecision(9) << ", ";
    }
    std::cout << std::endl;
    std::cout << "Verifier: ";
    for (size_t i = 0; i < 10; ++i)
    {
        std::cout << std::fixed << verifierTimeOutputs[i] << std::setprecision(9) << ", ";
    }
    std::cout << std::endl;
    std::cout << "Communication: ";
    for (size_t i = 0; i < 10; ++i)
    {
        std::cout << std::fixed << commTimeOutputs[i] << std::setprecision(9) << ", ";
    }
    std::cout << std::endl;

    j = 0;
    std::cout << "FLIOP WAN Delay" << std::endl;
    for (size_t i = 2u; i <= mInputLength; i *= 2u)
    {
        double fliopTime = FindFLIOPDelayRecursive(i, mTotalWANTimes);
        std::cout << "Length: " << i << " / Time : " << std::fixed << fliopTime + Network::WANBaseDelayMs
                  << std::setprecision(9) << std::endl;
        outputs[j] = fliopTime + Network::WANBaseDelayMs;

        proverTimeOutputs[j] = FindFLIOPDelayRecursive(i, mProverTimes);
        verifierTimeOutputs[j] = FindFLIOPDelayRecursive(i, mVerifierTimes);
        commTimeOutputs[j] = FindFLIOPDelayRecursive(i, mWANTimes) + Network::WANBaseDelayMs;
        ++j;
    }
    std::cout << "Total: ";
    for (size_t i = 0; i < 10; ++i)
    {
        std::cout << std::fixed << outputs[i] << std::setprecision(9) << ", ";
    }
    std::cout << std::endl;
    std::cout << "Prover: ";
    for (size_t i = 0; i < 10; ++i)
    {
        std::cout << std::fixed << proverTimeOutputs[i] << std::setprecision(9) << ", ";
    }
    std::cout << std::endl;
    std::cout << "Verifier: ";
    for (size_t i = 0; i < 10; ++i)
    {
        std::cout << std::fixed << verifierTimeOutputs[i] << std::setprecision(9) << ", ";
    }
    std::cout << std::endl;
    std::cout << "Communication: ";
    for (size_t i = 0; i < 10; ++i)
    {
        std::cout << std::fixed << commTimeOutputs[i] << std::setprecision(9) << ", ";
    }
    std::cout << std::endl;

    delete[] mProverTimes;
    mProverTimes = (double*)0;
    delete[] mVerifierTimes;
    mVerifierTimes = (double*)0;
    delete[] mLANTimes;
    mLANTimes = (double*)0;
    delete[] mWANTimes;
    mWANTimes = (double*)0;
    delete[] mTotalLANTimes;
    mTotalLANTimes = (double*)0;
    delete[] mTotalWANTimes;
    mTotalWANTimes = (double*)0;
}

template <typename Int>
void TwoPartySimulation<Int>::FindBestFLIOPCoefficientSchedule(const size_t inputLength, const size_t maxLambda)
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
        BestSchedule best = FindBestFLIOPScheduleRecursive(i, totalLANTimes, maxLambda, inputLength);
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
        BestSchedule best = FindBestFLIOPScheduleRecursive(i, totalWANTimes, maxLambda, inputLength);
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
