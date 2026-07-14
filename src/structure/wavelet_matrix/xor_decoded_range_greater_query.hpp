#pragma once

#include <algorithm>
#include <concepts>
#include <limits>
#include <vector>

#include "compressed_wavelet_matrix.hpp"

template<
    std::integral T,
    int MAX_SIZE,
    int ID_BIT_WIDTH = std::numeric_limits<unsigned>::digits
>
class XorDecodedRangeGreaterQuery{
    CompressedWaveletMatrix<T, MAX_SIZE, ID_BIT_WIDTH> matrix_;
    int last_answer_ = 0;

public:
    explicit XorDecodedRangeGreaterQuery(const std::vector<T>& values):
        matrix_(values){}

    int size() const{
        return matrix_.size();
    }

    int last_answer() const{
        return last_answer_;
    }

    int query(
        int encoded_left,
        int encoded_right,
        T encoded_threshold
    ){
        int left = encoded_left ^ last_answer_;
        int right = encoded_right ^ last_answer_;
        const T threshold = encoded_threshold ^ static_cast<T>(last_answer_);
        left = std::max(left, 1);
        right = std::min(right, size());
        if(left > right){
            last_answer_ = 0;
        }else{
            last_answer_ = matrix_.count_greater(left - 1, right, threshold);
        }
        return last_answer_;
    }

    void reset(){
        last_answer_ = 0;
    }
};
