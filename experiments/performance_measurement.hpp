#ifndef PERFORMANCE_MEASUREMENT_H
#define PERFORMANCE_MEASUREMENT_H

class PerformanceMeasurement
{
public:
    static const uint32_t BASE31 = 0x7FFFFFFF;         // 2^31 - 1
    static const uint64_t BASE61 = 0x1FFFFFFFFFFFFFFF; // 2^61 - 1
    static const uint64_t MASK = 0xFFFFFFFF;           // 2^32 - 1

    static uint32_t ReduceInt32To31(uint32_t x);
    static uint32_t ReduceInt32To31Simply(uint32_t x);
    static uint64_t ReduceInt64To31(uint64_t x);
    static uint64_t ReduceInt64To31Simply(uint64_t x);

    static uint64_t ReduceInt64To61(uint64_t x);
    static uint64_t ReduceInt64To61Simply(uint64_t x);
    static uint64_t ReduceInt64To61SimplyIncompletely(uint64_t x);

    static void ExtendedEuclidean32(uint32_t u, uint32_t* u1, uint32_t* u2, uint32_t* u3);
    static uint32_t ExtEuclideanInverse32(uint32_t a);
    static uint32_t Pow(uint32_t base, uint32_t exp);
    static uint32_t BinaryExpInverse32(uint32_t a);

    static uint64_t Multiply(uint64_t x, uint64_t y);

    static void ExtendedEuclidean64(uint64_t u, uint64_t* u1, uint64_t* u2, uint64_t* u3);
    static uint64_t ExtEuclideanInverse64(uint64_t a);
    static uint64_t Pow64(uint64_t base, uint64_t exp);
    static uint64_t BinaryExpInverse64(uint64_t a);

    static void CompareInt32To31Reduction();
    static void CompareInt64To31Reduction();
    static void CompareInt64To61Reduction();
    static void CompareInt31Inversion();
    static void CompareInt61Inversion();
};

#endif
