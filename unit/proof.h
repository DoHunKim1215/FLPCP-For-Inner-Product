#ifndef PROOF_H
#define PROOF_H

#include <vector>

#include "..\math\mpint32.h"

class Query;
class Mpint32Poly;

class Proof
{
public:
    Proof();
    Proof(Mpint32* values, size_t length, size_t proofLength);
    Proof(Mpint32Poly& poly);
    Proof(std::vector<Mpint32>& op0, std::vector<Mpint32>& op1, size_t nInputs, Mpint32* randoms, size_t nRandoms,
          Mpint32Poly& poly);
    Proof(Mpint32* op0, Mpint32* op1, size_t nInputs, Mpint32* randoms, size_t nRandoms, Mpint32Poly& poly);
    ~Proof();

    Mpint32 GetQueryAnswer(const Query& query) const;
    size_t GetBytes() const;
    size_t GetLength() const;
    std::vector<Proof> GetShares(size_t nShares);
    std::vector<Mpint32> GetRandoms(size_t nRandoms); 

    Proof& operator=(const Proof& obj);

private:
    Mpint32* mValues;
    size_t mLength;
    size_t mProofLength;
};

#endif
