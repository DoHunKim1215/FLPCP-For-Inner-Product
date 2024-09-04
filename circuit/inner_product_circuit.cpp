#include <cassert>
#include <cmath>

#include "inner_product_circuit.h"
#include "..\math\polynomial.h"
#include "..\unit\interactive_proof.h"


Mpint32 InnerProductCircuit::Forward(Mpint32* op0, Mpint32* op1, const size_t length)
{
    assert(length > 0);

    Mpint32 result(0);
    for (size_t i = 0; i < length; ++i)
    {
        result += (*op0++) * (*op1++);
    }

    return result;
}

Proof InnerProductCircuit::MakeProof(Mpint32* op0, Mpint32* op1, const size_t length, const size_t nGGate)
{
    assert(length > 0);
    assert(1 <= nGGate && nGGate <= length);

    const size_t nGGateInputHalf = (size_t)ceil(length / (double)nGGate);

    assert((size_t)(length / (double)nGGate) != nGGateInputHalf);

    const size_t nGGateInput = nGGateInputHalf * 2u;
    const size_t nPointsByOnePoly = nGGate + 1u;

    Mpint32* randoms = new Mpint32[nGGateInput];
    Mpint32* points = new Mpint32[nGGateInput * nPointsByOnePoly];
    std::memset(points, 0, (nGGateInput * nPointsByOnePoly) * sizeof(Mpint32));
    for (size_t i = 0; i < nGGateInput; ++i)
    {
        randoms[i] = Mpint32::GenerateRandom();
        points[i * nPointsByOnePoly] = randoms[i];
    }
    for (size_t i = 0; i < length; ++i)
    {
        const size_t gateNumber = i / nGGateInputHalf + 1u;
        const size_t inputNumber = i % nGGateInputHalf;
        points[gateNumber + inputNumber * nPointsByOnePoly] = op0[i];
        points[gateNumber + (inputNumber + nGGateInputHalf) * nPointsByOnePoly] = op1[i];
    }

    Mpint32Poly* polys = new Mpint32Poly[nGGateInput];
    for (size_t i = 0; i < nGGateInput; ++i)
    {
        polys[i] = Mpint32Poly::LagrangeInterpolation(points + i * nPointsByOnePoly, nPointsByOnePoly);
    }

    Mpint32Poly gPoly;
    for (size_t i = 0; i < nGGateInputHalf; ++i)
    {
        gPoly += polys[i] * polys[i + nGGateInputHalf];
    }

    Proof proof(op0, op1, length, randoms, nGGateInput, gPoly);

    delete[] polys;
    delete[] points;
    delete[] randoms;

    return proof;
}

std::vector<Query> InnerProductCircuit::MakeQuery(Mpint32 random, const size_t nGGate, const size_t inputSize)
{
    assert(inputSize > 0);

    const size_t nGGateInputHalf = (size_t)ceil(inputSize / (double)nGGate);

    assert((size_t)(inputSize / (double)nGGate) != nGGateInputHalf);

    const size_t nGGateInput = nGGateInputHalf * 2u;
    const size_t nCoefficients = nGGate * 2u + 1u;

    std::vector<Mpint32> interpCoeff(nGGate + 1u);
    for (size_t i = 0; i < nGGate + 1u; i++)
    {
        Mpint32 term(1u);
        for (int j = 0; j < nGGate + 1u; j++)
        {
            if (j != i)
            {
                term *= (random - Mpint32(j)) * (Mpint32(i) - Mpint32(j)).Invert();
            }
        }
        interpCoeff[i] = term;
    }

    const size_t queryLength = inputSize + inputSize + nGGateInput + nCoefficients;
    const size_t nQuery = nGGateInput + 2u;
    Mpint32* const queries = new Mpint32[queryLength * nQuery];
    std::memset(queries, 0, (queryLength * nQuery) * sizeof(Mpint32));

    Mpint32* queriesCurr = queries;
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
    Mpint32 power(1u);
    for (size_t j = 0; j < nCoefficients; ++j)
    {
        *queriesCurr = power;
        ++queriesCurr;
        power *= random;
    }

    Mpint32* powers = new Mpint32[nGGate];
    for (size_t j = 0; j < nGGate; ++j)
    {
        powers[j] = Mpint32(1u);
    }

    queriesCurr += inputSize + inputSize + nGGateInput;
    *queriesCurr = nGGate;
    ++queriesCurr;
    for (size_t i = 1; i < nCoefficients; ++i)
    {
        Mpint32 sum(0u);
        for (size_t j = 0; j < nGGate; ++j)
        {
            powers[j] *= Mpint32(j + 1);
            sum += powers[j];
        }
        *queriesCurr = sum;
        ++queriesCurr;
    }

    delete[] powers;
    powers = (Mpint32*)0;

    std::vector<Query> queryVectors(nQuery);

    for (size_t i = 0; i < nQuery; ++i)
    {
        queryVectors[i] = Query(queries + i * queryLength, queryLength);
    }

    delete[] queries;

    return queryVectors;
}

