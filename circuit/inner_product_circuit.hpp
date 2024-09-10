#ifndef INNER_PRODUCT_CIRCUIT_H
#define INNER_PRODUCT_CIRCUIT_H

#include <cassert>
#include <cmath>

#include "..\unit\interactive_proof.hpp"
#include "..\math\polynomial.hpp"
#include "..\unit\proof.hpp"
#include "..\unit\query.hpp"

template <typename Int> class InnerProductCircuit
{
public:
    static Int Forward(Int* op0, Int* op1, const size_t length);
    static Proof<Int> MakeProof(Int* op0, Int* op1, const size_t length, const size_t nGGate);
    static std::vector<Query<Int>> MakeQuery(Int random, const size_t nGGate, const size_t inputSize);
    static Proof<Int> MakeCoefficientProof(Int* op0, Int* op1, const size_t length, const size_t nPoly);
    static std::vector<Query<Int>> MakeCoefficientQuery(Int random, size_t inputSize, const size_t nPoly);
    static InteractiveProof<Int> MakeRoundProof(Int* op0, Int* op1, const size_t length, size_t subvectorSize);
    static std::vector<Query<Int>> MakeRoundQuery(Int random, size_t subvectorSize);
    static InteractiveProof<Int> MakeRoundCoefficientProof(Int* op0, Int* op1, const size_t length,
                                                      size_t subvectorSize);
    static std::vector<Query<Int>> MakeRoundCoefficientQuery(Int random, size_t subvectorSize);
};

template <typename Int> Int InnerProductCircuit<Int>::Forward(Int* op0, Int* op1, const size_t length)
{
    assert(length > 0);

    Int result(0);
    for (size_t i = 0; i < length; ++i)
    {
        result += (*op0++) * (*op1++);
    }

    return result;
}

template <typename Int>
Proof<Int> InnerProductCircuit<Int>::MakeProof(Int* op0, Int* op1, const size_t length, const size_t nGGate)
{
    assert(length > 0);
    assert(1 <= nGGate && nGGate <= length);

    const size_t nGGateInputHalf = (size_t)ceil(length / (double)nGGate);

    assert((size_t)(length / (double)nGGate) != nGGateInputHalf);

    const size_t nGGateInput = nGGateInputHalf * 2u;
    const size_t nPointsByOnePoly = nGGate + 1u;

    Int* randoms = new Int[nGGateInput];
    Int* points = new Int[nGGateInput * nPointsByOnePoly];
    std::memset(points, 0, (nGGateInput * nPointsByOnePoly) * sizeof(Int));
    for (size_t i = 0; i < nGGateInput; ++i)
    {
        randoms[i] = Int::GenerateRandom();
        points[i * nPointsByOnePoly] = randoms[i];
    }
    for (size_t i = 0; i < length; ++i)
    {
        const size_t gateNumber = i / nGGateInputHalf + 1u;
        const size_t inputNumber = i % nGGateInputHalf;
        points[gateNumber + inputNumber * nPointsByOnePoly] = op0[i];
        points[gateNumber + (inputNumber + nGGateInputHalf) * nPointsByOnePoly] = op1[i];
    }

    Polynomial<Int>* polys = new Polynomial<Int>[nGGateInput];
    for (size_t i = 0; i < nGGateInput; ++i)
    {
        polys[i] = Polynomial<Int>::LagrangeInterpolation(points + i * nPointsByOnePoly, nPointsByOnePoly);
    }

    Polynomial<Int> gPoly;
    for (size_t i = 0; i < nGGateInputHalf; ++i)
    {
        gPoly += polys[i] * polys[i + nGGateInputHalf];
    }

    Proof<Int> proof(op0, op1, length, randoms, nGGateInput, gPoly);

    delete[] polys;
    delete[] points;
    delete[] randoms;

    return proof;
}

