#include <cassert>

#include "query.h"

Query::Query()
{
    mValues = (Mpint32*)0;
    mLength = 0u;
}

Query::Query(Mpint32* values, size_t length)
{
    assert(length > 0);
    mValues = new Mpint32[length];
    std::memcpy(mValues, values, length * sizeof(Mpint32));
    mLength = length;
}

Query::~Query()
{
    delete[] mValues;
}

Query& Query::operator=(const Query& obj)
{
    if (mValues != (Mpint32*)0)
    {
        delete[] mValues;
    }

    mValues = new Mpint32[obj.mLength];
    std::memcpy(mValues, obj.mValues, obj.mLength * sizeof(Mpint32));
    mLength = obj.mLength;

    return *this;
}
