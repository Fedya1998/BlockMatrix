//
// Created by Fedya on 2019-12-04.
//

#ifndef HW1_MYMATRIX_H
#define HW1_MYMATRIX_H

#include <cstdlib>

template<typename T, size_t N>
struct Block {
    T data_[N][N];

    Block() : data_{} {}

    Block operator*(const Block &right) const;

    Block &operator*=(const Block &right);

    Block operator+(const Block &right) const;

    Block &operator+=(const Block &right);
};

template<typename T, size_t N>
Block<T, N> &Block<T, N>::operator+=(const Block &right) {
    *this = (*this) + right;
    return *this;
}

template<typename T, size_t N>
Block<T, N> Block<T, N>::operator+(const Block &right) const {
    Block res = *this;
    for (size_t i = 0; i < N; ++i)
        for (size_t j = 0; j < N; ++j)
            res.data_[i][j] += right.data_[i][j];
    return res;
}

template<typename T, size_t N>
Block<T, N> &Block<T, N>::operator*=(const Block &right) {
    *this = (*this) * right;
    return *this;
}

template<typename T, size_t N>
Block<T, N> Block<T, N>::operator*(const Block &right) const {
    Block res;
    for (size_t i = 0; i < N; ++i)
        for (size_t j = 0; j < N; ++j)
            for (size_t k = 0; k < N; ++k)
                res.data_[i][j] += this->data_[i][k] * right.data_[k][j];
    return res;
}

template<typename T, size_t N = 32> // 4-0.5t, 8,16,32-0.3t, 64,128,256-0.5t (data doesn't fit in cache line)
class BlockMatrix {
private:
    Block<T, N> *data_;
    size_t rows_;
    size_t columns_;

public:
    BlockMatrix(size_t rows, size_t columns);

    ~BlockMatrix();

    BlockMatrix operator*(const BlockMatrix &right) const;

    BlockMatrix &operator*=(const BlockMatrix &right);

    T &operator()(size_t i, size_t j);

    size_t CalcBlockNumber(size_t size, size_t block_size) const;
};

template<typename T, size_t N>
T &BlockMatrix<T, N>::operator()(size_t i, size_t j) {
    size_t i_block = i / N;
    size_t j_block = j / N;
    return data_[i_block * CalcBlockNumber(columns_, N) + j_block].data_[i % N][j % N];
}

template<typename T, size_t N>
BlockMatrix<T, N> BlockMatrix<T, N>::operator*(const BlockMatrix<T, N> &right) const {
    BlockMatrix<T, N> res(this->rows_, right.columns_);
    for (size_t i = 0; i < CalcBlockNumber(res.rows_, N); ++i)
        for (size_t j = 0; j < CalcBlockNumber(res.columns_, N); ++j) {
            Block<T, N> sum;
            for (size_t k = 0; k < CalcBlockNumber(this->columns_, N); ++k)
                sum += this->data_[i * CalcBlockNumber(this->columns_, N) + k] *
                        right.data_[k * CalcBlockNumber(right.columns_, N) + j];
            res.data_[i * CalcBlockNumber(res.columns_, N) + j] = sum;
        }
    return res;
}

template<typename T, size_t N>
BlockMatrix<T, N> &BlockMatrix<T, N>::operator*=(const BlockMatrix &right) {
    *this = (*this) * right;
    return *this;
}

template<typename T, size_t N>
BlockMatrix<T, N>::BlockMatrix(size_t rows, size_t columns) :
        data_(new Block<T, N>[CalcBlockNumber(rows, N) * CalcBlockNumber(columns, N)]),
        rows_(rows),
        columns_(columns)
        {}

template<typename T, size_t N>
BlockMatrix<T, N>::~BlockMatrix() {
    delete[] data_;
}

template<typename T, size_t N>
size_t BlockMatrix<T, N>::CalcBlockNumber(const size_t size, const size_t block_size) const{
    return size / block_size + (size % block_size != 0);
}


#endif //HW1_MYMATRIX_H