template <typename Int>
std::vector<Query<Int>> InnerProductCircuit<Int>::MakeQuery(Int random, const size_t nGGate, const size_t inputSize)
{
    assert(inputSize > 0);

    const size_t nGGateInputHalf = (size_t)ceil(inputSize / (double)nGGate);

    assert((size_t)(inputSize / (double)nGGate) != nGGateInputHalf);

    const size_t nGGateInput = nGGateInputHalf * 2u;
    const size_t nCoefficients = nGGate * 2u + 1u;

    std::vector<Int> interpCoeff(nGGate + 1u);
    for (size_t i = 0; i < nGGate + 1u; i++)
    {
        Int term(1u);
        for (int j = 0; j < nGGate + 1u; j++)
        {
            if (j != i)
            {
                term *= (random - Int(j)) * (Int(i) - Int(j)).Invert();
            }
        }
        interpCoeff[i] = term;
    }

    const size_t queryLength = inputSize + inputSize + nGGateInput + nCoefficients;
    const size_t nQuery = nGGateInput + 2u;
    Int* const queries = new Int[queryLength * nQuery];
    std::memset(queries, 0, (queryLength * nQuery) * sizeof(Int));

    Int* queriesCurr = queries;
    for (size_t i = 0; i < nGGateInput; ++i)
    {
        for (size_t j = 0; j < inputSize; ++j)
        {
            if (j % nGGateInputHalf == i)
            {
                *queriesCurr = interpCoeff[j / nGGateInputHalf + 1u];
            }
            ++queriesCurr;
        }
        for (size_t j = 0; j < inputSize; ++j)
        {
            if (j % nGGateInputHalf + nGGateInputHalf == i)
            {
                *queriesCurr = interpCoeff[j / nGGateInputHalf + 1u];
            }
            ++queriesCurr;
        }
        queriesCurr += i;
        *queriesCurr = interpCoeff[0];
        queriesCurr += nGGateInput + nCoefficients - i;
    }

    queriesCurr += inputSize + inputSize + nGGateInput;
    Int power(1u);
    for (size_t j = 0; j < nCoefficients; ++j)
    {
        *queriesCurr = power;
        ++queriesCurr;
        power *= random;
    }

    Int* powers = new Int[nGGate];
    for (size_t j = 0; j < nGGate; ++j)
    {
        powers[j] = Int(1u);
    }

    queriesCurr += inputSize + inputSize + nGGateInput;
    *queriesCurr = nGGate;
    ++queriesCurr;
    for (size_t i = 1; i < nCoefficients; ++i)
    {
        Int sum(0u);
        for (size_t j = 0; j < nGGate; ++j)
        {
            powers[j] *= Int(j + 1);
            sum += powers[j];
        }
        *queriesCurr = sum;
        ++queriesCurr;
    }

    delete[] powers;
    powers = (Int*)0;

    std::vector<Query<Int>> queryVectors(nQuery);

    for (size_t i = 0; i < nQuery; ++i)
    {
        queryVectors[i] = Query(queries + i * queryLength, queryLength);
    }

    delete[] queries;

    return queryVectors;
}

template <typename Int>
Proof<Int> InnerProductCircuit<Int>::MakeCoefficientProof(Int* op0, Int* op1, const size_t length, const size_t nPoly)
{
    assert(length > 0);
    assert(1 <= nPoly && nPoly <= length);

    const size_t polyLength = (size_t)ceil(length / (double)nPoly);

    assert((size_t)(length / (double)nPoly) == polyLength);

    const size_t nRandoms = nPoly * 2;
    Int* randoms = new Int[nRandoms];

    Polynomial<Int> gPoly;
    for (size_t i = 0; i < nPoly; ++i)
    {
        randoms[i] = Int::GenerateRandom();
        Polynomial<Int> poly0(randoms[i], op0 + i * polyLength, polyLength);
        Int::Reverse(op1 + i * polyLength, op1 + (i + 1) * polyLength - 1);
        randoms[i + nPoly] = Int::GenerateRandom();
        Polynomial<Int> poly1(randoms[i + nPoly], op1 + i * polyLength, polyLength);
        gPoly += poly0 * poly1;
        Int::Reverse(op1 + i * polyLength, op1 + (i + 1) * polyLength - 1);
    }

    Proof<Int> proof(op0, op1, length, randoms, nRandoms, gPoly);

    delete[] randoms;

    return proof;
}

