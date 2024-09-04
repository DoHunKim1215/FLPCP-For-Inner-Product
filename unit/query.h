#ifndef QUERY_H
#define QUERY_H

#include "..\math\mpint32.h"
#include "proof.h"

class Query
{
public:
    Query();
    Query(Mpint32* values, size_t length);
    ~Query();

    Query& operator=(const Query& obj);

private:
    Mpint32* mValues;
    size_t mLength;

    friend Mpint32 Proof::GetQueryAnswer(const Query&) const;
};

#endif
