#pragma once

#include <cstddef>
#include <cstdint>
#include <limits>
#include <memory>
#include <optional>
#include <stdexcept>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>
#include "../tree/implicit_treap.hpp"
#include "../tree/treap.hpp"
#include "dynamic_weighted_wavelet_matrix.hpp"

template<class X, class Y, class W, int MAX_SIZE,
         int Y_BIT_WIDTH = std::numeric_limits<std::make_unsigned_t<Y>>::digits,
         int BLOCK_SIZE = 512>
struct DynamicWaveletMatrix2DWeighted{
    static_assert(std::is_integral_v<X> && std::is_integral_v<Y>);
    static_assert(MAX_SIZE >= 0 && BLOCK_SIZE > 0);

private:
    static constexpr int STORAGE_SIZE = MAX_SIZE == 0 ? 1 : MAX_SIZE;
    struct Point{ X x{}; Y y{}; W weight{}; };
    struct IdMonoid{
        using S = std::uint64_t;
        static S op(S, S){ return 0; }
        static S e(){ return 0; }
    };
    static constexpr IdMonoid id_monoid{};
    using Sequence = ImplicitTreap<id_monoid, STORAGE_SIZE>;
    using XOrder = TreapSet<std::pair<X, std::uint64_t>, STORAGE_SIZE>;
    using Matrix = DynamicWeightedWaveletMatrix<Y, W, MAX_SIZE, Y_BIT_WIDTH, BLOCK_SIZE>;

    int _n = 0;
    std::uint64_t next_id_ = 1;
    mutable Sequence sequence_;
    XOrder x_order_;
    std::unordered_map<std::uint64_t, std::unique_ptr<Point>> points_;
    std::unique_ptr<Matrix> matrix_;

    void check_index(int index, const char* message) const{
        if(index < 0 || _n <= index)[[unlikely]] throw std::runtime_error(message);
    }
    void check_insert_index(int index, const char* message) const{
        if(index < 0 || _n < index || _n == MAX_SIZE)[[unlikely]] throw std::runtime_error(message);
    }
    static void check_rect(X xl, X xr, Y yl, Y yr, const char* message){
        if(xr < xl || yr < yl)[[unlikely]] throw std::runtime_error(message);
    }
    std::uint64_t id_at(int index) const{
        check_index(index, "library assertion fault: range violation (point access).");
        return sequence_.get(index);
    }
    Point* point_of(std::uint64_t id) const{ return points_.find(id)->second.get(); }
    Point* point_at(int index) const{ return point_of(id_at(index)); }
    int sorted_index(X x_value, std::uint64_t id) const{
        return x_order_.order_of_key({x_value, id});
    }
    std::pair<int, int> x_range(X xl, X xr) const{
        return {x_order_.order_of_key({xl, 0}), x_order_.order_of_key({xr, 0})};
    }
    void initialize_empty_matrix(){
        matrix_ = std::make_unique<Matrix>(std::vector<Y>{}, std::vector<W>{});
    }
    void build(const std::vector<X>& xs, const std::vector<Y>& ys,
               const std::vector<W>& weights){
        if(xs.size() != ys.size() || xs.size() != weights.size() ||
           xs.size() > static_cast<std::size_t>(MAX_SIZE))[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (constructor).");
        }
        points_.reserve(xs.size());
        for(std::size_t index = 0; index < xs.size(); ++index){
            const std::uint64_t id = next_id_++;
            points_.emplace(id, std::make_unique<Point>(Point{xs[index], ys[index], weights[index]}));
            sequence_.push_back(id);
            x_order_.insert({xs[index], id});
        }
        std::vector<Y> sorted_y;
        std::vector<W> sorted_weights;
        sorted_y.reserve(xs.size());
        sorted_weights.reserve(xs.size());
        for(int index = 0; index < static_cast<int>(xs.size()); ++index){
            const auto key = x_order_.kth(index);
            Point* point = point_of(key->second);
            sorted_y.push_back(point->y);
            sorted_weights.push_back(point->weight);
        }
        matrix_ = std::make_unique<Matrix>(sorted_y, sorted_weights);
        _n = static_cast<int>(xs.size());
    }

public:
    DynamicWaveletMatrix2DWeighted(){ initialize_empty_matrix(); }
    DynamicWaveletMatrix2DWeighted(const std::vector<X>& xs, const std::vector<Y>& ys,
                                   const std::vector<W>& weights){
        build(xs, ys, weights);
    }
    explicit DynamicWaveletMatrix2DWeighted(const std::vector<std::tuple<X, Y, W>>& points){
        std::vector<X> xs(points.size());
        std::vector<Y> ys(points.size());
        std::vector<W> weights(points.size());
        for(std::size_t i = 0; i < points.size(); ++i){
            xs[i] = std::get<0>(points[i]);
            ys[i] = std::get<1>(points[i]);
            weights[i] = std::get<2>(points[i]);
        }
        build(xs, ys, weights);
    }