template <typename Int>
std::vector<Query<Int>> InnerProductCircuit<Int>::MakeCoefficientQuery(Int random, size_t inputSize, const size_t nPoly)
{
    assert(inputSize > 0);

    const size_t polyLength = (size_t)ceil(inputSize / (double)nPoly);

    assert((size_t)(inputSize / (double)nPoly) == polyLength);

    const size_t queryLength = inputSize * 2 + nPoly * 2 + polyLength * 2 + 1;
    const size_t nQueries = nPoly * 2 + 2;
    Int* const queries = new Int[queryLength * nQueries];
    std::memset(queries, 0, (queryLength * nQueries) * sizeof(Int));

    std::vector<Query<Int>> queryVectors(nQueries);

    Int* const randoms = new Int[polyLength * 2 + 1];
    Int randomPower(1);
    for (size_t i = 0; i < polyLength * 2 + 1; ++i)
    {
        randoms[i] = randomPower;
        randomPower *= random;
    }

    Int* const reverseRandoms = new Int[polyLength];
    randomPower = random;
    for (size_t i = 0; i < polyLength; ++i)
    {
        reverseRandoms[polyLength - i - 1] = randomPower;
        randomPower *= random;
    }

    Int* queriesCurr = queries;
    for (size_t i = 0; i < nPoly; ++i)
    {
        std::memcpy(queries + i * (polyLength + queryLength), randoms + 1, polyLength * sizeof(Int));
        queries[inputSize * 2 + i * (queryLength + 1)] = Int(1);
    }
    for (size_t i = nPoly; i < nPoly * 2; ++i)
    {
        std::memcpy(queries + i * (polyLength + queryLength), reverseRandoms, polyLength * sizeof(Int));
        queries[inputSize * 2 + i * (queryLength + 1)] = Int(1);
    }

    std::memcpy(queries + queryLength * nPoly * 2 + inputSize * 2 + nPoly * 2, randoms,
                (polyLength * 2 + 1) * sizeof(Int));

    queries[queryLength * (nPoly * 2 + 1) + inputSize * 2u + nPoly * 2u + polyLength + 1] = Int(1);

    for (size_t i = 0; i < nQueries; ++i)
    {
        queryVectors[i] = Query(queries + i * queryLength, queryLength);
    }

    delete[] queries;
    delete[] randoms;
    delete[] reverseRandoms;

    return queryVectors;
}

template <typename Int>
InteractiveProof<Int> InnerProductCircuit<Int>::MakeRoundProof(Int* op0, Int* op1, const size_t length,
                                                               size_t subvectorSize)
{
    assert(length > 0);

    const size_t nPoly = ceil(length / (double)subvectorSize);

    assert(nPoly > 1);

    Int* const resizedInput = new Int[nPoly * subvectorSize * 2u];
    std::memset(resizedInput, 0, (nPoly * subvectorSize * 2u) * sizeof(Int));
    std::memcpy(resizedInput, op0, length * sizeof(Int));
    std::memcpy(resizedInput + nPoly * subvectorSize, op1, length * sizeof(Int));

    Polynomial<Int> gPoly;
    std::vector<Polynomial<Int>> poly0s;
    poly0s.reserve(nPoly);
    std::vector<Polynomial<Int>> poly1s;
    poly1s.reserve(nPoly);
    for (size_t i = 0; i < nPoly; ++i)
    {
        Polynomial<Int> poly0 = Polynomial<Int>::LagrangeInterpolation(resizedInput + subvectorSize * i, subvectorSize);
        Polynomial<Int> poly1 =
            Polynomial<Int>::LagrangeInterpolation(resizedInput + subvectorSize * (nPoly + i), subvectorSize);
        gPoly += poly0 * poly1;
        poly0s.emplace_back(poly0);
        poly1s.emplace_back(poly1);
    }

    delete[] resizedInput;

    return InteractiveProof<Int>(poly0s, poly1s, gPoly);
}

