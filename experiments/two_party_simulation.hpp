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
    ~TwoPartySimulation();

    void FindBestFLIOPSchedule();

private:
    size_t mInputLength;
    size_t mMaxLambda;
    uint32_t mSeed;

    // (mMaxLambda + 1) x (mInputLength + 1)
    OneRoundMeasurement** mOneRoundMeasures;
    double** mTotalLANTimes;
    double** mTotalWANTimes;

    OneRoundMeasurement SimulateFLIOPOneRound(size_t inputLength, size_t compressFactor);
    OneRoundMeasurement SimulateFLIOPCoefficientOneRound(size_t inputLength, size_t compressFactor);
    IOPSchedule FindBestLANSchedule(const size_t inputLength);
    IOPSchedule FindBestLANScheduleRecursive(const size_t inputLength);
    IOPSchedule FindBestWANSchedule(const size_t inputLength);
    IOPSchedule FindBestWANScheduleRecursive(const size_t inputLength);
    IOPSchedule FindLANDelay(const size_t inputLength);
    IOPSchedule FindLANDelayRecursive(const size_t inputLength);
    IOPSchedule FindWANDelay(const size_t inputLength);
    IOPSchedule FindWANDelayRecursive(const size_t inputLength);
    void CalculateOneRoundTimesRecursive(const size_t inputLength);
};

template <typename Int>
TwoPartySimulation<Int>::TwoPartySimulation(const uint32_t seed, const size_t inputLength, const size_t maxLambda)
{
    mSeed = seed;
    mInputLength = inputLength;
    mMaxLambda = maxLambda;

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

template <typename Int>
TwoPartySimulation<Int>::~TwoPartySimulation()
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
OneRoundMeasurement TwoPartySimulation<Int>::SimulateFLIOPOneRound(size_t inputLength, size_t compressFactor)
{
    Int::SetSeed(mSeed);

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
        proverTime = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

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
        verifierTime = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
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
        proverTime = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

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
        verifierTime = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    }

    if (!isValid)
    {
        std::cerr << "FLIOP is aborted!!" << std::endl;
        exit(-1);
    }

    return OneRoundMeasurement(proverTime, verifierTime, LANTime, WANTime);
}

template <typename Int>
OneRoundMeasurement TwoPartySimulation<Int>::SimulateFLIOPCoefficientOneRound(size_t inputLength, size_t compressFactor)
{
    Int::SetSeed(mSeed);

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
        proverTime = std::chrono::duration_cast<std::chrono::nanoseconds>(end_prover - start_prover).count();

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
        verifierTime = std::chrono::duration_cast<std::chrono::nanoseconds>(end_verifier - start_verifier).count();
    }
    else
    {
        // Prover
        auto start = std::chrono::high_resolution_clock::now();
        Proof<Int> proof = InnerProductCircuit<Int>::MakeCoefficientProof(op0.data(), op1.data(), op0.size(), 1);
        auto end = std::chrono::high_resolution_clock::now();
        proverTime = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

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
        verifierTime = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    }

    if (!isValid)
    {
        std::cerr << "FLIOP is aborted!!" << std::endl;
        exit(-1);
    }

    return OneRoundMeasurement(proverTime, verifierTime, LANTime, WANTime);
}

template <typename Int> IOPSchedule TwoPartySimulation<Int>::FindBestLANSchedule(const size_t inputLength)
{
    IOPSchedule best = FindBestLANScheduleRecursive(inputLength);
    std::reverse(best.lambdas.begin(), best.lambdas.end());
    std::reverse(best.trace.begin(), best.trace.end());
    return best;
}

