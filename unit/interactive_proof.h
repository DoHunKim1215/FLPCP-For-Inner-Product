#ifndef INTERACTIVE_PROOF_H
#define INTERACTIVE_PROOF_H

#include "..\math\polynomial.h"
#include "proof.h"

class InteractiveProof
{
public:
    InteractiveProof(std::vector<Mpint32Poly>& polyPs, std::vector<Mpint32Poly>& polyQs,
                     Mpint32Poly& polyG);

    Mpint32 GetQueryAnswer(const Query& query);
    size_t GetBytes();
    std::vector<Proof> GetShares(size_t nShares);
    std::vector<Mpint32> EvaluatePolyPs(Mpint32 x);
    std::vector<Mpint32> EvaluatePolyQs(Mpint32 x);

private:
    std::vector<Mpint32Poly> mPolyPs;
    std::vector<Mpint32Poly> mPolyQs;
    Proof mProof;
    bool mIsFinalRound;
};

#endif
