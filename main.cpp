#include <iostream>
#include <iomanip>
#include <stdexcept>
#include <string>
#include <utility>
#include <random>
#include <chrono>
#include <cassert>

#include "experiments\performance_measurement.h"
#include "experiments\two_party_computation.h"
#include "experiments\three_party_computation.h"

int main(int argc, char* argv[])
{
    TwoPC::ExperimentFLIOPCoefficient();
    system("pause");
    return 0;
}