template <typename Int>
IOPSchedule TwoPartySimulation<Int>::FindBestLANScheduleRecursive(const size_t inputLength)
{
    if (inputLength <= 2)
    {
        std::vector<size_t> lambdas(1);
        std::vector<OneRoundMeasurement> trace(1);
        lambdas[0] = 2;
        trace[0] = mOneRoundMeasures[2][inputLength];
        double totalTime = trace[0].proverTimeNs + trace[0].verifierTimeNs + trace[0].communicationTimeNsInLAN;
        return IOPSchedule(totalTime, lambdas, trace);
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

template <typename Int> IOPSchedule TwoPartySimulation<Int>::FindBestWANSchedule(const size_t inputLength)
{
    IOPSchedule best = FindBestWANScheduleRecursive(inputLength);
    std::reverse(best.lambdas.begin(), best.lambdas.end());
    std::reverse(best.trace.begin(), best.trace.end());
    return best;
}

template <typename Int> IOPSchedule TwoPartySimulation<Int>::FindBestWANScheduleRecursive(const size_t inputLength)
{
    if (inputLength <= 2)
    {
        std::vector<size_t> lambdas(1);
        std::vector<OneRoundMeasurement> trace(1);
        lambdas[0] = 2;
        trace[0] = mOneRoundMeasures[2][inputLength];
        double totalTime = trace[0].proverTimeNs + trace[0].verifierTimeNs + trace[0].communicationTimeNsInWAN;
        return IOPSchedule(totalTime, lambdas, trace);
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

template <typename Int> IOPSchedule TwoPartySimulation<Int>::FindLANDelay(const size_t inputLength)
{
    IOPSchedule schedule = FindLANDelayRecursive(inputLength);
    std::reverse(schedule.trace.begin(), schedule.trace.end());
    return schedule;
}

template <typename Int> IOPSchedule TwoPartySimulation<Int>::FindLANDelayRecursive(const size_t inputLength)
{
    if (inputLength <= 2)
    {
        std::vector<size_t> lambdas(1);
        std::vector<OneRoundMeasurement> trace(1);
        lambdas[0] = 2;
        trace[0] = mOneRoundMeasures[2][inputLength];
        double totalTime = trace[0].proverTimeNs + trace[0].verifierTimeNs + trace[0].communicationTimeNsInLAN;
        return IOPSchedule(totalTime, lambdas, trace);
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

template <typename Int> IOPSchedule TwoPartySimulation<Int>::FindWANDelay(const size_t inputLength)
{
    IOPSchedule schedule = FindWANDelayRecursive(inputLength);
    std::reverse(schedule.trace.begin(), schedule.trace.end());
    return schedule;
}

template <typename Int> IOPSchedule TwoPartySimulation<Int>::FindWANDelayRecursive(const size_t inputLength)
{
    if (inputLength <= 2)
    {
        std::vector<size_t> lambdas(1);
        std::vector<OneRoundMeasurement> trace(1);
        lambdas[0] = 2;
        trace[0] = mOneRoundMeasures[2][inputLength];
        double totalTime = trace[0].proverTimeNs + trace[0].verifierTimeNs + trace[0].communicationTimeNsInWAN;
        return IOPSchedule(totalTime, lambdas, trace);
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

template <typename Int> void TwoPartySimulation<Int>::CalculateOneRoundTimesRecursive(const size_t inputLength)
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

template <typename Int>
void TwoPartySimulation<Int>::FindBestFLIOPSchedule()
{
    const size_t nStep = (size_t)std::log2(mInputLength / (double)2) + 1;

    std::cout.sync_with_stdio(false);

    CalculateOneRoundTimesRecursive(mInputLength);
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
        outputs[j] = best.time * 1e-6 + Network::LANBaseDelayMs;

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
        commTimeOutputs[j] = commTimeOutputs[j] * 1e-6 + Network::LANBaseDelayMs;

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
        std::cout << " / Min time : " << std::fixed << best.time * 1e-6 + Network::WANBaseDelayMs
                  << std::setprecision(9);
        std::cout << " / Best schedule : ";
        for (size_t k = 0; k < best.lambdas.size(); ++k)
        {
            std::cout << best.lambdas[k] << " ";
        }
        std::cout << std::endl;

        outputs[j] = best.time * 1e-6 + Network::WANBaseDelayMs;

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
        commTimeOutputs[j] = commTimeOutputs[j] * 1e-6 + Network::WANBaseDelayMs;

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
        std::cout << "Length: " << i << " / Time : " << std::fixed << schedule.time * 1e-6 + Network::LANBaseDelayMs
                  << std::setprecision(9) << std::endl;
        outputs[j] = schedule.time * 1e-6 + Network::LANBaseDelayMs;

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
        commTimeOutputs[j] = commTimeOutputs[j] * 1e-6 + Network::LANBaseDelayMs;

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
        std::cout << "Length: " << i << " / Time : " << std::fixed << schedule.time * 1e-6 + Network::WANBaseDelayMs
                  << std::setprecision(9) << std::endl;
        outputs[j] = schedule.time * 1e-6 + Network::WANBaseDelayMs;

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
        commTimeOutputs[j] = commTimeOutputs[j] * 1e-6 + Network::WANBaseDelayMs;

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
