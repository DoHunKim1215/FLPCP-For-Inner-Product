#ifndef INNER_PRODUCT_CIRCUIT_H
#define INNER_PRODUCT_CIRCUIT_H

#include "..\math\mpint32.h"
#include "..\unit\proof.h"
#include "..\unit\interactive_proof.h"
#include "..\unit\query.h"

class InnerProductCircuit
{
public:
    static Mpint32 Forward(Mpint32* op0, Mpint32* op1, const size_t length);
    static Proof MakeProof(Mpint32* op0, Mpint32* op1, const size_t length, const size_t nGGate);
    static std::vector<Query> MakeQuery(Mpint32 random, const size_t nGGate, const size_t inputSize);
    static Proof MakeCoefficientProof(Mpint32* op0, Mpint32* op1, const size_t length, const size_t nPoly);
    static std::vector<Query> MakeCoefficientQuery(Mpint32 random, size_t inputSize, const size_t nPoly);
    static InteractiveProof MakeRoundProof(Mpint32* op0, Mpint32* op1, const size_t length, size_t subvectorSize);
    static std::vector<Query> MakeRoundQuery(Mpint32 random, size_t subvectorSize);
    static InteractiveProof MakeRoundCoefficientProof(Mpint32* op0, Mpint32* op1, const size_t length, size_t subvectorSize);
    static std::vector<Query> MakeRoundCoefficientQuery(Mpint32 random, size_t subvectorSize);
};

#endif
