#include "math\mpint64.hpp"
#include "experiments\two_party_computation.hpp"
#include "experiments\multi_party_computation.hpp"

#include "math\sha512.hpp"

int main(int argc, char* argv[])
{
    MPC<Mpint64> mpc(23571113, 65536, 64, 6);
    mpc.FindBestFLIOPSchedule(false, 100);
    system("pause");
    return 0;
}
