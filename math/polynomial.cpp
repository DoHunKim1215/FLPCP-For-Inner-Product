#include <algorithm>
#include <cassert>
#include <cstring>

#include <iostream>

#include "polynomial.h"

Mpint32Poly::Mpint32Poly()
{
    mCoefficients = (Mpint32*)0;
    mCapacity = 0;
}

Mpint32Poly::Mpint32Poly(const Mpint32Poly& obj)
{
    mCoefficients = new Mpint32[obj.mCapacity];
    std::memcpy(mCoefficients, obj.mCoefficients, obj.mCapacity * sizeof(Mpint32));
    mCapacity = obj.mCapacity;
}

Mpint32Poly::Mpint32Poly(Mpint32* coefficients, const size_t capacity, bool isDeepCopy)
{
    assert(capacity > 0);
    if (isDeepCopy)
    {
        mCoefficients = new Mpint32[capacity];
        std::memcpy(mCoefficients, coefficients, capacity * sizeof(Mpint32));
        mCapacity = capacity;
    }
    else
    {
        mCoefficients = coefficients;
        mCapacity = capacity;
    }
}

Mpint32Poly::Mpint32Poly(Mpint32 random, Mpint32* inputs, const size_t length)
{
    assert(length > 0);
    mCapacity = length + 1u;
    mCoefficients = new Mpint32[mCapacity];
    *mCoefficients = random;
    std::memcpy(mCoefficients + 1u, inputs, length * sizeof(Mpint32));
}

Mpint32Poly::~Mpint32Poly()
{
    if (mCoefficients != (Mpint32*)0)
    {
        delete[] mCoefficients;
    }
}

Mpint32 Mpint32Poly::Evaluate(const Mpint32 x) const
{
    Mpint32 value(0u);
    Mpint32 power(1u);
    for (size_t i = 0; i < mCapacity; ++i)
    {
        value += power * mCoefficients[i];
        power *= x;
    }
    return value;
}

Mpint32Poly Mpint32Poly::LagrangeInterpolation(Mpint32* points, const size_t nPoints)
{
    assert(nPoints > 1u);

    Mpint32* coefficients = new Mpint32[nPoints];
    Mpint32* tempCoefficients = new Mpint32[nPoints];
    std::memset(coefficients, 0, nPoints * sizeof(Mpint32));

    for (size_t i = 0; i < nPoints; ++i)
    {
        std::memset(tempCoefficients, 0, nPoints * sizeof(Mpint32));
        tempCoefficients[0] = points[i];
        Mpint32 prod(1u);

        for (size_t j = 0; j < nPoints; ++j)
        {
            if (i == j)
            {
                continue;
            }

            prod *= Mpint32(i) - Mpint32(j);
            Mpint32 precedent(0u);

            for (size_t k = 0; k < nPoints; ++k)
            {
                Mpint32 newres = tempCoefficients[k] * (-Mpint32(j)) + precedent;
                precedent = tempCoefficients[k];
                tempCoefficients[k] = newres;
            }
        }

        prod = prod.Invert();
        for (size_t j = 0; j < nPoints; j++)
        {
            coefficients[j] += tempCoefficients[j] * prod;
        }
    }

    delete[] tempCoefficients;

    return Mpint32Poly(coefficients, nPoints);
}

Mpint32Poly& Mpint32Poly::operator=(const Mpint32Poly& op)
{
    if (mCoefficients != (Mpint32*)0)
    {
        delete[] mCoefficients;
    }

    mCoefficients = new Mpint32[op.mCapacity];
    std::memcpy(mCoefficients, op.mCoefficients, op.mCapacity * sizeof(Mpint32));
    mCapacity = op.mCapacity;

    return *this;
}

Mpint32Poly Mpint32Poly::operator+(const Mpint32Poly& op)
{
    size_t max, min;
    if (this->mCapacity > op.mCapacity)
    {
        max = this->mCapacity;
        min = op.mCapacity;
    }
    else
    {
        max = op.mCapacity;
        min = this->mCapacity;
    }
    
    Mpint32* coefficients = new Mpint32[max];
    for (size_t i = 0; i < min; ++i)
    {
        coefficients[i] = this->mCoefficients[i] + op.mCoefficients[i];
    }
    if (this->mCapacity > op.mCapacity)
    {
        std::memcpy(coefficients + min, this->mCoefficients + min, (max - min) * sizeof(Mpint32));
    }
    else
    {
        std::memcpy(coefficients + min, op.mCoefficients + min, (max - min) * sizeof(Mpint32));
    }
    
    return Mpint32Poly(coefficients, max);
}

void Mpint32Poly::operator+=(const Mpint32Poly& op)
{
    size_t max, min;
    if (this->mCapacity > op.mCapacity)
    {
        max = this->mCapacity;
        min = op.mCapacity;
    }
    else
    {
        max = op.mCapacity;
        min = this->mCapacity;
    }

    Mpint32* coefficients = new Mpint32[max];
    for (size_t i = 0; i < min; ++i)
    {
        coefficients[i] = this->mCoefficients[i] + op.mCoefficients[i];
    }
    if (this->mCapacity > op.mCapacity)
    {
        std::memcpy(coefficients + min, this->mCoefficients + min, (max - min) * sizeof(Mpint32));
    }
    else
    {
        std::memcpy(coefficients + min, op.mCoefficients + min, (max - min) * sizeof(Mpint32));
    }

    delete[] mCoefficients;
    mCoefficients = coefficients;
    mCapacity = max;
}

Mpint32Poly Mpint32Poly::operator-(const Mpint32Poly& op)
{
    size_t max, min;
    if (this->mCapacity > op.mCapacity)
    {
        max = this->mCapacity;
        min = op.mCapacity;
    }
    else
    {
        max = op.mCapacity;
        min = this->mCapacity;
    }

    Mpint32* coefficients = new Mpint32[max];
    for (size_t i = 0; i < min; ++i)
    {
        coefficients[i] = this->mCoefficients[i] - op.mCoefficients[i];
    }
    if (this->mCapacity > op.mCapacity)
    {
        std::memcpy(coefficients + min, this->mCoefficients + min, (max - min) * sizeof(Mpint32));
    }
    else
    {
        std::memcpy(coefficients + min, op.mCoefficients + min, (max - min) * sizeof(Mpint32));
    }

    return Mpint32Poly(coefficients, max);
}

Mpint32Poly Mpint32Poly::operator*(const Mpint32Poly& op)
{
    const size_t capacity = mCapacity + op.mCapacity - 1;

    Mpint32* coefficients = new Mpint32[capacity];
    std::memset(coefficients, 0, capacity * sizeof(Mpint32));
    for (size_t i = 0; i < mCapacity; ++i)
    {
        for (size_t j = 0; j < op.mCapacity; ++j)
        {
            coefficients[i + j] += mCoefficients[i] * op.mCoefficients[j];
        }
    }

    return Mpint32Poly(coefficients, capacity);
}
