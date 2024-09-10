#include "math\mpint64.hpp"
#include "experiments\three_party_computation.hpp"

int main(int argc, char* argv[])
{
    ThreePC<Mpint64>::FindBestFLIOPCoefficientSchedule(1024, 32);
    system("pause");
    return 0;
}
