#pragma once

#include <limits>
#include <optional>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>
#include "dynamic_wavelet_matrix_2d_weighted.hpp"

template<class X, class Y, int MAX_SIZE,
         int Y_BIT_WIDTH = std::numeric_limits<std::make_unsigned_t<Y>>::digits,
         int BLOCK_SIZE = 512>
struct DynamicWaveletMatrix2D{
private:
    using Base = DynamicWaveletMatrix2DWeighted<
        X, Y, int, MAX_SIZE, Y_BIT_WIDTH, BLOCK_SIZE
    >;
    Base base_;

public:
    static_assert(std::is_integral_v<X> && std::is_integral_v<Y>);

    DynamicWaveletMatrix2D() = default;
    DynamicWaveletMatrix2D(const std::vector<X>& xs, const std::vector<Y>& ys)
        : base_(xs, ys, std::vector<int>(xs.size(), 0)){}
    explicit DynamicWaveletMatrix2D(const std::vector<std::pair<X, Y>>& points){
        std::vector<std::tuple<X, Y, int>> weighted_points;
        weighted_points.reserve(points.size());
        for(const auto& [x_value, y_value] : points){
            weighted_points.emplace_back(x_value, y_value, 0);
        }
        base_ = Base(weighted_points);
    }

    DynamicWaveletMatrix2D(const DynamicWaveletMatrix2D&) = delete;
    DynamicWaveletMatrix2D& operator=(const DynamicWaveletMatrix2D&) = delete;
    DynamicWaveletMatrix2D(DynamicWaveletMatrix2D&&) noexcept = default;
    DynamicWaveletMatrix2D& operator=(DynamicWaveletMatrix2D&&) noexcept = default;

    int size() const{ return base_.size(); }
    bool empty() const{ return base_.empty(); }
    X x(int index) const{ return base_.x(index); }
    Y y(int index) const{ return base_.y(index); }

    void insert(int index, X x_value, Y y_value){
        base_.insert(index, x_value, y_value, 0);
    }
    void push_back(X x_value, Y y_value){ base_.push_back(x_value, y_value, 0); }
    std::pair<X, Y> erase(int index){
        const auto [x_value, y_value, ignored] = base_.erase(index);
        (void)ignored;
        return {x_value, y_value};
    }
    std::pair<X, Y> pop_back(){
        const auto [x_value, y_value, ignored] = base_.pop_back();
        (void)ignored;
        return {x_value, y_value};
    }

    void set_y(int index, Y value){ base_.set_y(index, value); }
    void set_x(int index, X value){ base_.set_x(index, value); }
    void set(int index, X x_value, Y y_value){
        base_.set_point(index, x_value, y_value, 0);
    }

    int rectangle_count(X xl, X xr, Y yl, Y yr) const{
        return base_.rectangle_count(xl, xr, yl, yr);
    }
    int range_freq(X xl, X xr, Y yl, Y yr) const{
        return rectangle_count(xl, xr, yl, yr);
    }
    Y kth_smallest_y(X xl, X xr, int k) const{
        return base_.kth_smallest_y(xl, xr, k);
    }
    std::optional<Y> prev_y(X xl, X xr, Y upper) const{
        return base_.prev_y(xl, xr, upper);
    }
    std::optional<Y> next_y(X xl, X xr, Y lower) const{
        return base_.next_y(xl, xr, lower);
    }
};