template <typename Int>
std::vector<Query<Int>> InnerProductCircuit<Int>::MakeRoundQuery(Int random, size_t subvectorSize)
{
    const size_t queryLength = subvectorSize * 2u - 1u;

    std::vector<Query<Int>> queryVectors;
    queryVectors.reserve(2);

    Int* const queries = new Int[queryLength * 2u];
    Int* queriesCurr = queries;

    Int* const powers = new Int[subvectorSize];
    for (size_t j = 0; j < subvectorSize; ++j)
    {
        powers[j] = Int(1u);
    }

    *queriesCurr = Int(subvectorSize);
    ++queriesCurr;
    for (size_t i = 1; i < queryLength; ++i)
    {
        Int sum(0u);
        for (size_t j = 0; j < subvectorSize - 1; ++j)
        {
            powers[j] *= Int(j + 1);
            sum += powers[j];
        }
        *queriesCurr = sum;
        ++queriesCurr;
    }

    Int power(1u);
    for (size_t i = 0; i < queryLength; ++i)
    {
        *queriesCurr = power;
        ++queriesCurr;
        power *= random;
    }

    queryVectors.emplace_back(queries, queryLength);
    queryVectors.emplace_back(queries + queryLength, queryLength);

    delete[] powers;
    delete[] queries;

    return queryVectors;
}

template <typename Int>
InteractiveProof<Int> InnerProductCircuit<Int>::MakeRoundCoefficientProof(Int* op0, Int* op1, const size_t length,
                                                                          size_t subvectorSize)
{
    assert(length > 0);

    const size_t nPoly = ceil(length / (double)subvectorSize);

    assert(nPoly > 1);

    Int* const resizedInput = new Int[nPoly * subvectorSize * 2u];
    std::memset(resizedInput, 0, (nPoly * subvectorSize * 2u) * sizeof(Int));
    std::memcpy(resizedInput, op0, length * sizeof(Int));
    std::memcpy(resizedInput + nPoly * subvectorSize, op1, length * sizeof(Int));

    Polynomial<Int> gPoly;
    std::vector<Polynomial<Int>> poly0s;
    poly0s.reserve(nPoly);
    std::vector<Polynomial<Int>> poly1s;
    poly1s.reserve(nPoly);
    for (size_t i = 0; i < nPoly; ++i)
    {
        Polynomial<Int> poly0(resizedInput + subvectorSize * i, subvectorSize, true);
        Int::Reverse(resizedInput + subvectorSize * (nPoly + i),
                     resizedInput + subvectorSize * (nPoly + i) + subvectorSize - 1);
        Polynomial<Int> poly1(resizedInput + subvectorSize * (nPoly + i), subvectorSize, true);
        gPoly += poly0 * poly1;
        poly0s.emplace_back(poly0);
        poly1s.emplace_back(poly1);
    }

    delete[] resizedInput;

    return InteractiveProof<Int>(poly0s, poly1s, gPoly);
}

template <typename Int>
std::vector<Query<Int>> InnerProductCircuit<Int>::MakeRoundCoefficientQuery(Int random, size_t subvectorSize)
{
    const size_t queryLength = subvectorSize * 2u - 1u;

    std::vector<Query<Int>> queryVectors;
    queryVectors.reserve(2);

    Int* const queries = new Int[queryLength * 2u];
    std::memset(queries, 0, (queryLength * 2u) * sizeof(Int));

    queries[subvectorSize - 1u] = Int(1u);

    Int* queriesCurr = queries + queryLength;
    Int power(1u);
    for (size_t i = 0; i < queryLength; ++i)
    {
        *queriesCurr = power;
        ++queriesCurr;
        power *= random;
    }

    queryVectors.emplace_back(queries, queryLength);
    queryVectors.emplace_back(queries + queryLength, queryLength);

    delete[] queries;

    return queryVectors;
}

#endif