Proof InnerProductCircuit::MakeCoefficientProof(Mpint32* op0, Mpint32* op1, const size_t length, const size_t nPoly)
{
    assert(length > 0);
    assert(1 <= nPoly && nPoly <= length);

    const size_t polyLength = (size_t)ceil(length / (double)nPoly);

    assert((size_t)(length / (double)nPoly) == polyLength);

    const size_t nRandoms = nPoly * 2;
    Mpint32* randoms = new Mpint32[nRandoms];

    Mpint32Poly gPoly;
    for (size_t i = 0; i < nPoly; ++i)
    {
        randoms[i] = Mpint32::GenerateRandom();
        Mpint32Poly poly0(randoms[i], op0 + i * polyLength, polyLength);
        Mpint32::Reverse(op1 + i * polyLength, op1 + (i + 1) * polyLength - 1);
        randoms[i + nPoly] = Mpint32::GenerateRandom();
        Mpint32Poly poly1(randoms[i + nPoly], op1 + i * polyLength, polyLength);
        gPoly += poly0 * poly1;
        Mpint32::Reverse(op1 + i * polyLength, op1 + (i + 1) * polyLength - 1);
    }

    Proof proof(op0, op1, length, randoms, nRandoms, gPoly);

    delete[] randoms;

    return proof;
}

std::vector<Query> InnerProductCircuit::MakeCoefficientQuery(Mpint32 random, size_t inputSize, const size_t nPoly)
{
    assert(inputSize > 0);

    const size_t polyLength = (size_t)ceil(inputSize / (double)nPoly);

    assert((size_t)(inputSize / (double)nPoly) == polyLength);

    const size_t queryLength = inputSize * 2 + nPoly * 2 + polyLength * 2 + 1;
    const size_t nQueries = nPoly * 2 + 2;
    Mpint32* const queries = new Mpint32[queryLength * nQueries];
    std::memset(queries, 0, (queryLength * nQueries) * sizeof(Mpint32));

    std::vector<Query> queryVectors(nQueries);
    
    Mpint32* const randoms = new Mpint32[polyLength * 2 + 1];
    Mpint32 randomPower(1);
    for (size_t i = 0; i < polyLength * 2 + 1; ++i)
    {
        randoms[i] = randomPower;
        randomPower *= random;
    }

    Mpint32* const reverseRandoms = new Mpint32[polyLength];
    randomPower = random;
    for (size_t i = 0; i < polyLength; ++i)
    {
        reverseRandoms[polyLength - i - 1] = randomPower;
        randomPower *= random;
    }

    Mpint32* queriesCurr = queries;
    for (size_t i = 0; i < nPoly; ++i)
    {
        std::memcpy(queries + i * (polyLength + queryLength), randoms + 1, polyLength * sizeof(Mpint32));
        queries[inputSize * 2 + i * (queryLength + 1)] = Mpint32(1);
    }
    for (size_t i = nPoly; i < nPoly * 2; ++i)
    {
        std::memcpy(queries + i * (polyLength + queryLength), reverseRandoms, polyLength * sizeof(Mpint32));
        queries[inputSize * 2 + i * (queryLength + 1)] = Mpint32(1);
    }

    std::memcpy(queries + queryLength * nPoly * 2 + inputSize * 2 + nPoly * 2, randoms,
                (polyLength * 2 + 1) * sizeof(Mpint32));

    queries[queryLength * (nPoly * 2 + 1) + inputSize * 2u + nPoly * 2u + polyLength + 1] = Mpint32(1);

    for (size_t i = 0; i < nQueries; ++i)
    {
        queryVectors[i] = Query(queries + i * queryLength, queryLength);
    }

    delete[] queries;
    delete[] randoms;
    delete[] reverseRandoms;

    return queryVectors;
}

