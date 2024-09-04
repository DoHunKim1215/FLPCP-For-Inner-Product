#ifndef MPINT32_H
#define MPINT32_H

#include <random>
#include <stdint.h>


/* 32-bit Integer over Mersenne Prime Field : Z_{2^31 - 1} */
class Mpint32
{
public:
    Mpint32();
    Mpint32(uint32_t value);

    static const uint32_t GetBase();
    static const uint32_t GetSeed();
    const uint32_t GetValue() const;

    static void SetSeed(uint32_t seed);

    static Mpint32 GenerateRandom();
    static Mpint32 GenerateRandomAbove(uint32_t min);
    static void Reverse(Mpint32* begin, Mpint32* end);

    Mpint32 Invert() const;
    Mpint32 Pow(uint32_t exp) const;

    Mpint32 operator+(const Mpint32& op);
    Mpint32& operator+=(const Mpint32& op);
    Mpint32 operator-(const Mpint32& op);
    Mpint32& operator-=(const Mpint32& op);
    Mpint32 operator-();
    Mpint32 operator*(const Mpint32& op);
    Mpint32& operator*=(const Mpint32& op);
    Mpint32 operator/(const Mpint32& op);
    Mpint32& operator/=(const Mpint32& op);
    bool operator==(const Mpint32& op);
    bool operator>(const Mpint32& op);
    bool operator<(const Mpint32& op);
    bool operator>=(const Mpint32& op);
    bool operator<=(const Mpint32& op);
    bool operator!=(const Mpint32& op);

private:
    static const uint32_t BASE = 0x7FFFFFFF; // 2^31 - 1 (Mersenne prime)
    static uint32_t sSeed;                   // Random seed
    static std::mt19937 sRandomGenerator;
    static std::uniform_int_distribution<uint32_t> sDistribution;

    uint32_t mValue;

    static uint32_t ReduceInt32(uint32_t x);
    static uint64_t Reduce(uint64_t x);
};

#endif
