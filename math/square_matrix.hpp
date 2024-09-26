#ifndef SQUARE_MATRIX_H
#define SQUARE_MATRIX_H

template <typename Int> class SquareMatrix
{
public:
    ~SquareMatrix();

    static SquareMatrix<Int> GetVandermonde(Int* xs, const size_t nXs);
    static SquareMatrix<Int> GetVandermondeInverse(const size_t n);

    void Inverse();
    Int Get(size_t i, size_t j);

private:
    SquareMatrix(Int* xs, const size_t nXs); // Create a square Vandermonde matrix

    Int* mValues;
    size_t mSize;
};

template <typename Int> SquareMatrix<Int>::~SquareMatrix()
{
    if (mValues != (Int*)0)
    {
        delete[] mValues;
    }
}

template <typename Int> SquareMatrix<Int> SquareMatrix<Int>::GetVandermonde(Int* xs, const size_t nXs)
{
    return SquareMatrix(xs, nXs);
}

template <typename Int> SquareMatrix<Int> SquareMatrix<Int>::GetVandermondeInverse(const size_t n)
{
    Int* xs = new Int[n];
    for (size_t i = 0; i < n; ++i)
    {
        xs[i] = Int(i);
    }
    SquareMatrix<Int> evalToCoeff = SquareMatrix<Int>::GetVandermonde(xs, n);
    evalToCoeff.Inverse();

    delete[] xs;

    return evalToCoeff;
}

template <typename Int> void SquareMatrix<Int>::Inverse()
{
    // Start with a identity matrix
    Int* const invValues = new Int[mSize * mSize];
    std::memset(invValues, 0, mSize * mSize * sizeof(Int));
    for (size_t i = 0; i < mSize; ++i)
    {
        invValues[i * mSize + i] = Int(1);
    }

    // Apply Gaussian elimination : O(n^3)
    for (size_t i = 0; i < mSize; ++i)
    {
        Int pivot = mValues[i * mSize + i];

        assert(pivot != Int((uint64_t)0)); // Singular matrix, no inverse

        for (size_t j = 0; j < mSize; ++j)
        {
            mValues[i * mSize + j] /= pivot;
            invValues[i * mSize + j] /= pivot;
        }

        for (size_t k = 0; k < mSize; ++k)
        {
            if (k != i)
            {
                Int factor = mValues[k * mSize + i];
                for (size_t j = 0; j < mSize; ++j)
                {
                    mValues[k * mSize + j] -= factor * mValues[i * mSize + j];
                    invValues[k * mSize + j] -= factor * invValues[i * mSize + j];
                }
            }
        }
    }

    delete[] mValues;
    mValues = invValues;
}

template <typename Int> Int SquareMatrix<Int>::Get(size_t i, size_t j)
{
    return mValues[i * mSize + j];
}

template <typename Int> SquareMatrix<Int>::SquareMatrix(Int* xs, const size_t nXs)
{
    assert(nXs >= 2);

    mValues = new Int[nXs * nXs];
    mSize = nXs;
    for (size_t i = 0; i < mSize; ++i)
    {
        Int x = xs[i];
        mValues[i * mSize] = Int(1);
        for (size_t j = 1; j < mSize; ++j)
        {
            mValues[i * mSize + j] = x;
            x *= xs[i];
        }
    }
}

#endif
