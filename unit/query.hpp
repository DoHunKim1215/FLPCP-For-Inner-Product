#ifndef QUERY_H
#define QUERY_H

#include <cassert>

template <typename Int> class Proof;

template <typename Int> class Query
{
public:
    friend Int Proof<Int>::GetQueryAnswer(const Query<Int>&) const;

    Query();
    Query(Int* values, size_t length);
    ~Query();

    Query<Int>& operator=(const Query<Int>& obj);

private:
    Int* mValues;
    size_t mLength;
};

template <typename Int> Query<Int>::Query()
{
    mValues = (Int*)0;
    mLength = 0u;
}

template <typename Int> Query<Int>::Query(Int* values, size_t length)
{
    assert(length > 0);
    mValues = new Int[length];
    std::memcpy(mValues, values, length * sizeof(Int));
    mLength = length;
}

template <typename Int> Query<Int>::~Query()
{
    delete[] mValues;
}

template <typename Int> Query<Int>& Query<Int>::operator=(const Query<Int>& obj)
{
    if (mValues != (Int*)0)
    {
        delete[] mValues;
    }

    mValues = new Int[obj.mLength];
    std::memcpy(mValues, obj.mValues, obj.mLength * sizeof(Int));
    mLength = obj.mLength;

    return *this;
}

#endif
