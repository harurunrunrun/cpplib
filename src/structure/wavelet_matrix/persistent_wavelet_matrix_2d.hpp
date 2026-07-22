#ifndef CPPLIB_SRC_STRUCTURE_WAVELET_MATRIX_PERSISTENT_WAVELET_MATRIX_2D_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_WAVELET_MATRIX_PERSISTENT_WAVELET_MATRIX_2D_HPP_INCLUDED

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
#include "persistent_wavelet_matrix.hpp"

template<
    class X,
    class Y,
    int MAX_SIZE,
    int MAX_VERSION,
    int Y_BIT_WIDTH = std::numeric_limits<std::make_unsigned_t<Y>>::digits,
    int BLOCK_SIZE = 512
>
struct PersistentWaveletMatrix2D{
    static_assert(std::is_integral_v<X>);
    static_assert(std::is_integral_v<Y>);
    static_assert(MAX_SIZE >= 0);
    static_assert(MAX_VERSION >= 0);

private:
    using Matrix = PersistentWaveletMatrix<Y, MAX_SIZE, MAX_VERSION, Y_BIT_WIDTH, BLOCK_SIZE>;

    struct State{
        std::array<X, MAX_SIZE> x{};
        std::array<X, MAX_SIZE> original_x{};
        std::array<int, MAX_SIZE> original_to_sorted{};
        std::unique_ptr<Matrix> matrix;
    };

    int _n = 0;
    std::unique_ptr<State> state;

    void check_index(int k, const char* message) const{
        if(k < 0 || _n <= k)[[unlikely]] throw std::runtime_error(message);
    }
    void check_rect(X xl, X xr, Y yl, Y yr, const char* message) const{
        if(xr < xl || yr < yl)[[unlikely]] throw std::runtime_error(message);
    }
    std::pair<int, int> x_range(X xl, X xr) const{
        const auto first = state->x.begin();
        int l = static_cast<int>(std::lower_bound(first, first + _n, xl) - first);
        int r = static_cast<int>(std::lower_bound(first, first + _n, xr) - first);
        return {l, r};
    }
    void build(const std::vector<X>& xs, const std::vector<Y>& ys){
        _n = static_cast<int>(xs.size());
        if(xs.size() != ys.size())[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (constructor).");
        }
        if(xs.size() > static_cast<std::size_t>(MAX_SIZE))[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (constructor).");
        }
        state = std::make_unique<State>();
        std::vector<int> order(static_cast<std::size_t>(_n));
        std::iota(order.begin(), order.end(), 0);
        std::sort(order.begin(), order.end(), [&](int lhs, int rhs){
            if(xs[static_cast<std::size_t>(lhs)] != xs[static_cast<std::size_t>(rhs)]){
                return xs[static_cast<std::size_t>(lhs)] < xs[static_cast<std::size_t>(rhs)];
            }
            if(ys[static_cast<std::size_t>(lhs)] != ys[static_cast<std::size_t>(rhs)]){
                return ys[static_cast<std::size_t>(lhs)] < ys[static_cast<std::size_t>(rhs)];
            }
            return lhs < rhs;
        });
        std::vector<Y> sorted_y(static_cast<std::size_t>(_n));
        for(int i = 0; i < _n; i++){
            int index = order[static_cast<std::size_t>(i)];
            state->x[static_cast<std::size_t>(i)] = xs[static_cast<std::size_t>(index)];
            state->original_x[static_cast<std::size_t>(index)] = xs[static_cast<std::size_t>(index)];
            state->original_to_sorted[static_cast<std::size_t>(index)] = i;
            sorted_y[static_cast<std::size_t>(i)] = ys[static_cast<std::size_t>(index)];
        }
        state->matrix = std::make_unique<Matrix>(sorted_y);
    }

public:
    PersistentWaveletMatrix2D():
        PersistentWaveletMatrix2D(std::vector<X>{}, std::vector<Y>{}){}
    PersistentWaveletMatrix2D(const std::vector<X>& xs, const std::vector<Y>& ys){ build(xs, ys); }
    explicit PersistentWaveletMatrix2D(const std::vector<std::pair<X, Y>>& points){
        std::vector<X> xs(points.size());
        std::vector<Y> ys(points.size());
        for(std::size_t i = 0; i < points.size(); i++){
            xs[i] = points[i].first;
            ys[i] = points[i].second;
        }
        build(xs, ys);
    }

    PersistentWaveletMatrix2D(const PersistentWaveletMatrix2D&) = delete;
    PersistentWaveletMatrix2D& operator=(const PersistentWaveletMatrix2D&) = delete;
    PersistentWaveletMatrix2D(PersistentWaveletMatrix2D&&) = default;
    PersistentWaveletMatrix2D& operator=(PersistentWaveletMatrix2D&&) = default;

    int size() const{ return _n; }
    int versions() const{ return state->matrix->versions(); }
    int latest_version() const{ return state->matrix->latest_version(); }
    X x(int k) const{
        check_index(k, "library assertion fault: range violation (x).");
        return state->original_x[static_cast<std::size_t>(k)];
    }
    Y y(int version, int k) const{
        check_index(k, "library assertion fault: range violation (y).");
        int sorted_index = state->original_to_sorted[static_cast<std::size_t>(k)];
        return state->matrix->access(version, sorted_index);
    }
    int set_y(int version, int k, Y value){
        check_index(k, "library assertion fault: range violation (set_y).");
        int sorted_index = state->original_to_sorted[static_cast<std::size_t>(k)];
        return state->matrix->set(version, sorted_index, value);
    }
    int fork(int version){ return state->matrix->fork(version); }
    int rectangle_count(int version, X xl, X xr, Y yl, Y yr) const{
        check_rect(xl, xr, yl, yr, "library assertion fault: range violation (rectangle_count).");
        auto [l, r] = x_range(xl, xr);
        return state->matrix->range_freq(version, l, r, yl, yr);
    }
    int range_freq(int version, X xl, X xr, Y yl, Y yr) const{
        return rectangle_count(version, xl, xr, yl, yr);
    }
    Y kth_smallest_y(int version, X xl, X xr, int k) const{
        if(xr < xl)[[unlikely]] throw std::runtime_error("library assertion fault: range violation (kth_smallest_y).");
        auto [l, r] = x_range(xl, xr);
        return state->matrix->kth_smallest(version, l, r, k);
    }
    std::optional<Y> prev_y(int version, X xl, X xr, Y upper) const{
        if(xr < xl)[[unlikely]] throw std::runtime_error("library assertion fault: range violation (prev_y).");
        auto [l, r] = x_range(xl, xr);
        return state->matrix->prev_value(version, l, r, upper);
    }
    std::optional<Y> next_y(int version, X xl, X xr, Y lower) const{
        if(xr < xl)[[unlikely]] throw std::runtime_error("library assertion fault: range violation (next_y).");
        auto [l, r] = x_range(xl, xr);
        return state->matrix->next_value(version, l, r, lower);
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_WAVELET_MATRIX_PERSISTENT_WAVELET_MATRIX_2D_HPP_INCLUDED
