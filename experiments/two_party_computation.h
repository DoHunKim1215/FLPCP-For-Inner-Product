#ifndef TWO_PARTY_COMPUTATION_H
#define TWO_PARTY_COMPUTATION_H

struct FLPCPMeasurement
{
    size_t proofLength;
    size_t nQueries;
    double proverTime;
    double verifierTime;
    bool isVaild;
};

class TwoPC
{
public:
    static FLPCPMeasurement FLPCP(size_t inputLength, size_t nGGate);
    static void ExperimentFLPCP();
    static void ExperimentFLPCPSqrt();
    static FLPCPMeasurement FLPCPCoefficient(size_t inputLength, size_t nGGate);
    static void ExperimentFLPCPCoefficient();
    static void ExperimentFLPCPCoefficientSqrt();
    static FLPCPMeasurement FLIOP(const size_t inputLength, const size_t compressFactor);
    static void ExperimentFLIOP();
    static FLPCPMeasurement FLIOPCoefficient(const size_t inputLength, const size_t compressFactor);
    static void ExperimentFLIOPCoefficient();
};

#endif
