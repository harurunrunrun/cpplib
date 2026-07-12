#pragma once

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
#include "partially_persistent_weighted_wavelet_matrix.hpp"

template<
    class X,
    class Y,
    class W,
    int MAX_SIZE,
    int MAX_VERSION,
    int Y_BIT_WIDTH = std::numeric_limits<std::make_unsigned_t<Y>>::digits,
    int BLOCK_SIZE = 512
>
struct PartiallyPersistentWaveletMatrix2DWeighted{
    static_assert(std::is_integral_v<X>);
    static_assert(std::is_integral_v<Y>);
    static_assert(MAX_SIZE >= 0);
    static_assert(MAX_VERSION >= 0);

private:
    using Matrix = PartiallyPersistentWeightedWaveletMatrix<Y, W, MAX_SIZE, MAX_VERSION, Y_BIT_WIDTH, BLOCK_SIZE>;
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
    void build(const std::vector<X>& xs, const std::vector<Y>& ys, const std::vector<W>& weights){
        _n = static_cast<int>(xs.size());
        if(xs.size() != ys.size() || xs.size() != weights.size())[[unlikely]] throw std::runtime_error("library assertion fault: range violation (constructor).");
        if(xs.size() > static_cast<std::size_t>(MAX_SIZE))[[unlikely]] throw std::runtime_error("library assertion fault: range violation (constructor).");
        state = std::make_unique<State>();
        std::vector<int> order(static_cast<std::size_t>(_n));
        std::iota(order.begin(), order.end(), 0);
        std::sort(order.begin(), order.end(), [&](int lhs, int rhs){
            if(xs[static_cast<std::size_t>(lhs)] != xs[static_cast<std::size_t>(rhs)]) return xs[static_cast<std::size_t>(lhs)] < xs[static_cast<std::size_t>(rhs)];
            if(ys[static_cast<std::size_t>(lhs)] != ys[static_cast<std::size_t>(rhs)]) return ys[static_cast<std::size_t>(lhs)] < ys[static_cast<std::size_t>(rhs)];
            return lhs < rhs;
        });
        std::vector<Y> sorted_y(static_cast<std::size_t>(_n));
        std::vector<W> sorted_weight(static_cast<std::size_t>(_n));
        for(int i = 0; i < _n; i++){
            int index = order[static_cast<std::size_t>(i)];
            state->x[static_cast<std::size_t>(i)] = xs[static_cast<std::size_t>(index)];
            state->original_x[static_cast<std::size_t>(index)] = xs[static_cast<std::size_t>(index)];
            state->original_to_sorted[static_cast<std::size_t>(index)] = i;
            sorted_y[static_cast<std::size_t>(i)] = ys[static_cast<std::size_t>(index)];
            sorted_weight[static_cast<std::size_t>(i)] = weights[static_cast<std::size_t>(index)];
        }
        state->matrix = std::make_unique<Matrix>(sorted_y, sorted_weight);
    }
public:
    PartiallyPersistentWaveletMatrix2DWeighted():
        PartiallyPersistentWaveletMatrix2DWeighted(
            std::vector<X>{}, std::vector<Y>{}, std::vector<W>{}
        ){}
    PartiallyPersistentWaveletMatrix2DWeighted(const std::vector<X>& xs, const std::vector<Y>& ys, const std::vector<W>& weights){ build(xs, ys, weights); }
    explicit PartiallyPersistentWaveletMatrix2DWeighted(const std::vector<std::tuple<X, Y, W>>& points){
        std::vector<X> xs(points.size());
        std::vector<Y> ys(points.size());
        std::vector<W> weights(points.size());
        for(std::size_t i = 0; i < points.size(); i++){
            xs[i] = std::get<0>(points[i]); ys[i] = std::get<1>(points[i]); weights[i] = std::get<2>(points[i]);
        }
        build(xs, ys, weights);
    }
    int size() const{ return _n; }
    int versions() const{ return state->matrix->versions(); }
    int latest_version() const{ return state->matrix->latest_version(); }
    X x(int k) const{ check_index(k, "library assertion fault: range violation (x)."); return state->original_x[static_cast<std::size_t>(k)]; }
    Y y(int version, int k) const{ check_index(k, "library assertion fault: range violation (y)."); return state->matrix->access(version, state->original_to_sorted[static_cast<std::size_t>(k)]); }
    W weight(int version, int k) const{ check_index(k, "library assertion fault: range violation (weight)."); return state->matrix->weight(version, state->original_to_sorted[static_cast<std::size_t>(k)]); }
    int set(int k, Y y_value, W weight_value){ check_index(k, "library assertion fault: range violation (set)."); return state->matrix->set(state->original_to_sorted[static_cast<std::size_t>(k)], y_value, weight_value); }
    int set_y(int k, Y value){ return set(k, value, weight(latest_version(), k)); }
    int set_weight(int k, W value){ return set(k, y(latest_version(), k), value); }
    int rectangle_count(int version, X xl, X xr, Y yl, Y yr) const{
        check_rect(xl, xr, yl, yr, "library assertion fault: range violation (rectangle_count)."); auto [l, r] = x_range(xl, xr); return state->matrix->range_freq(version, l, r, yl, yr);
    }
    W rectangle_sum(int version, X xl, X xr, Y yl, Y yr) const{
        check_rect(xl, xr, yl, yr, "library assertion fault: range violation (rectangle_sum)."); auto [l, r] = x_range(xl, xr); return state->matrix->range_sum(version, l, r, yl, yr);
    }
    int range_freq(int version, X xl, X xr, Y yl, Y yr) const{ return rectangle_count(version, xl, xr, yl, yr); }
    W range_sum(int version, X xl, X xr, Y yl, Y yr) const{ return rectangle_sum(version, xl, xr, yl, yr); }
};
