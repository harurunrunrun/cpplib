#ifndef CPPLIB_SRC_STRUCTURE_RANGE_QUERY_VALUE_QUERY_RANGE_TREE_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_RANGE_QUERY_VALUE_QUERY_RANGE_TREE_HPP_INCLUDED

#include <algorithm>
#include <stdexcept>
#include <iterator>
#include <utility>
#include <vector>

template<class Coordinate, class Payload = int>
class RangeTree{
public:
    struct Point{ Coordinate x, y; Payload payload; };

private:
    std::vector<Point> points_;
    std::vector<std::vector<std::pair<Coordinate, int>>> tree_;

    void build(int node, int left, int right){
        if(right - left == 1){
            tree_[node] = {{points_[left].y, left}};
            return;
        }
        const int middle = left + (right - left) / 2;
        build(node * 2, left, middle);
        build(node * 2 + 1, middle, right);
        std::merge(tree_[node * 2].begin(), tree_[node * 2].end(),
                   tree_[node * 2 + 1].begin(), tree_[node * 2 + 1].end(),
                   std::back_inserter(tree_[node]));
    }
    void report(int node, int left, int right, int query_left, int query_right,
                const Coordinate& lower_y, const Coordinate& upper_y,
                std::vector<Payload>& result) const {
        if(query_right <= left || right <= query_left) return;
        if(query_left <= left && right <= query_right){
            const auto begin = std::lower_bound(tree_[node].begin(), tree_[node].end(),
                                                std::pair<Coordinate, int>{lower_y, -1});
            const auto end = std::lower_bound(tree_[node].begin(), tree_[node].end(),
                                              std::pair<Coordinate, int>{upper_y, -1});
            for(auto it = begin; it != end; ++it) result.push_back(points_[it->second].payload);
            return;
        }
        const int middle = left + (right - left) / 2;
        report(node * 2, left, middle, query_left, query_right, lower_y, upper_y, result);
        report(node * 2 + 1, middle, right, query_left, query_right, lower_y, upper_y, result);
    }

public:
    RangeTree() = default;
    explicit RangeTree(std::vector<Point> points): points_(std::move(points)){
        std::sort(points_.begin(), points_.end(), [](const Point& a, const Point& b){
            if(a.x != b.x) return a.x < b.x;
            return a.y < b.y;
        });
        if(!points_.empty()){
            tree_.resize(points_.size() * 4);
            build(1, 0, static_cast<int>(points_.size()));
        }
    }
    int size() const noexcept { return static_cast<int>(points_.size()); }
    std::vector<Payload> report(const Coordinate& lower_x, const Coordinate& upper_x,
                                const Coordinate& lower_y, const Coordinate& upper_y) const {
        if(upper_x < lower_x || upper_y < lower_y) throw std::invalid_argument("RangeTree: invalid rectangle");
        std::vector<Payload> result;
        if(points_.empty()) return result;
        const int left = static_cast<int>(std::lower_bound(points_.begin(), points_.end(), lower_x,
            [](const Point& point, const Coordinate& x){ return point.x < x; }) - points_.begin());
        const int right = static_cast<int>(std::lower_bound(points_.begin(), points_.end(), upper_x,
            [](const Point& point, const Coordinate& x){ return point.x < x; }) - points_.begin());
        report(1, 0, size(), left, right, lower_y, upper_y, result);
        return result;
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_RANGE_QUERY_VALUE_QUERY_RANGE_TREE_HPP_INCLUDED
