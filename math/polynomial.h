#ifndef POLYNOMIAL_H
#define POLYNOMIAL_H

#include "mpint32.h"
#include "..\unit\proof.h"

class Mpint32Poly
{
public:
    friend Proof::Proof(Mpint32Poly&);
    friend Proof::Proof(std::vector<Mpint32>&, std::vector<Mpint32>&, size_t, Mpint32*, size_t, Mpint32Poly&);
    friend Proof::Proof(Mpint32* op0, Mpint32* op1, size_t nInputs, Mpint32* randoms, size_t nRandoms,
                        Mpint32Poly& poly);

    Mpint32Poly();
    Mpint32Poly(const Mpint32Poly& obj);
    Mpint32Poly(Mpint32* coefficients, const size_t length, bool isDeepCopy = false);
    Mpint32Poly(Mpint32 random, Mpint32* inputs, const size_t length);
    ~Mpint32Poly();

    Mpint32 Evaluate(const Mpint32 x) const;

    static Mpint32Poly LagrangeInterpolation(Mpint32* points, const size_t nPoints);

    Mpint32Poly& operator=(const Mpint32Poly& op);
    Mpint32Poly operator+(const Mpint32Poly& op);
    void operator+=(const Mpint32Poly& op);
    Mpint32Poly operator-(const Mpint32Poly& op);
    Mpint32Poly operator*(const Mpint32Poly& op);

private:
    Mpint32* mCoefficients;
    size_t mCapacity;
};

#endif
