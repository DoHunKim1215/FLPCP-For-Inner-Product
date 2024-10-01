cmake_minimum_required (VERSION 3.8)

if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("FLPCP")

add_executable (FLPCP "main.cpp"  "math/mpint32.hpp" "math/mpint32.cpp"  "circuit/inner_product_circuit.hpp"  "math/polynomial.hpp"  "unit/proof.hpp"  "unit/query.hpp"  "unit/interactive_proof.hpp"  "experiments/two_party_computation.hpp"  "experiments/multi_party_computation.hpp" "experiments/performance_measurement.cpp" "experiments/performance_measurement.hpp" "math/mpint64.cpp" "math/mpint64.hpp"   )

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET FLPCP PROPERTY CXX_STANDARD 20)
endif()
