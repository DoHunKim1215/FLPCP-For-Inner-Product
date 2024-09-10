#ifndef POLYNOMIAL_H
#define POLYNOMIAL_H

#include <algorithm>
#include <cassert>
#include <cstring>
#include <iostream>

template <typename Int> class Proof;

template <typename Int> class Polynomial
{
public:
    friend class Proof<Int>;

    Polynomial();
    Polynomial(const Polynomial<Int>& obj);
    Polynomial(Int* coefficients, const size_t length, bool isDeepCopy = false);
    Polynomial(Int random, Int* inputs, const size_t length);
    ~Polynomial();

    Int Evaluate(const Int x) const;

    static Polynomial LagrangeInterpolation(Int* points, const size_t nPoints);

    Polynomial<Int>& operator=(const Polynomial<Int>& op);
    Polynomial<Int> operator+(const Polynomial<Int>& op);
    void operator+=(const Polynomial<Int>& op);
    Polynomial<Int> operator-(const Polynomial<Int>& op);
    Polynomial<Int> operator*(const Polynomial<Int>& op);

private:
    Int* mCoefficients;
    size_t mCapacity;
};

template <typename Int> Polynomial<Int>::Polynomial()
{
    mCoefficients = (Int*)0;
    mCapacity = 0;
}

template <typename Int> Polynomial<Int>::Polynomial(const Polynomial<Int>& obj)
{
    mCoefficients = new Int[obj.mCapacity];
    std::memcpy(mCoefficients, obj.mCoefficients, obj.mCapacity * sizeof(Int));
    mCapacity = obj.mCapacity;
}

template <typename Int> Polynomial<Int>::Polynomial(Int* coefficients, const size_t capacity, bool isDeepCopy)
{
    assert(capacity > 0);
    if (isDeepCopy)
    {
        mCoefficients = new Int[capacity];
        std::memcpy(mCoefficients, coefficients, capacity * sizeof(Int));
        mCapacity = capacity;
    }
    else
    {
        mCoefficients = coefficients;
        mCapacity = capacity;
    }
}

template <typename Int> Polynomial<Int>::Polynomial(Int random, Int* inputs, const size_t length)
{
    assert(length > 0);
    mCapacity = length + 1u;
    mCoefficients = new Int[mCapacity];
    *mCoefficients = random;
    std::memcpy(mCoefficients + 1u, inputs, length * sizeof(Int));
}

template <typename Int> Polynomial<Int>::~Polynomial()
{
    if (mCoefficients != (Int*)0)
    {
        delete[] mCoefficients;
    }
}

template <typename Int> Int Polynomial<Int>::Evaluate(const Int x) const
{
    Int value(0u);
    Int power(1u);
    for (size_t i = 0; i < mCapacity; ++i)
    {
        value += power * mCoefficients[i];
        power *= x;
    }
    return value;
}

template <typename Int> Polynomial<Int> Polynomial<Int>::LagrangeInterpolation(Int* points, const size_t nPoints)
{
    assert(nPoints > 1u);

    Int* coefficients = new Int[nPoints];
    Int* tempCoefficients = new Int[nPoints];
    std::memset(coefficients, 0, nPoints * sizeof(Int));

    for (size_t i = 0; i < nPoints; ++i)
    {
        std::memset(tempCoefficients, 0, nPoints * sizeof(Int));
        tempCoefficients[0] = points[i];
        Int prod(1u);

        for (size_t j = 0; j < nPoints; ++j)
        {
            if (i == j)
            {
                continue;
            }

            prod *= Int(i) - Int(j);
            Int precedent(0u);

            for (size_t k = 0; k < nPoints; ++k)
            {
                Int newres = tempCoefficients[k] * (-Int(j)) + precedent;
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

    return Polynomial(coefficients, nPoints);
}

template <typename Int> Polynomial<Int>& Polynomial<Int>::operator=(const Polynomial<Int>& op)
{
    if (mCoefficients != (Int*)0)
    {
        delete[] mCoefficients;
    }

    mCoefficients = new Int[op.mCapacity];
    std::memcpy(mCoefficients, op.mCoefficients, op.mCapacity * sizeof(Int));
    mCapacity = op.mCapacity;

    return *this;
}

template <typename Int> Polynomial<Int> Polynomial<Int>::operator+(const Polynomial<Int>& op)
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

    Int* coefficients = new Int[max];
    for (size_t i = 0; i < min; ++i)
    {
        coefficients[i] = this->mCoefficients[i] + op.mCoefficients[i];
    }
    if (this->mCapacity > op.mCapacity)
    {
        std::memcpy(coefficients + min, this->mCoefficients + min, (max - min) * sizeof(Int));
    }
    else
    {
        std::memcpy(coefficients + min, op.mCoefficients + min, (max - min) * sizeof(Int));
    }

    return Polynomial(coefficients, max);
}

template <typename Int> void Polynomial<Int>::operator+=(const Polynomial& op)
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

    Int* coefficients = new Int[max];
    for (size_t i = 0; i < min; ++i)
    {
        coefficients[i] = this->mCoefficients[i] + op.mCoefficients[i];
    }
    if (this->mCapacity > op.mCapacity)
    {
        std::memcpy(coefficients + min, this->mCoefficients + min, (max - min) * sizeof(Int));
    }
    else
    {
        std::memcpy(coefficients + min, op.mCoefficients + min, (max - min) * sizeof(Int));
    }

    delete[] mCoefficients;
    mCoefficients = coefficients;
    mCapacity = max;
}

template <typename Int> Polynomial<Int> Polynomial<Int>::operator-(const Polynomial& op)
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

    Int* coefficients = new Int[max];
    for (size_t i = 0; i < min; ++i)
    {
        coefficients[i] = this->mCoefficients[i] - op.mCoefficients[i];
    }
    if (this->mCapacity > op.mCapacity)
    {
        std::memcpy(coefficients + min, this->mCoefficients + min, (max - min) * sizeof(Int));
    }
    else
    {
        std::memcpy(coefficients + min, op.mCoefficients + min, (max - min) * sizeof(Int));
    }

    return Polynomial(coefficients, max);
}

template <typename Int> Polynomial<Int> Polynomial<Int>::operator*(const Polynomial<Int>& op)
{
    const size_t capacity = mCapacity + op.mCapacity - 1;

    Int* coefficients = new Int[capacity];
    std::memset(coefficients, 0, capacity * sizeof(Int));
    for (size_t i = 0; i < mCapacity; ++i)
    {
        for (size_t j = 0; j < op.mCapacity; ++j)
        {
            coefficients[i + j] += mCoefficients[i] * op.mCoefficients[j];
        }
    }

    return Polynomial(coefficients, capacity);
}

#endif
