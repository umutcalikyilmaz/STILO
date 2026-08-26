#pragma once
#include <span>
#include <vector>

namespace STILO
{
    template <typename T>
    class Matrix
    {
    public:

        Matrix()
            : rowCount_(0),
              columnCount_(0) { }

        Matrix(const Matrix& original)
            : rowCount_(original.rowCount_),
              columnCount_(original.columnCount_),
              data_(original.data_) { }

        Matrix(int rowCount, int columnCount)
            : rowCount_(rowCount),
              columnCount_(columnCount),
              data_(rowCount * columnCount) { }

        std::span<T> getRow(int row)
        {
            return std::span<T>(data_).subspan(row * columnCount_, columnCount_);
        }

        std::span<T> operator[](int row)
        {
            return std::span<T>(data_).subspan(row * columnCount_, columnCount_);
        }

        T getElement(int row, int column)
        {
            return data_[row * columnCount_ + column];
        }

    private:

        std::vector<T> data_;
        std::size_t rowCount_;
        std::size_t columnCount_;
    };
}