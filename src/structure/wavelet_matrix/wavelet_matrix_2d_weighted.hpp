#ifndef CPPLIB_SRC_STRUCTURE_WAVELET_MATRIX_WAVELET_MATRIX_2D_WEIGHTED_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_WAVELET_MATRIX_WAVELET_MATRIX_2D_WEIGHTED_HPP_INCLUDED

#include <algorithm>
#include <array>
#include <cstddef>
#include <limits>
#include <memory>
#include <numeric>
#include <stdexcept>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>
#include "weighted_wavelet_matrix.hpp"

template<
    class X,
    class Y,
    class W,
    int MAX_SIZE,
    int Y_BIT_WIDTH = std::numeric_limits<std::make_unsigned_t<Y>>::digits
>
struct WaveletMatrix2DWeighted{
    static_assert(std::is_integral_v<X>);
    static_assert(std::is_integral_v<Y>);
    static_assert(MAX_SIZE >= 0);

private:
    struct State{
        std::array<X, MAX_SIZE> x{};
        std::unique_ptr<WeightedWaveletMatrix<Y, W, MAX_SIZE, Y_BIT_WIDTH>> matrix;
    };

    int _n = 0;
    std::unique_ptr<State> state;

    void check_rect(X xl, X xr, Y yl, Y yr, const char* message) const{
        if(xr < xl || yr < yl)[[unlikely]] throw std::runtime_error(message);
    }

    std::pair<int, int> x_range(X xl, X xr) const{
        const auto first = state->x.begin();
        int l = static_cast<int>(std::lower_bound(first, first + _n, xl) - first);
        int r = static_cast<int>(std::lower_bound(first, first + _n, xr) - first);
        return {l, r};
    }

public:
    WaveletMatrix2DWeighted(
        const std::vector<X>& xs,
        const std::vector<Y>& ys,
        const std::vector<W>& weights
    ): _n(static_cast<int>(xs.size())), state(nullptr){
        if(xs.size() != ys.size() || xs.size() != weights.size())[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (constructor).");
        }
        if(xs.size() > static_cast<std::size_t>(MAX_SIZE))[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (constructor).");
        }
        state = std::make_unique<State>();
        std::vector<int> order(static_cast<std::size_t>(_n));
        std::iota(order.begin(), order.end(), 0);
        std::sort(order.begin(), order.end(), [&](int a, int b){
            if(xs[static_cast<std::size_t>(a)] != xs[static_cast<std::size_t>(b)]){
                return xs[static_cast<std::size_t>(a)] < xs[static_cast<std::size_t>(b)];
            }
            return ys[static_cast<std::size_t>(a)] < ys[static_cast<std::size_t>(b)];
        });
        std::vector<Y> sorted_y(static_cast<std::size_t>(_n));
        std::vector<W> sorted_weight(static_cast<std::size_t>(_n));
        for(int k = 0; k < _n; k++){
            int index = order[static_cast<std::size_t>(k)];
            state->x[k] = xs[static_cast<std::size_t>(index)];
            sorted_y[static_cast<std::size_t>(k)] = ys[static_cast<std::size_t>(index)];
            sorted_weight[static_cast<std::size_t>(k)] = weights[static_cast<std::size_t>(index)];
        }
        state->matrix =
            std::make_unique<WeightedWaveletMatrix<Y, W, MAX_SIZE, Y_BIT_WIDTH>>(
                sorted_y, sorted_weight);
    }

    explicit WaveletMatrix2DWeighted(const std::vector<std::tuple<X, Y, W>>& points):
        _n(static_cast<int>(points.size())), state(nullptr){
        if(points.size() > static_cast<std::size_t>(MAX_SIZE))[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (constructor).");
        }
        state = std::make_unique<State>();
        std::vector<std::tuple<X, Y, W>> sorted = points;
        std::sort(sorted.begin(), sorted.end(), [](const auto& a, const auto& b){
            if(std::get<0>(a) != std::get<0>(b)) return std::get<0>(a) < std::get<0>(b);
            return std::get<1>(a) < std::get<1>(b);
        });
        std::vector<Y> sorted_y(sorted.size());
        std::vector<W> sorted_weight(sorted.size());
        for(int k = 0; k < _n; k++){
            state->x[k] = std::get<0>(sorted[static_cast<std::size_t>(k)]);
            sorted_y[static_cast<std::size_t>(k)] =
                std::get<1>(sorted[static_cast<std::size_t>(k)]);
            sorted_weight[static_cast<std::size_t>(k)] =
                std::get<2>(sorted[static_cast<std::size_t>(k)]);
        }
        state->matrix =
            std::make_unique<WeightedWaveletMatrix<Y, W, MAX_SIZE, Y_BIT_WIDTH>>(
                sorted_y, sorted_weight);
    }

    WaveletMatrix2DWeighted(const WaveletMatrix2DWeighted&) = delete;
    WaveletMatrix2DWeighted& operator=(const WaveletMatrix2DWeighted&) = delete;
    WaveletMatrix2DWeighted(WaveletMatrix2DWeighted&&) = default;
    WaveletMatrix2DWeighted& operator=(WaveletMatrix2DWeighted&&) = default;

    int size() const{ return _n; }

    int rectangle_count(X xl, X xr, Y yl, Y yr) const{
        check_rect(xl, xr, yl, yr, "library assertion fault: range violation (rectangle_count).");
        auto [l, r] = x_range(xl, xr);
        return state->matrix->range_freq(l, r, yl, yr);
    }
    W rectangle_sum(X xl, X xr, Y yl, Y yr) const{
        check_rect(xl, xr, yl, yr, "library assertion fault: range violation (rectangle_sum).");
        auto [l, r] = x_range(xl, xr);
        return state->matrix->range_sum(l, r, yl, yr);
    }
    int range_freq(X xl, X xr, Y yl, Y yr) const{
        return rectangle_count(xl, xr, yl, yr);
    }
    W range_sum(X xl, X xr, Y yl, Y yr) const{
        return rectangle_sum(xl, xr, yl, yr);
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_WAVELET_MATRIX_WAVELET_MATRIX_2D_WEIGHTED_HPP_INCLUDED
