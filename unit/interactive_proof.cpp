#include "interactive_proof.h"

InteractiveProof::InteractiveProof(std::vector<Mpint32Poly>& pPolys, std::vector<Mpint32Poly>& qPolys,
                                   Mpint32Poly& gPoly)
{
    mPolyPs.reserve(pPolys.size());
    for (size_t i = 0; i < pPolys.size(); ++i)
    {
        mPolyPs.emplace_back(pPolys[i]);
    }

    mPolyQs.reserve(qPolys.size());
    for (size_t i = 0; i < qPolys.size(); ++i)
    {
        mPolyQs.emplace_back(qPolys[i]);
    }

    mProof = Proof(gPoly);
    mIsFinalRound = false;
}

Mpint32 InteractiveProof::GetQueryAnswer(const Query& query)
{
    return mProof.GetQueryAnswer(query);
}

size_t InteractiveProof::GetBytes()
{
    return mProof.GetBytes();
}

std::vector<Proof> InteractiveProof::GetShares(size_t nShares)
{
    return mProof.GetShares(nShares);
}

std::vector<Mpint32> InteractiveProof::EvaluatePolyPs(Mpint32 x)
{
    std::vector<Mpint32> results;
    results.reserve(mPolyPs.size());
    for (size_t i = 0; i < mPolyPs.size(); ++i)
    {
        results.emplace_back(mPolyPs[i].Evaluate(x));
    }
    return results;
}

std::vector<Mpint32> InteractiveProof::EvaluatePolyQs(Mpint32 x)
{
    std::vector<Mpint32> results;
    results.reserve(mPolyQs.size());
    for (size_t i = 0; i < mPolyQs.size(); ++i)
    {
        results.emplace_back(mPolyQs[i].Evaluate(x));
    }
    return results;
}
