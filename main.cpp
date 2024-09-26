#include "math\mpint64.hpp"
#include "experiments\two_party_simulation.hpp"

#include "math\sha512.hpp"

int main(int argc, char* argv[])
{
    TwoPartySimulation<Mpint64> tps(23571113, 4096, 32);
    tps.FindBestFLIOPSchedule();
    system("pause");
    return 0;
}