    DynamicWaveletMatrix2DWeighted(const DynamicWaveletMatrix2DWeighted&) = delete;
    DynamicWaveletMatrix2DWeighted& operator=(const DynamicWaveletMatrix2DWeighted&) = delete;
    DynamicWaveletMatrix2DWeighted(DynamicWaveletMatrix2DWeighted&&) noexcept = default;
    DynamicWaveletMatrix2DWeighted& operator=(DynamicWaveletMatrix2DWeighted&&) noexcept = default;

    int size() const{ return _n; }
    bool empty() const{ return _n == 0; }
    X x(int index) const{ return point_at(index)->x; }
    Y y(int index) const{ return point_at(index)->y; }
    W weight(int index) const{ return point_at(index)->weight; }

    void insert(int index, X x_value, Y y_value, W weight_value){
        check_insert_index(index, "library assertion fault: range violation (insert).");
        const std::uint64_t id = next_id_++;
        const int matrix_index = sorted_index(x_value, id);
        matrix_->insert(matrix_index, y_value, weight_value);
        points_.emplace(id, std::make_unique<Point>(Point{x_value, y_value, weight_value}));
        sequence_.insert(index, id);
        x_order_.insert({x_value, id});
        ++_n;
    }
    void push_back(X x_value, Y y_value, W weight_value){
        insert(_n, x_value, y_value, weight_value);
    }
    std::tuple<X, Y, W> erase(int index){
        const std::uint64_t id = id_at(index);
        Point* point = point_of(id);
        const std::tuple<X, Y, W> result{point->x, point->y, point->weight};
        matrix_->erase(sorted_index(point->x, id));
        x_order_.erase({point->x, id});
        sequence_.erase(index);
        points_.erase(id);
        --_n;
        return result;
    }
    std::tuple<X, Y, W> pop_back(){ return erase(_n - 1); }

    void set(int index, Y y_value, W weight_value){
        const std::uint64_t id = id_at(index);
        Point* point = point_of(id);
        matrix_->set(sorted_index(point->x, id), y_value, weight_value);
        point->y = y_value;
        point->weight = weight_value;
    }
    void set_y(int index, Y value){ set(index, value, weight(index)); }
    void set_weight(int index, W value){ set(index, y(index), value); }
    void set_x(int index, X value){
        const std::uint64_t id = id_at(index);
        Point* point = point_of(id);
        if(point->x == value) return;
        matrix_->erase(sorted_index(point->x, id));
        x_order_.erase({point->x, id});
        point->x = value;
        matrix_->insert(sorted_index(point->x, id), point->y, point->weight);
        x_order_.insert({point->x, id});
    }
    void set_point(int index, X x_value, Y y_value, W weight_value){
        set_x(index, x_value);
        set(index, y_value, weight_value);
    }

    int rectangle_count(X xl, X xr, Y yl, Y yr) const{
        check_rect(xl, xr, yl, yr, "library assertion fault: range violation (rectangle_count).");
        const auto [left, right] = x_range(xl, xr);
        return matrix_->range_freq(left, right, yl, yr);
    }
    W rectangle_sum(X xl, X xr, Y yl, Y yr) const{
        check_rect(xl, xr, yl, yr, "library assertion fault: range violation (rectangle_sum).");
        const auto [left, right] = x_range(xl, xr);
        return matrix_->range_sum(left, right, yl, yr);
    }
    int range_freq(X xl, X xr, Y yl, Y yr) const{
        return rectangle_count(xl, xr, yl, yr);
    }
    W range_sum(X xl, X xr, Y yl, Y yr) const{
        return rectangle_sum(xl, xr, yl, yr);
    }
    Y kth_smallest_y(X xl, X xr, int k) const{
        if(xr < xl)[[unlikely]] throw std::runtime_error(
            "library assertion fault: range violation (kth_smallest_y).");
        const auto [left, right] = x_range(xl, xr);
        return matrix_->kth_smallest(left, right, k);
    }
    std::optional<Y> prev_y(X xl, X xr, Y upper) const{
        if(xr < xl)[[unlikely]] throw std::runtime_error(
            "library assertion fault: range violation (prev_y).");
        const auto [left, right] = x_range(xl, xr);
        return matrix_->prev_value(left, right, upper);
    }
    std::optional<Y> next_y(X xl, X xr, Y lower) const{
        if(xr < xl)[[unlikely]] throw std::runtime_error(
            "library assertion fault: range violation (next_y).");
        const auto [left, right] = x_range(xl, xr);
        return matrix_->next_value(left, right, lower);
    }
};
