#ifndef CPPLIB_SRC_STRUCTURE_WAVELET_MATRIX_WAVELET_MATRIX_2D_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_WAVELET_MATRIX_WAVELET_MATRIX_2D_HPP_INCLUDED

#include <algorithm>
#include <array>
#include <cstddef>
#include <limits>
#include <memory>
#include <numeric>
#include <optional>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>
#include "wavelet_matrix.hpp"

template<
    class X,
    class Y,
    int MAX_SIZE,
    int Y_BIT_WIDTH = std::numeric_limits<std::make_unsigned_t<Y>>::digits
>
struct WaveletMatrix2D{
    static_assert(std::is_integral_v<X>);
    static_assert(std::is_integral_v<Y>);
    static_assert(MAX_SIZE >= 0);

private:
    struct State{
        std::array<X, MAX_SIZE> x{};
        std::unique_ptr<WaveletMatrix<Y, MAX_SIZE, Y_BIT_WIDTH>> matrix;
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
    WaveletMatrix2D(const std::vector<X>& xs, const std::vector<Y>& ys):
        _n(static_cast<int>(xs.size())), state(nullptr){
        if(xs.size() != ys.size())[[unlikely]]{
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
        for(int k = 0; k < _n; k++){
            int index = order[static_cast<std::size_t>(k)];
            state->x[k] = xs[static_cast<std::size_t>(index)];
            sorted_y[static_cast<std::size_t>(k)] = ys[static_cast<std::size_t>(index)];
        }
        state->matrix = std::make_unique<WaveletMatrix<Y, MAX_SIZE, Y_BIT_WIDTH>>(sorted_y);
    }

    explicit WaveletMatrix2D(const std::vector<std::pair<X, Y>>& points):
        _n(static_cast<int>(points.size())), state(nullptr){
        if(points.size() > static_cast<std::size_t>(MAX_SIZE))[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (constructor).");
        }
        state = std::make_unique<State>();
        std::vector<std::pair<X, Y>> sorted = points;
        std::sort(sorted.begin(), sorted.end());
        std::vector<Y> sorted_y(sorted.size());
        for(int k = 0; k < _n; k++){
            state->x[k] = sorted[static_cast<std::size_t>(k)].first;
            sorted_y[static_cast<std::size_t>(k)] = sorted[static_cast<std::size_t>(k)].second;
        }
        state->matrix = std::make_unique<WaveletMatrix<Y, MAX_SIZE, Y_BIT_WIDTH>>(sorted_y);
    }

    WaveletMatrix2D(const WaveletMatrix2D&) = delete;
    WaveletMatrix2D& operator=(const WaveletMatrix2D&) = delete;
    WaveletMatrix2D(WaveletMatrix2D&&) = default;
    WaveletMatrix2D& operator=(WaveletMatrix2D&&) = default;

    int size() const{ return _n; }

    int rectangle_count(X xl, X xr, Y yl, Y yr) const{
        check_rect(xl, xr, yl, yr, "library assertion fault: range violation (rectangle_count).");
        auto [l, r] = x_range(xl, xr);
        return state->matrix->range_freq(l, r, yl, yr);
    }
    int range_freq(X xl, X xr, Y yl, Y yr) const{
        return rectangle_count(xl, xr, yl, yr);
    }

    Y kth_smallest_y(X xl, X xr, int k) const{
        if(xr < xl)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (kth_smallest_y).");
        }
        auto [l, r] = x_range(xl, xr);
        return state->matrix->kth_smallest(l, r, k);
    }
    std::optional<Y> prev_y(X xl, X xr, Y upper) const{
        if(xr < xl)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (prev_y).");
        }
        auto [l, r] = x_range(xl, xr);
        return state->matrix->prev_value(l, r, upper);
    }
    std::optional<Y> next_y(X xl, X xr, Y lower) const{
        if(xr < xl)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (next_y).");
        }
        auto [l, r] = x_range(xl, xr);
        return state->matrix->next_value(l, r, lower);
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_WAVELET_MATRIX_WAVELET_MATRIX_2D_HPP_INCLUDED
