#include <cassert>
#include <chrono>
#include <iostream>
#include <limits>
#include <random>

#include "performance_measurement.hpp"
#include "..\math\mpint32.hpp"

uint32_t PerformanceMeasurement::ReduceInt32To31(uint32_t x)
{
    uint32_t q = x >> 31;
    uint32_t r = x - (q << 31);
    if (q > 0)
    {
        r += 1;
    }
    if (r >= BASE31)
    {
        r -= BASE31;
    }
    return r;
}

uint32_t PerformanceMeasurement::ReduceInt32To31Simply(uint32_t x)
{
    uint32_t r = (x >> 31) + (x & BASE31);
    while (r >= BASE31)
    {
        r -= BASE31;
    }
    return r;
}

uint64_t PerformanceMeasurement::ReduceInt64To31(uint64_t x)
{
    uint64_t q = x >> 31;
    uint64_t r = x - (q << 31);
    while (q > 0)
    {
        uint64_t qNew = q >> 31;
        r += q - (qNew << 31);
        q = qNew;
    }
    while (r >= BASE31)
    {
        r -= BASE31;
    }
    return r;
}

uint64_t PerformanceMeasurement::ReduceInt64To31Simply(uint64_t x)
{
    uint64_t r = (x >> 31) + (x & BASE31);
    while (r >= BASE31)
    {
        r -= BASE31;
    }
    return r;
}

uint64_t PerformanceMeasurement::ReduceInt64To61(uint64_t x)
{
    uint64_t q = x >> 61;
    uint64_t r = x - (q << 61);
    while (q > 0)
    {
        uint64_t qNew = q >> 61;
        r += q - (qNew << 61);
        q = qNew;
    }
    while (r >= BASE61)
    {
        r -= BASE61;
    }
    return r;
}

uint64_t PerformanceMeasurement::ReduceInt64To61Simply(uint64_t x)
{
    uint64_t r = (x >> 61) + (x & BASE61);
    while (r >= BASE61)
    {
        r -= BASE61;
    }
    return r;
}

uint64_t PerformanceMeasurement::ReduceInt64To61SimplyIncompletely(uint64_t x)
{
    return (x >> 61) + (x & BASE61);
}

void PerformanceMeasurement::ExtendedEuclidean32(uint32_t u, uint32_t* u1, uint32_t* u2, uint32_t* u3)
{
    *u1 = 1;
    *u2 = 0;
    *u3 = u;
    uint64_t v1 = 0;
    uint64_t v2 = 1;
    uint64_t v3 = BASE31;
    uint64_t q;
    uint64_t t1;
    uint64_t t2;
    uint64_t t3;
    do
    {
        q = *u3 / v3;
        t1 = (BASE31 - ((q * v1) % BASE31) + (*u1)) % BASE31;
        t2 = (BASE31 - ((q * v2) % BASE31) + (*u2)) % BASE31;
        t3 = (BASE31 - ((q * v3) % BASE31) + (*u3)) % BASE31;
        (*u1) = (uint32_t)v1;
        (*u2) = (uint32_t)v2;
        (*u3) = (uint32_t)v3;
        v1 = t1;
        v2 = t2;
        v3 = t3;
    } while (v3 != 0 && v3 != BASE31);
}

uint32_t PerformanceMeasurement::ExtEuclideanInverse32(uint32_t a)
{
    uint32_t u1;
    uint32_t u2;
    uint32_t u3;
    ExtendedEuclidean32(a, &u1, &u2, &u3);
    if (u3 == 1)
        return ReduceInt32To31Simply(u1);
    else
        return 0;
}

uint32_t PerformanceMeasurement::Pow(uint32_t base, uint32_t exp)
{
    uint64_t result = 1u;
    while (exp > 0)
    {
        if (exp % 2u == 1u)
        {
            result = (result * (uint64_t)base) % BASE31;
        }
        exp = exp >> 1;
        base = ((uint64_t)base * (uint64_t)base) % BASE31;
    }
    return (uint32_t)result;
}

uint32_t PerformanceMeasurement::BinaryExpInverse32(uint32_t a)
{
    return Pow(a, BASE31 - 2);
}

uint64_t PerformanceMeasurement::Multiply(uint64_t x, uint64_t y)
{
    uint64_t hi_x = x >> 32;
    uint64_t hi_y = y >> 32;
    uint64_t low_x = x & MASK;
    uint64_t low_y = y & MASK;

    uint64_t piece1 = ReduceInt64To61SimplyIncompletely((hi_x * hi_y) << 3);
    uint64_t z = (hi_x * low_y + hi_y * low_x);
    uint64_t hi_z = z >> 32;
    uint64_t low_z = z & MASK;

    uint64_t piece2 = ReduceInt64To61SimplyIncompletely((hi_z << 3) + ReduceInt64To61SimplyIncompletely((low_z << 32)));
    uint64_t piece3 = ReduceInt64To61SimplyIncompletely(low_x * low_y);
    uint64_t result = ReduceInt64To61SimplyIncompletely(piece1 + piece2 + piece3);

    return result;
}

