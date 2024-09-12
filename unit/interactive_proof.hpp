#ifndef INTERACTIVE_PROOF_H
#define INTERACTIVE_PROOF_H

#include "..\math\polynomial.hpp"
#include "proof.hpp"

template <typename Int> class InteractiveProof
{
public:
    InteractiveProof(const InteractiveProof<Int>& obj);
    InteractiveProof(std::vector<Polynomial<Int>>& polyPs, std::vector<Polynomial<Int>>& polyQs,
                     Polynomial<Int>& polyG);

    Int GetQueryAnswer(const Query<Int>& query);
    size_t GetBytes();
    std::vector<Proof<Int>> GetShares(size_t nShares);
    std::vector<Int> EvaluatePolyPs(Int x);
    std::vector<Int> EvaluatePolyQs(Int x);
    Int GetRandomFromOracle();

private:
    std::vector<Polynomial<Int>> mPolyPs;
    std::vector<Polynomial<Int>> mPolyQs;
    Proof<Int> mProof;
    bool mIsFinalRound;
};

template <typename Int> InteractiveProof<Int>::InteractiveProof(const InteractiveProof<Int>& obj)
{
    mPolyPs = obj.mPolyPs;
    mPolyQs = obj.mPolyQs;
    mProof = obj.mProof;
    mIsFinalRound = obj.mIsFinalRound;
}

template <typename Int>
InteractiveProof<Int>::InteractiveProof(std::vector<Polynomial<Int>>& pPolys, std::vector<Polynomial<Int>>& qPolys,
                                        Polynomial<Int>& gPoly)
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

template <typename Int> Int InteractiveProof<Int>::GetQueryAnswer(const Query<Int>& query)
{
    return mProof.GetQueryAnswer(query);
}

template <typename Int> size_t InteractiveProof<Int>::GetBytes()
{
    return mProof.GetBytes();
}

template <typename Int> std::vector<Proof<Int>> InteractiveProof<Int>::GetShares(size_t nShares)
{
    return mProof.GetShares(nShares);
}

template <typename Int> std::vector<Int> InteractiveProof<Int>::EvaluatePolyPs(Int x)
{
    std::vector<Int> results;
    results.reserve(mPolyPs.size());
    for (size_t i = 0; i < mPolyPs.size(); ++i)
    {
        results.emplace_back(mPolyPs[i].Evaluate(x));
    }
    return results;
}

template <typename Int> std::vector<Int> InteractiveProof<Int>::EvaluatePolyQs(Int x)
{
    std::vector<Int> results;
    results.reserve(mPolyQs.size());
    for (size_t i = 0; i < mPolyQs.size(); ++i)
    {
        results.emplace_back(mPolyQs[i].Evaluate(x));
    }
    return results;
}

template <typename Int> Int InteractiveProof<Int>::GetRandomFromOracle()
{
    return mProof.GetRandomFromOracle();
}

#endif
