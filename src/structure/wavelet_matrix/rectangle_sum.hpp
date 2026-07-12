#pragma once

#include <limits>
#include <type_traits>
#include "wavelet_matrix_2d_weighted.hpp"

template<
    class X,
    class Y,
    class W,
    int MAX_SIZE,
    int Y_BIT_WIDTH = std::numeric_limits<std::make_unsigned_t<Y>>::digits
>
struct RectangleSum : WaveletMatrix2DWeighted<X, Y, W, MAX_SIZE, Y_BIT_WIDTH>{
    using Base = WaveletMatrix2DWeighted<X, Y, W, MAX_SIZE, Y_BIT_WIDTH>;
    using Base::Base;
};
