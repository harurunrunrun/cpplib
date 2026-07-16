#pragma once

#include <limits>
#include <vector>

#include "compressed_wavelet_matrix.hpp"

template<
    class T,
    int MAX_SIZE,
    int ID_BIT_WIDTH = std::numeric_limits<unsigned>::digits
>
class StaticRangeGreaterCount{
private:
    CompressedWaveletMatrix<T, MAX_SIZE, ID_BIT_WIDTH> matrix;

public:
    explicit StaticRangeGreaterCount(const std::vector<T>& values):
        matrix(values){}

    int size() const{
        return matrix.size();
    }

    int count_greater(int left, int right, const T& threshold) const{
        return matrix.count_greater(left, right, threshold);
    }
};
