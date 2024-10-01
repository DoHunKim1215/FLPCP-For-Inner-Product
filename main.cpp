#include "math\mpint64.hpp"
#include "experiments\two_party_computation.hpp"

#include "math\sha512.hpp"

int main(int argc, char* argv[])
{
    TwoPC<Mpint64>::ExperimentFLPCP();
    system("pause");
    return 0;
}
