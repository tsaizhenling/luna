#include <vector>
#include <iostream>

template <class T>
T& Matrix<T>::operator()(size_t i, size_t j)
{
    return mData[i * mCols + j];
}

template <class T>
T Matrix<T>::operator()(size_t i, size_t j) const
{
    return mData[i * mCols + j];
}

template <class T>
Matrix<T>::Matrix(size_t rows, size_t cols)
: mRows(rows),
  mCols(cols),
  mData(rows * cols)
{
}