#pragma once

#include <limits>
#include <type_traits>
#include "persistent_wavelet_matrix_2d_weighted.hpp"

template<
    class X,
    class Y,
    class W,
    int MAX_SIZE,
    int MAX_VERSION,
    int Y_BIT_WIDTH = std::numeric_limits<std::make_unsigned_t<Y>>::digits,
    int BLOCK_SIZE = 512
>
struct PersistentRectangleSum :
    PersistentWaveletMatrix2DWeighted<X, Y, W, MAX_SIZE, MAX_VERSION, Y_BIT_WIDTH, BLOCK_SIZE>{
    using Base = PersistentWaveletMatrix2DWeighted<X, Y, W, MAX_SIZE, MAX_VERSION, Y_BIT_WIDTH, BLOCK_SIZE>;
    using Base::Base;
};
