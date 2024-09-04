#ifndef THREE_PARTY_COMPUTATION_H
#define THREE_PARTY_COMPUTATION_H

#include <vector>

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

class ThreePC
{
public:
    static double GetLANDelay(size_t payloadBytes);
    static double GetWANDelay(size_t payloadBytes);
    static void FLPCP(size_t inputLength, size_t nGGate);
    static void FLPCPCoefficient(size_t inputLength, size_t nGGate);
    static void FLIOP(const size_t inputLength, const size_t compressFactor);
    static void FLIOPCoefficient(const size_t inputLength, const size_t compressFactor);
    static OneRoundMeasurement SimulateFLIOPOneRound(size_t inputLength, size_t compressFactor);
    static OneRoundMeasurement SimulateFLIOPCoefficientOneRound(size_t inputLength, size_t compressFactor);
    static BestSchedule FindBestFLIOPScheduleRecursive(const size_t inputLength, double* totalTimes,
                                                       const size_t maxLambda, const size_t maxLength);
    static double FindFLIOPDelayRecursive(const size_t inputLength, double* totalTimes, const size_t maxLength);
    static void FindBestFLIOPSchedule(const size_t inputLength, const size_t maxLambda);
    static void FindBestFLIOPCoefficientSchedule(const size_t inputLength, const size_t maxLambda);
};

#endif