void PerformanceMeasurement::ExtendedEuclidean64(uint64_t u, uint64_t* u1, uint64_t* u2, uint64_t* u3)
{
    *u1 = 1;
    *u2 = 0;
    *u3 = u;
    uint64_t v1 = 0;
    uint64_t v2 = 1;
    uint64_t v3 = BASE61;
    uint64_t q;
    uint64_t t1;
    uint64_t t2;
    uint64_t t3;
    do
    {
        q = *u3 / v3;
        t1 = ReduceInt64To61Simply(BASE61 - Multiply(q, v1) + (*u1));
        t2 = ReduceInt64To61Simply(BASE61 - Multiply(q, v2) + (*u2));
        t3 = ReduceInt64To61Simply(BASE61 - Multiply(q, v3) + (*u3));
        (*u1) = v1;
        (*u2) = v2;
        (*u3) = v3;
        v1 = t1;
        v2 = t2;
        v3 = t3;
    } while (v3 != 0 && v3 != BASE61);
}

uint64_t PerformanceMeasurement::ExtEuclideanInverse64(uint64_t a)
{
    uint64_t u1;
    uint64_t u2;
    uint64_t u3;
    ExtendedEuclidean64(a, &u1, &u2, &u3);
    if (u3 == 1)
        return ReduceInt64To61Simply(u1);
    else
        return 0;
}

uint64_t PerformanceMeasurement::Pow64(uint64_t base, uint64_t exp)
{
    uint64_t result = 1u;
    while (exp > 0)
    {
        if (exp % 2u == 1u)
        {
            result = Multiply(result, base);
        }
        exp = exp >> 1;
        base = Multiply(base, base);
    }
    return result;
}

uint64_t PerformanceMeasurement::BinaryExpInverse64(uint64_t a)
{
    return Pow64(a, BASE61 - 2);
}

void PerformanceMeasurement::CompareInt32To31Reduction()
{
    std::cout << "Comparison for Reduction Algorithm Performance on Single-Precision Unsigned Integer  over Z_{2^31-1}"
              << std::endl;

    const size_t nIteration = 10000;

    std::mt19937 randomGenerator = std::mt19937(10);
    std::uniform_int_distribution<uint32_t> dist = std::uniform_int_distribution<uint32_t>(0u, std::numeric_limits<uint32_t>::max());

    std::unique_ptr<uint32_t[]> targets = std::make_unique<uint32_t[]>(nIteration);
    for (size_t i = 0; i < nIteration; ++i)
    {
        targets[i] = dist(randomGenerator);
    }

    std::unique_ptr<uint32_t[]> trueRes = std::make_unique<uint32_t[]>(nIteration);
    std::unique_ptr<uint32_t[]> res = std::make_unique<uint32_t[]>(nIteration);
    std::unique_ptr<uint32_t[]> resTcode = std::make_unique<uint32_t[]>(nIteration);

    auto start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < nIteration; ++i)
    {
        for (size_t j = 0; j < nIteration; ++j)
        {
            trueRes[j] = targets[j] % BASE31;
        }
    }
    auto end = std::chrono::high_resolution_clock::now();
    double time_taken_naive = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

    start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < nIteration; ++i)
    {
        for (size_t j = 0; j < nIteration; ++j)
        {
            res[j] = PerformanceMeasurement::ReduceInt32To31(targets[j]);
        }
    }
    end = std::chrono::high_resolution_clock::now();
    double time_taken_alg = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

    start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < nIteration; ++i)
    {
        for (size_t j = 0; j < nIteration; ++j)
        {
            resTcode[j] = PerformanceMeasurement::ReduceInt32To31Simply(targets[j]);
        }
    }
    end = std::chrono::high_resolution_clock::now();
    double time_taken_tcode = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

    for (size_t j = 0; j < nIteration; ++j)
    {
        if (res[j] != trueRes[j] || resTcode[j] != trueRes[j])
        {
            std::cout << "Mersenne Prime Reduction Gives Wrong Result!!!!" << std::endl;
            return;
        }
    }

    std::cout << "Naive Reduction : " << time_taken_naive * 1e-6 << "ms" << std::endl;
    std::cout << "Mersenne Prime Reduction : " << time_taken_alg * 1e-6 << "ms" << std::endl;
    std::cout << "Tcode version : " << time_taken_tcode * 1e-6 << "ms" << std::endl;
    std::cout << std::endl;
}

