#include "math\mpint64.hpp"
#include "experiments\two_party_computation.hpp"

int main(int argc, char* argv[])
{
    TwoPC<Mpint64>::ExperimentFLPCPWithPrecompute();
    system("pause");
    return 0;
}
