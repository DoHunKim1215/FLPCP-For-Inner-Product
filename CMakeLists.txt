cmake_minimum_required (VERSION 3.8)

if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("FLPCP")

add_executable (FLPCP "main.cpp"  "math/mpint32.h" "math/mpint32.cpp" "circuit/inner_product_circuit.cpp" "circuit/inner_product_circuit.h" "math/polynomial.cpp" "math/polynomial.h" "unit/proof.cpp" "unit/proof.h" "unit/query.cpp" "unit/query.h" "unit/interactive_proof.cpp" "unit/interactive_proof.h" "experiments/two_party_computation.cpp" "experiments/two_party_computation.h" "experiments/three_party_computation.cpp" "experiments/three_party_computation.h" "experiments/performance_measurement.cpp" "experiments/performance_measurement.h" "math/mpint64.cpp" "math/mpint64.h"  )

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET FLPCP PROPERTY CXX_STANDARD 20)
endif()
