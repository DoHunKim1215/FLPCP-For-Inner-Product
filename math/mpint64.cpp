#include <cassert>

#include "mpint64.hpp"

/* Initialize static members */
uint32_t Mpint64::sSeed = 0u;
std::mt19937 Mpint64::sRandomGenerator = std::mt19937(Mpint64::sSeed);
std::uniform_int_distribution<uint64_t> Mpint64::sDistribution =
    std::uniform_int_distribution<uint64_t>(0u, Mpint64::BASE - 1u);

/* Define member functions */
Mpint64::Mpint64()
{
    mValue = 0u;
}

Mpint64::Mpint64(uint64_t value)
{
    mValue = Reduce(value);
}

Mpint64::Mpint64(unsigned char* addr)
{
    std::memcpy(&mValue, addr, sizeof(uint64_t));
    mValue = mValue >> 3;
}

const uint64_t Mpint64::GetBase()
{
    return BASE;
}

const uint32_t Mpint64::GetSeed()
{
    return sSeed;
}

const uint64_t Mpint64::GetValue() const
{
    return mValue;
}

void Mpint64::SetSeed(uint32_t seed)
{
    sSeed = seed;
    sRandomGenerator.seed(seed);
}

Mpint64 Mpint64::GenerateRandom()
{
    return Mpint64(sDistribution(sRandomGenerator));
}

Mpint64 Mpint64::GenerateRandomAbove(uint64_t min)
{
    sDistribution = std::uniform_int_distribution<uint64_t>(min, Mpint64::BASE - 1u);
    return Mpint64(sDistribution(sRandomGenerator));
}

void Mpint64::Reverse(Mpint64* begin, Mpint64* end)
{
    const size_t length = (end - begin + 1) / 2;

    assert(length > 0);

    for (size_t i = 0; i < length; ++i)
    {
        Mpint64 temp = *begin;
        *begin = *end;
        *end = temp;
        ++begin;
        --end;
    }
}

Mpint64 Mpint64::Invert() const
{
    return this->Pow(BASE - 2);
}

Mpint64 Mpint64::Pow(uint64_t exp) const
{
    uint64_t result = 1u;
    uint64_t base = mValue;
    while (exp > 0)
    {
        if (exp % 2u == 1u)
        {
            result = Multiply(result, base);
        }
        exp = exp >> 1;
        base = Multiply(base, base);
    }
    return Mpint64(result);
}

Mpint64 Mpint64::operator+(const Mpint64& op)
{
    return Mpint64(this->mValue + op.mValue);
}

Mpint64& Mpint64::operator+=(const Mpint64& op)
{
    this->mValue = Reduce(this->mValue + op.mValue);
    return *this;
}

Mpint64 Mpint64::operator-(const Mpint64& op)
{
    return Mpint64(this->mValue - op.mValue + BASE);
}

Mpint64& Mpint64::operator-=(const Mpint64& op)
{
    this->mValue = Reduce(this->mValue - op.mValue + BASE);
    return *this;
}

Mpint64 Mpint64::operator-()
{
    return Mpint64(Reduce(BASE - this->mValue));
}

Mpint64 Mpint64::operator*(const Mpint64& op)
{
    return Mpint64(Multiply(this->mValue, op.mValue));
}

Mpint64& Mpint64::operator*=(const Mpint64& op)
{
    this->mValue = Multiply(this->mValue, op.mValue);
    return *this;
}

Mpint64 Mpint64::operator/(const Mpint64& op)
{
    return (*this) * op.Invert();
}

Mpint64& Mpint64::operator/=(const Mpint64& op)
{
    this->mValue = Multiply(this->mValue, op.Invert().mValue);
    return *this;
}

bool Mpint64::operator==(const Mpint64& op)
{
    return this->mValue == op.mValue;
}

bool Mpint64::operator>(const Mpint64& op)
{
    return this->mValue > op.mValue;
}

bool Mpint64::operator<(const Mpint64& op)
{
    return this->mValue < op.mValue;
}

bool Mpint64::operator>=(const Mpint64& op)
{
    return this->mValue >= op.mValue;
}

bool Mpint64::operator<=(const Mpint64& op)
{
    return this->mValue <= op.mValue;
}

bool Mpint64::operator!=(const Mpint64& op)
{
    return this->mValue != op.mValue;
}

uint64_t Mpint64::Reduce(uint64_t x)
{
    uint64_t r = (x >> 61) + (x & BASE);
    while (r >= BASE)
    {
        r -= BASE;
    }
    return r;
}

uint64_t Mpint64::ReduceIncompletely(uint64_t x)
{
    return (x >> 61) + (x & BASE);
}

uint64_t Mpint64::Multiply(uint64_t x, uint64_t y)
{
    uint64_t hi_x = x >> 32;
    uint64_t hi_y = y >> 32;
    uint64_t low_x = x & MASK;
    uint64_t low_y = y & MASK;

    uint64_t piece1 = ReduceIncompletely((hi_x * hi_y) << 3);
    uint64_t z = (hi_x * low_y + hi_y * low_x);
    uint64_t hi_z = z >> 32;
    uint64_t low_z = z & MASK;

    uint64_t piece2 = ReduceIncompletely((hi_z << 3) + ReduceIncompletely((low_z << 32)));
    uint64_t piece3 = ReduceIncompletely(low_x * low_y);
    uint64_t result = ReduceIncompletely(piece1 + piece2 + piece3);

    return result;
}
