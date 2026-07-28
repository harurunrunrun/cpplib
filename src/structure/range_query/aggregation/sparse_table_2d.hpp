#ifndef CPPLIB_SRC_STRUCTURE_RANGE_QUERY_AGGREGATION_SPARSE_TABLE_2D_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_RANGE_QUERY_AGGREGATION_SPARSE_TABLE_2D_HPP_INCLUDED

#include <bit>
#include <stdexcept>
#include <vector>
#include <utility>

template<class T, class Operation>
class SparseTable2D{
    int height_ = 0, width_ = 0, log_height_ = 0, log_width_ = 0;
    Operation operation_;
    std::vector<std::vector<std::vector<std::vector<T>>>> table_;

public:
    SparseTable2D() = default;
    SparseTable2D(const std::vector<std::vector<T>>& matrix, Operation operation = Operation{})
        : height_(static_cast<int>(matrix.size())),
          width_(height_ == 0 ? 0 : static_cast<int>(matrix.front().size())),
          operation_(std::move(operation)){
        for(const auto& row: matrix) if(static_cast<int>(row.size()) != width_)
            throw std::invalid_argument("SparseTable2D: ragged matrix");
        if(height_ == 0 || width_ == 0) return;
        log_height_ = static_cast<int>(std::bit_width(static_cast<unsigned int>(height_)));
        log_width_ = static_cast<int>(std::bit_width(static_cast<unsigned int>(width_)));
        table_.resize(log_height_);
        for(int kh = 0; kh < log_height_; ++kh){
            table_[kh].resize(log_width_);
            for(int kw = 0; kw < log_width_; ++kw)
                table_[kh][kw].assign(height_ - (1 << kh) + 1,
                                      std::vector<T>(width_ - (1 << kw) + 1));
        }
        table_[0][0] = matrix;
        for(int kw = 1; kw < log_width_; ++kw){
            const int half = 1 << (kw - 1);
            for(int y = 0; y < height_; ++y) for(int x = 0; x + (1 << kw) <= width_; ++x)
                table_[0][kw][y][x] = operation_(table_[0][kw - 1][y][x],
                                                 table_[0][kw - 1][y][x + half]);
        }
        for(int kh = 1; kh < log_height_; ++kh){
            const int half = 1 << (kh - 1);
            for(int kw = 0; kw < log_width_; ++kw)
                for(int y = 0; y + (1 << kh) <= height_; ++y)
                    for(int x = 0; x + (1 << kw) <= width_; ++x)
                        table_[kh][kw][y][x] = operation_(table_[kh - 1][kw][y][x],
                                                         table_[kh - 1][kw][y + half][x]);
        }
    }
    int height() const noexcept { return height_; }
    int width() const noexcept { return width_; }
    T prod(int top, int left, int bottom, int right) const {
        if(top < 0 || top >= bottom || bottom > height_ || left < 0 || left >= right || right > width_)
            throw std::out_of_range("SparseTable2D: invalid rectangle");
        const int kh = static_cast<int>(std::bit_width(static_cast<unsigned int>(bottom - top))) - 1;
        const int kw = static_cast<int>(std::bit_width(static_cast<unsigned int>(right - left))) - 1;
        const int y2 = bottom - (1 << kh), x2 = right - (1 << kw);
        return operation_(operation_(table_[kh][kw][top][left], table_[kh][kw][top][x2]),
                          operation_(table_[kh][kw][y2][left], table_[kh][kw][y2][x2]));
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_RANGE_QUERY_AGGREGATION_SPARSE_TABLE_2D_HPP_INCLUDED
