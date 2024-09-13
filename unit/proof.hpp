#ifndef PROOF_H
#define PROOF_H

#include <cassert>
#include <vector>

#include "../math/polynomial.hpp"
#include "../math/sha512.hpp"
#include "query.hpp"

template<typename Int> class Proof
{
public:
    Proof();
    Proof(Polynomial<Int>&);
    Proof(Int* values, size_t length, size_t proofLength);
    Proof(std::vector<Int>& op0, std::vector<Int>& op1, size_t nInputs, Int* randoms, size_t nRandoms,
          Polynomial<Int>& poly);
    Proof(Int* op0, Int* op1, size_t nInputs, Int* randoms, size_t nRandoms, Polynomial<Int>& poly);
    ~Proof();

    Int GetQueryAnswer(const Query<Int>& query) const;
    size_t GetBytes() const;
    size_t GetLength() const;
    std::vector<Proof<Int>> GetShares(size_t nShares);
    std::vector<Int> GetRandoms(size_t nRandoms);
    Int GetRandomFromOracle();
    Int GetRandomFromOracle(unsigned char* secretKey, const size_t keyLength);

    Proof<Int>& operator=(const Proof<Int>& obj);

private:
    Int* mValues;
    size_t mLength;
    size_t mProofLength;
};

template <typename Int> Proof<Int>::Proof()
{
    mValues = (Int*)0;
    mLength = 0;
    mProofLength = 0;
}

template <typename Int> Proof<Int>::Proof(Polynomial<Int>& poly)
{
    mLength = poly.mCapacity;
    mProofLength = poly.mCapacity;
    mValues = new Int[poly.mCapacity];
    std::memcpy(mValues, poly.mCoefficients, poly.mCapacity * sizeof(Int));
}

template <typename Int> Proof<Int>::Proof(Int* values, size_t length, size_t proofLength)
{
    mLength = length;
    mProofLength = proofLength;
    mValues = new Int[length];
    std::memcpy(mValues, values, length * sizeof(Int));
}

template <typename Int>
Proof<Int>::Proof(std::vector<Int>& op0, std::vector<Int>& op1, size_t nInputs, Int* randoms, size_t nRandoms,
                  Polynomial<Int>& poly)
{
    assert(op0.size() == nInputs);
    assert(op1.size() == nInputs);
    assert(nRandoms > 0);
    mLength = nRandoms + poly.mCapacity + nInputs + nInputs;
    mProofLength = nRandoms + poly.mCapacity;
    mValues = new Int[mLength];
    std::memcpy(mValues, op0.data(), nInputs * sizeof(Int));
    std::memcpy(mValues + nInputs, op1.data(), nInputs * sizeof(Int));
    std::memcpy(mValues + nInputs + nInputs, randoms, nRandoms * sizeof(Int));
    std::memcpy(mValues + nInputs + nInputs + nRandoms, poly.mCoefficients, poly.mCapacity * sizeof(Int));
}

template <typename Int>
Proof<Int>::Proof(Int* op0, Int* op1, size_t nInputs, Int* randoms, size_t nRandoms, Polynomial<Int>& poly)
{
    assert(nRandoms > 0);
    mLength = nRandoms + poly.mCapacity + nInputs + nInputs;
    mProofLength = nRandoms + poly.mCapacity;
    mValues = new Int[mLength];
    std::memcpy(mValues, op0, nInputs * sizeof(Int));
    std::memcpy(mValues + nInputs, op1, nInputs * sizeof(Int));
    std::memcpy(mValues + nInputs + nInputs, randoms, nRandoms * sizeof(Int));
    std::memcpy(mValues + nInputs + nInputs + nRandoms, poly.mCoefficients, poly.mCapacity * sizeof(Int));
}

template <typename Int> Proof<Int>::~Proof()
{
    if (mValues != (Int*)0)
    {
        delete[] mValues;
    }
}

template <typename Int> Proof<Int>& Proof<Int>::operator=(const Proof<Int>& obj)
{
    mValues = new Int[obj.mLength];
    std::memcpy(mValues, obj.mValues, obj.mLength * sizeof(Int));
    mLength = obj.mLength;
    mProofLength = obj.mProofLength;

    return *this;
}

template <typename Int> Int Proof<Int>::GetQueryAnswer(const Query<Int>& query) const
{
    assert(mLength == query.mLength);

    Int result((uint64_t)0);
    for (size_t i = 0; i < mLength; ++i)
    {
        result += mValues[i] * query.mValues[i];
    }

    return result;
}

template <typename Int> size_t Proof<Int>::GetBytes() const
{
    return mProofLength * sizeof(Int);
}

template <typename Int> size_t Proof<Int>::GetLength() const
{
    return mLength;
}

template <typename Int> std::vector<Proof<Int>> Proof<Int>::GetShares(size_t nShares)
{
    assert(nShares > 0);

    Int* valuesCopy = new Int[mLength];
    std::memcpy(valuesCopy, mValues, mLength * sizeof(Int));

    std::vector<Proof> shares;
    shares.reserve(nShares);
    for (size_t i = 0; i < nShares - 1; ++i)
    {
        Int* randomValues = new Int[mLength];
        for (size_t j = 0; j < mLength; ++j)
        {
            randomValues[j] = Int::GenerateRandom();
            valuesCopy[j] -= randomValues[j];
        }

        shares.emplace_back(randomValues, mLength, mProofLength);

        delete[] randomValues;
    }

    shares.emplace_back(valuesCopy, mLength, mProofLength);

    delete[] valuesCopy;

    return shares;
}

template <typename Int> std::vector<Int> Proof<Int>::GetRandoms(size_t nRandoms)
{
    assert(nRandoms <= mProofLength);

    std::vector<Int> randomsVector(nRandoms);
    Int* randoms = mValues + mLength - mProofLength;
    for (size_t i = 0; i < nRandoms; ++i)
    {
        randomsVector[i] = *randoms;
        ++randoms;
    }
    return randomsVector;
}

template <typename Int> Int Proof<Int>::GetRandomFromOracle()
{
    SHA512_CTX ctx;
    unsigned char digest[SHA512_DIGEST_LENGTH];
    SHA512_Init(&ctx);
    SHA512_Update(&ctx, mValues + (mLength - mProofLength), mProofLength * sizeof(Int));
    SHA512_Final(digest, &ctx);
    return Int(digest);
}

template <typename Int> Int Proof<Int>::GetRandomFromOracle(unsigned char* secretKey, const size_t keyLength)
{
    SHA512_CTX ctx;
    unsigned char digest[SHA512_DIGEST_LENGTH];
    SHA512_Init(&ctx);

    unsigned char* const values = new unsigned char[keyLength * sizeof(unsigned char) + mProofLength * sizeof(Int)];
    std::memcpy(values, secretKey, keyLength * sizeof(unsigned char));
    std::memcpy(values + keyLength, mValues + (mLength - mProofLength), mProofLength * sizeof(Int));

    SHA512_Update(&ctx, values, keyLength * sizeof(unsigned char) + mProofLength * sizeof(Int));
    SHA512_Final(digest, &ctx);

    delete[] values;

    return Int(digest);
}

#endif