InteractiveProof InnerProductCircuit::MakeRoundProof(Mpint32* op0, Mpint32* op1, const size_t length,
                                                     size_t subvectorSize)
{
    assert(length > 0);

    const size_t nPoly = ceil(length / (double)subvectorSize);

    assert(nPoly > 1);

    Mpint32* const resizedInput = new Mpint32[nPoly * subvectorSize * 2u];
    std::memset(resizedInput, 0, (nPoly * subvectorSize * 2u) * sizeof(Mpint32));
    std::memcpy(resizedInput, op0, length * sizeof(Mpint32));
    std::memcpy(resizedInput + nPoly * subvectorSize, op1, length * sizeof(Mpint32));

    Mpint32Poly gPoly;
    std::vector<Mpint32Poly> poly0s;
    poly0s.reserve(nPoly);
    std::vector<Mpint32Poly> poly1s;
    poly1s.reserve(nPoly);
    for (size_t i = 0; i < nPoly; ++i)
    {
        Mpint32Poly poly0 = Mpint32Poly::LagrangeInterpolation(resizedInput + subvectorSize * i, subvectorSize);
        Mpint32Poly poly1 =
            Mpint32Poly::LagrangeInterpolation(resizedInput + subvectorSize * (nPoly + i), subvectorSize);
        gPoly += poly0 * poly1;
        poly0s.emplace_back(poly0);
        poly1s.emplace_back(poly1);
    }

    delete[] resizedInput;

    return InteractiveProof(poly0s, poly1s, gPoly);
}

std::vector<Query> InnerProductCircuit::MakeRoundQuery(Mpint32 random, size_t subvectorSize)
{
    const size_t queryLength = subvectorSize * 2u - 1u;

    std::vector<Query> queryVectors;
    queryVectors.reserve(2);

    Mpint32* const queries = new Mpint32[queryLength * 2u];
    Mpint32* queriesCurr = queries;

    Mpint32* const powers = new Mpint32[subvectorSize];
    for (size_t j = 0; j < subvectorSize; ++j)
    {
        powers[j] = Mpint32(1u);
    }

    *queriesCurr = Mpint32(subvectorSize);
    ++queriesCurr;
    for (size_t i = 1; i < queryLength; ++i)
    {
        Mpint32 sum(0u);
        for (size_t j = 0; j < subvectorSize - 1; ++j)
        {
            powers[j] *= Mpint32(j + 1);
            sum += powers[j];
        }
        *queriesCurr = sum;
        ++queriesCurr;
    }

    Mpint32 power(1u);
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

InteractiveProof InnerProductCircuit::MakeRoundCoefficientProof(Mpint32* op0, Mpint32* op1, const size_t length,
                                                     size_t subvectorSize)
{
    assert(length > 0);

    const size_t nPoly = ceil(length / (double)subvectorSize);

    assert(nPoly > 1);

    Mpint32* const resizedInput = new Mpint32[nPoly * subvectorSize * 2u];
    std::memset(resizedInput, 0, (nPoly * subvectorSize * 2u) * sizeof(Mpint32));
    std::memcpy(resizedInput, op0, length * sizeof(Mpint32));
    std::memcpy(resizedInput + nPoly * subvectorSize, op1, length * sizeof(Mpint32));

    Mpint32Poly gPoly;
    std::vector<Mpint32Poly> poly0s;
    poly0s.reserve(nPoly);
    std::vector<Mpint32Poly> poly1s;
    poly1s.reserve(nPoly);
    for (size_t i = 0; i < nPoly; ++i)
    {
        Mpint32Poly poly0(resizedInput + subvectorSize * i, subvectorSize, true);
        Mpint32::Reverse(resizedInput + subvectorSize * (nPoly + i),
                         resizedInput + subvectorSize * (nPoly + i) + subvectorSize - 1);
        Mpint32Poly poly1(resizedInput + subvectorSize * (nPoly + i), subvectorSize, true);
        gPoly += poly0 * poly1;
        poly0s.emplace_back(poly0);
        poly1s.emplace_back(poly1);
    }

    delete[] resizedInput;

    return InteractiveProof(poly0s, poly1s, gPoly);
}

std::vector<Query> InnerProductCircuit::MakeRoundCoefficientQuery(Mpint32 random, size_t subvectorSize)
{
    const size_t queryLength = subvectorSize * 2u - 1u;

    std::vector<Query> queryVectors;
    queryVectors.reserve(2);

    Mpint32* const queries = new Mpint32[queryLength * 2u];
    std::memset(queries, 0, (queryLength * 2u) * sizeof(Mpint32));
    
    queries[subvectorSize - 1u] = Mpint32(1u);

    Mpint32* queriesCurr = queries + queryLength;
    Mpint32 power(1u);
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