#include "math\mpint64.hpp"
#include "experiments\three_party_computation.hpp"

#include "math\sha512.hpp"

int main(int argc, char* argv[])
{
    ThreePC<Mpint64>::FindBestFLIOPSchedule(1024, 32);
    ThreePC<Mpint64>::FindBestFLIOPCoefficientSchedule(1024, 32);
    
    system("pause");
    return 0;
}
