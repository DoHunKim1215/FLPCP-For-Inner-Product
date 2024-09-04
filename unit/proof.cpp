#include <cassert>

#include "proof.h"
#include "..\math\polynomial.h"
#include "query.h"

Proof::Proof()
{
    mValues = (Mpint32*)0;
    mLength = 0;
    mProofLength = 0;
}

Proof::Proof(Mpint32Poly& poly)
{
    mLength = poly.mCapacity;
    mProofLength = poly.mCapacity;
    mValues = new Mpint32[poly.mCapacity];
    std::memcpy(mValues, poly.mCoefficients, poly.mCapacity * sizeof(Mpint32));
}

Proof::Proof(Mpint32* values, size_t length, size_t proofLength)
{
    mLength = length;
    mProofLength = proofLength;
    mValues = new Mpint32[length];
    std::memcpy(mValues, values, length * sizeof(Mpint32));
}

Proof::Proof(std::vector<Mpint32>& op0, std::vector<Mpint32>& op1, size_t nInputs, Mpint32* randoms, size_t nRandoms,
             Mpint32Poly& poly)
{
    assert(op0.size() == nInputs);
    assert(op1.size() == nInputs);
    assert(nRandoms > 0);
    mLength = nRandoms + poly.mCapacity + nInputs + nInputs;
    mProofLength = nRandoms + poly.mCapacity;
    mValues = new Mpint32[mLength];
    std::memcpy(mValues, op0.data(), nInputs * sizeof(Mpint32));
    std::memcpy(mValues + nInputs, op1.data(), nInputs * sizeof(Mpint32));
    std::memcpy(mValues + nInputs + nInputs, randoms, nRandoms * sizeof(Mpint32));
    std::memcpy(mValues + nInputs + nInputs + nRandoms, poly.mCoefficients, poly.mCapacity * sizeof(Mpint32));
}

Proof::Proof(Mpint32* op0, Mpint32* op1, size_t nInputs, Mpint32* randoms, size_t nRandoms,
             Mpint32Poly& poly)
{
    assert(nRandoms > 0);
    mLength = nRandoms + poly.mCapacity + nInputs + nInputs;
    mProofLength = nRandoms + poly.mCapacity;
    mValues = new Mpint32[mLength];
    std::memcpy(mValues, op0, nInputs * sizeof(Mpint32));
    std::memcpy(mValues + nInputs, op1, nInputs * sizeof(Mpint32));
    std::memcpy(mValues + nInputs + nInputs, randoms, nRandoms * sizeof(Mpint32));
    std::memcpy(mValues + nInputs + nInputs + nRandoms, poly.mCoefficients, poly.mCapacity * sizeof(Mpint32));
}

Proof::~Proof()
{
    if (mValues != (Mpint32*)0)
    {
        delete[] mValues;
    }
}

Proof& Proof::operator=(const Proof& obj)
{
    mValues = new Mpint32[obj.mLength];
    std::memcpy(mValues, obj.mValues, obj.mLength * sizeof(Mpint32));
    mLength = obj.mLength;
    mProofLength = obj.mProofLength;

    return *this;
}

Mpint32 Proof::GetQueryAnswer(const Query& query) const
{
    assert(mLength == query.mLength);
    
    Mpint32 result(0u);
    for (size_t i = 0; i < mLength; ++i)
    {
        result += mValues[i] * query.mValues[i];
    }

    return result;
}

size_t Proof::GetBytes() const
{
    return mProofLength * sizeof(Mpint32);
}

size_t Proof::GetLength() const
{
    return mLength;
}

std::vector<Proof> Proof::GetShares(size_t nShares)
{
    assert(nShares > 0);

    Mpint32* valuesCopy = new Mpint32[mLength];
    std::memcpy(valuesCopy, mValues, mLength * sizeof(Mpint32));

    std::vector<Proof> shares;
    shares.reserve(nShares);
    for (size_t i = 0; i < nShares - 1; ++i)
    {
        Mpint32* randomValues = new Mpint32[mLength];
        for (size_t j = 0; j < mLength; ++j)
        {
            randomValues[j] = Mpint32::GenerateRandom();
            valuesCopy[j] -= randomValues[j];
        }

        shares.emplace_back(randomValues, mLength, mProofLength);

        delete[] randomValues;
    }

    shares.emplace_back(valuesCopy, mLength, mProofLength);

    delete[] valuesCopy;

    return shares;
}

std::vector<Mpint32> Proof::GetRandoms(size_t nRandoms)
{
    assert(nRandoms <= mProofLength);

    std::vector<Mpint32> randomsVector(nRandoms);
    Mpint32* randoms = mValues + mLength - mProofLength;
    for (size_t i = 0; i < nRandoms; ++i)
    {
        randomsVector[i] = *randoms;
        ++randoms;
    }
    return randomsVector;
}
