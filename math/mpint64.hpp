#ifndef MPINT64_H
#define MPINT64_H

#include <random>
#include <stdint.h>

/* 64-bit Integer over Mersenne Prime Field : Z_{2^61 - 1} */
class Mpint64
{
public:
    Mpint64();
    Mpint64(uint64_t value);

    static const uint64_t GetBase();
    static const uint32_t GetSeed();
    const uint64_t GetValue() const;

    static void SetSeed(uint32_t seed);

    static Mpint64 GenerateRandom();
    static Mpint64 GenerateRandomAbove(uint64_t min);
    static void Reverse(Mpint64* begin, Mpint64* end);

    Mpint64 Invert() const;
    Mpint64 Pow(uint64_t exp) const;

    Mpint64 operator+(const Mpint64& op);
    Mpint64& operator+=(const Mpint64& op);
    Mpint64 operator-(const Mpint64& op);
    Mpint64& operator-=(const Mpint64& op);
    Mpint64 operator-();
    Mpint64 operator*(const Mpint64& op);
    Mpint64& operator*=(const Mpint64& op);
    Mpint64 operator/(const Mpint64& op);
    Mpint64& operator/=(const Mpint64& op);
    bool operator==(const Mpint64& op);
    bool operator>(const Mpint64& op);
    bool operator<(const Mpint64& op);
    bool operator>=(const Mpint64& op);
    bool operator<=(const Mpint64& op);
    bool operator!=(const Mpint64& op);

private:
    static const uint64_t BASE = 0x1FFFFFFFFFFFFFFF; // 2^61 - 1 (Mersenne prime)
    static const uint64_t MASK = 0xFFFFFFFF;         // 2^32 - 1
    static uint32_t sSeed;                           // Random seed
    static std::mt19937 sRandomGenerator;
    static std::uniform_int_distribution<uint64_t> sDistribution;

    uint64_t mValue;

    static uint64_t Reduce(uint64_t x);
    static uint64_t ReduceIncompletely(uint64_t x);
    static uint64_t Multiply(uint64_t x, uint64_t y);
};

#endif