void PerformanceMeasurement::CompareInt64To31Reduction()
{
    std::cout << "Comparison for Reduction Algorithm Performance on Double-Precision Unsigned Integer over Z_{2^31-1}"
              << std::endl;
    
    const size_t nIteration = 10000;

    std::mt19937 randomGenerator = std::mt19937(10);
    std::uniform_int_distribution<uint64_t> dist =
        std::uniform_int_distribution<uint64_t>(0u, std::numeric_limits<uint64_t>::max());

    std::unique_ptr<uint64_t[]> targets = std::make_unique<uint64_t[]>(nIteration);
    for (size_t i = 0; i < nIteration; ++i)
    {
        targets[i] = dist(randomGenerator);
    }

    std::unique_ptr<uint64_t[]> trueRes = std::make_unique<uint64_t[]>(nIteration);
    std::unique_ptr<uint64_t[]> res = std::make_unique<uint64_t[]>(nIteration);
    std::unique_ptr<uint64_t[]> resTcode = std::make_unique<uint64_t[]>(nIteration);

    auto start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < nIteration; ++i)
    {
        for (size_t j = 0; j < nIteration; ++j)
        {
            trueRes[j] = targets[j] % BASE31;
        }
    }
    auto end = std::chrono::high_resolution_clock::now();
    double time_taken_naive = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

    start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < nIteration; ++i)
    {
        for (size_t j = 0; j < nIteration; ++j)
        {
            res[j] = PerformanceMeasurement::ReduceInt64To31(targets[j]);
        }
    }
    end = std::chrono::high_resolution_clock::now();
    double time_taken_alg = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

    start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < nIteration; ++i)
    {
        for (size_t j = 0; j < nIteration; ++j)
        {
            resTcode[j] = PerformanceMeasurement::ReduceInt64To31Simply(targets[j]);
        }
    }
    end = std::chrono::high_resolution_clock::now();
    double time_taken_tcode = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

    for (size_t j = 0; j < nIteration; ++j)
    {
        if (res[j] != trueRes[j] || resTcode[j] != trueRes[j])
        {
            std::cout << "Mersenne Prime Reduction Gives Wrong Result!!!!" << std::endl;
            return;
        }
    }

    std::cout << "Naive Reduction : " << time_taken_naive * 1e-6 << "ms" << std::endl;
    std::cout << "Mersenne Prime Reduction : " << time_taken_alg * 1e-6 << "ms" << std::endl;
    std::cout << "Tcode version : " << time_taken_tcode * 1e-6 << "ms" << std::endl;
    std::cout << std::endl;
}

void PerformanceMeasurement::CompareInt64To61Reduction()
{
    std::cout << "Comparison for Reduction Algorithm Performance on Double-Precision Unsigned Integer over Z_{2^61-1}"
              << std::endl;

    const size_t nIteration = 10000;

    std::mt19937 randomGenerator = std::mt19937(10);
    std::uniform_int_distribution<uint64_t> dist =
        std::uniform_int_distribution<uint64_t>(0u, std::numeric_limits<uint64_t>::max());

    std::unique_ptr<uint64_t[]> targets = std::make_unique<uint64_t[]>(nIteration);
    for (size_t i = 0; i < nIteration; ++i)
    {
        targets[i] = dist(randomGenerator);
    }

    std::unique_ptr<uint64_t[]> trueRes = std::make_unique<uint64_t[]>(nIteration);
    std::unique_ptr<uint64_t[]> res = std::make_unique<uint64_t[]>(nIteration);
    std::unique_ptr<uint64_t[]> resTcode = std::make_unique<uint64_t[]>(nIteration);

    auto start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < nIteration; ++i)
    {
        for (size_t j = 0; j < nIteration; ++j)
        {
            trueRes[j] = targets[j] % BASE61;
        }
    }
    auto end = std::chrono::high_resolution_clock::now();
    double time_taken_naive = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

    start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < nIteration; ++i)
    {
        for (size_t j = 0; j < nIteration; ++j)
        {
            res[j] = PerformanceMeasurement::ReduceInt64To61(targets[j]);
        }
    }
    end = std::chrono::high_resolution_clock::now();
    double time_taken_alg = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

    start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < nIteration; ++i)
    {
        for (size_t j = 0; j < nIteration; ++j)
        {
            resTcode[j] = PerformanceMeasurement::ReduceInt64To61Simply(targets[j]);
        }
    }
    end = std::chrono::high_resolution_clock::now();
    double time_taken_tcode = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

    for (size_t j = 0; j < nIteration; ++j)
    {
        if (res[j] != trueRes[j] || resTcode[j] != trueRes[j])
        {
            std::cout << "Mersenne Prime Reduction Gives Wrong Result!!!!" << std::endl;
            return;
        }
    }

    std::cout << "Naive Reduction : " << time_taken_naive * 1e-6 << "ms" << std::endl;
    std::cout << "Mersenne Prime Reduction : " << time_taken_alg * 1e-6 << "ms" << std::endl;
    std::cout << "Tcode version : " << time_taken_tcode * 1e-6 << "ms" << std::endl;
    std::cout << std::endl;
}

