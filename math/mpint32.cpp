#include <cassert>

#include "mpint32.hpp"

/* Initialize static members */
uint32_t Mpint32::sSeed = 0u;
std::mt19937 Mpint32::sRandomGenerator = std::mt19937(Mpint32::sSeed);
std::uniform_int_distribution<uint32_t> Mpint32::sDistribution =
    std::uniform_int_distribution<uint32_t>(0u, Mpint32::BASE - 1u);

/* Define member functions */
Mpint32::Mpint32()
{
    mValue = 0u;
}

Mpint32::Mpint32(uint32_t value)
{
    mValue = ReduceInt32(value);
}

Mpint32::Mpint32(unsigned char* addr)
{
    std::memcpy(&mValue, addr, sizeof(uint32_t));
    mValue = mValue >> 1;
}

const uint32_t Mpint32::GetBase()
{
    return BASE;
}

const uint32_t Mpint32::GetSeed()
{
    return sSeed;
}

const uint32_t Mpint32::GetValue() const
{
    return mValue;
}

void Mpint32::SetSeed(uint32_t seed)
{
    sSeed = seed;
    sRandomGenerator.seed(seed);
}

Mpint32 Mpint32::GenerateRandom()
{
    return Mpint32(sDistribution(sRandomGenerator));
}

Mpint32 Mpint32::GenerateRandomAbove(uint32_t min)
{
    sDistribution = std::uniform_int_distribution<uint32_t>(min, Mpint32::BASE - 1u);
    return Mpint32(sDistribution(sRandomGenerator));
}

Mpint32 Mpint32::Invert() const
{
    return this->Pow(BASE - 2);
}

Mpint32 Mpint32::Pow(uint32_t exp) const
{
    uint64_t result = 1u;
    uint64_t base = mValue;
    while (exp > 0)
    {
        if (exp % 2u == 1u)
        {
            result = Reduce(result * base);
        }
        exp = exp >> 1;
        base = Reduce(base * base);
    }
    return Mpint32((uint32_t)result);
}

void Mpint32::Reverse(Mpint32* begin, Mpint32* end)
{
    const size_t length = (end - begin + 1) / 2;

    assert(length > 0);

    for (size_t i = 0; i < length; ++i)
    {
        Mpint32 temp = *begin;
        *begin = *end;
        *end = temp;
        ++begin;
        --end;
    }
}

Mpint32 Mpint32::operator+(const Mpint32& op)
{
    return Mpint32(this->mValue + op.mValue);
}

Mpint32& Mpint32::operator+=(const Mpint32& op)
{
    this->mValue = ReduceInt32(this->mValue + op.mValue);
    return *this;
}

Mpint32 Mpint32::operator-(const Mpint32& op)
{
    return Mpint32(this->mValue - op.mValue + BASE);
}

Mpint32& Mpint32::operator-=(const Mpint32& op)
{
    this->mValue = ReduceInt32(this->mValue - op.mValue + BASE);
    return *this;
}

Mpint32 Mpint32::operator-()
{
    return Mpint32(ReduceInt32(BASE - this->mValue));
}

Mpint32 Mpint32::operator*(const Mpint32& op)
{
    uint64_t a = this->mValue;
    uint64_t b = op.mValue;
    return Mpint32((uint32_t)(Reduce(a * b)));
}

Mpint32& Mpint32::operator*=(const Mpint32& op)
{
    uint64_t a = this->mValue;
    uint64_t b = op.mValue;
    this->mValue = (uint32_t)(Reduce(a * b));
    return *this;
}

Mpint32 Mpint32::operator/(const Mpint32& op)
{
    return (*this) * op.Invert();
}

Mpint32& Mpint32::operator/=(const Mpint32& op)
{
    uint64_t a = this->mValue;
    uint64_t b = op.Invert().mValue;
    this->mValue = (uint32_t)(Reduce(a * b));
    return *this;
}

bool Mpint32::operator==(const Mpint32& op)
{
    return this->mValue == op.mValue;
}

bool Mpint32::operator>(const Mpint32& op)
{
    return this->mValue > op.mValue;
}

bool Mpint32::operator<(const Mpint32& op)
{
    return this->mValue < op.mValue;
}

bool Mpint32::operator>=(const Mpint32& op)
{
    return this->mValue >= op.mValue;
}

bool Mpint32::operator<=(const Mpint32& op)
{
    return this->mValue <= op.mValue;
}

bool Mpint32::operator!=(const Mpint32& op)
{
    return this->mValue != op.mValue;
}

uint32_t Mpint32::ReduceInt32(uint32_t x)
{
    uint32_t r = (x >> 31) + (x & BASE);
    while (r >= BASE)
    {
        r -= BASE;
    }
    return r;
}

uint64_t Mpint32::Reduce(uint64_t x)
{
    return x % BASE;
}
