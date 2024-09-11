#include "math\mpint64.hpp"
#include "experiments\two_party_computation.hpp"

int main(int argc, char* argv[])
{
    TwoPC<Mpint64>::ExperimentFLPCP();
    TwoPC<Mpint64>::ExperimentFLPCPWithPrecompute();
    TwoPC<Mpint64>::ExperimentFLPCPSqrt();
    TwoPC<Mpint64>::ExperimentFLPCPSqrtWithPrecompute();
    TwoPC<Mpint64>::ExperimentFLPCPCoefficient();
    TwoPC<Mpint64>::ExperimentFLPCPCoefficientSqrt();
    system("pause");
    return 0;
}
