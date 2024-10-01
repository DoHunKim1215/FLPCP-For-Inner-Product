#include "math\mpint64.hpp"
#include "experiments\multi_party_computation.hpp"

#include "math\sha512.hpp"

int main(int argc, char* argv[])
{
    MPC<Mpint64> mpc(23571113, 65536, 64, 10);
    mpc.FindBestFLIOPSchedule(true);
    system("pause");
    return 0;
}