void PerformanceMeasurement::CompareInt31Inversion()
{
    std::cout << "Comparison for Inversion Performance over 31-bit Mersenne Prime Field" << std::endl;

    const size_t nIteration = 10000;

    std::mt19937 randomGenerator = std::mt19937(10);
    std::uniform_int_distribution<uint32_t> dist = std::uniform_int_distribution<uint32_t>(0u, BASE31 - 1);

    std::unique_ptr<uint32_t[]> targets = std::make_unique<uint32_t[]>(nIteration);
    for (size_t i = 0; i < nIteration; ++i)
    {
        targets[i] = dist(randomGenerator);
    }

    std::unique_ptr<uint32_t[]> expRes = std::make_unique<uint32_t[]>(nIteration);
    std::unique_ptr<uint32_t[]> euclideanRes = std::make_unique<uint32_t[]>(nIteration);

    auto start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < nIteration; ++i)
    {
        for (size_t j = 0; j < nIteration; ++j)
        {
            expRes[j] = PerformanceMeasurement::BinaryExpInverse32(targets[j]);
        }
    }
    auto end = std::chrono::high_resolution_clock::now();
    double time_taken_exp = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

    start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < nIteration; ++i)
    {
        for (size_t j = 0; j < nIteration; ++j)
        {
            euclideanRes[j] = PerformanceMeasurement::ExtEuclideanInverse32(targets[j]);
        }
    }
    end = std::chrono::high_resolution_clock::now();
    double time_taken_euclidean = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

    for (size_t i = 0; i < nIteration; ++i)
    {
        if (expRes[i] != euclideanRes[i])
        {
            std::cout << "Two algorithm give different result!!!!" << std::endl;
            return;
        }
    }

    std::cout << "Binary Exponentiation : " << time_taken_exp * 1e-6 << "ms" << std::endl;
    std::cout << "Extended Euclidean Algorithm : " << time_taken_euclidean * 1e-6 << "ms" << std::endl;
    std::cout << std::endl;
}

void PerformanceMeasurement::CompareInt61Inversion()
{
    std::cout << "Comparison for Inversion Performance over 61-bit Mersenne Prime Field" << std::endl;

    const size_t nIteration = 10000;

    std::mt19937 randomGenerator = std::mt19937(10);
    std::uniform_int_distribution<uint64_t> dist = std::uniform_int_distribution<uint64_t>(0u, BASE61 - 1);

    std::unique_ptr<uint64_t[]> targets = std::make_unique<uint64_t[]>(nIteration);
    for (size_t i = 0; i < nIteration; ++i)
    {
        targets[i] = dist(randomGenerator);
    }

    std::unique_ptr<uint64_t[]> expRes = std::make_unique<uint64_t[]>(nIteration);
    std::unique_ptr<uint64_t[]> euclideanRes = std::make_unique<uint64_t[]>(nIteration);

    auto start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < nIteration; ++i)
    {
        for (size_t j = 0; j < nIteration; ++j)
        {
            expRes[j] = PerformanceMeasurement::BinaryExpInverse64(targets[j]);
        }
    }
    auto end = std::chrono::high_resolution_clock::now();
    double time_taken_exp = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

    start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < nIteration; ++i)
    {
        for (size_t j = 0; j < nIteration; ++j)
        {
            euclideanRes[j] = PerformanceMeasurement::ExtEuclideanInverse64(targets[j]);
        }
    }
    end = std::chrono::high_resolution_clock::now();
    double time_taken_euclidean = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

    for (size_t i = 0; i < nIteration; ++i)
    {
        if (expRes[i] != euclideanRes[i])
        {
            std::cout << "Two algorithm give different result!!!!" << std::endl;
            return;
        }
    }

    std::cout << "Binary Exponentiation : " << time_taken_exp * 1e-6 << "ms" << std::endl;
    std::cout << "Extended Euclidean Algorithm : " << time_taken_euclidean * 1e-6 << "ms" << std::endl;
    std::cout << std::endl;
}
