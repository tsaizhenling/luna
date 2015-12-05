#include <vector>

template <class T>
class Matrix
{
public:
    Matrix(size_t rows, size_t cols);
    T& operator()(size_t i, size_t j);
    T operator()(size_t i, size_t j) const;

private:
    size_t mRows;
    size_t mCols;
    std::vector<T> mData;
};

#include "matrix.tpp"