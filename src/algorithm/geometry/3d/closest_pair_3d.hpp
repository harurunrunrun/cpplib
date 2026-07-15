#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <limits>
#include <numeric>
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>

#include "base.hpp"

struct ClosestPair3DResult{
    std::size_t first;
    std::size_t second;
    long double distance;
};

namespace closest_pair_3d_detail{

inline long double coordinate(const Point3& point, std::size_t axis){
    if(axis == 0) return point.x;
    if(axis == 1) return point.y;
    return point.z;
}

inline bool finite(const Point3& point){
    return std::isfinite(point.x) && std::isfinite(point.y) &&
        std::isfinite(point.z);
}

inline long double point_distance(const Point3& left, const Point3& right){
    return std::hypot(
        left.x - right.x, left.y - right.y, left.z - right.z
    );
}

struct Node{
    std::size_t point_index = 0;
    int left = -1;
    int right = -1;
    Point3 minimum{};
    Point3 maximum{};
};

class Tree{
    const std::vector<Point3>& points;
    std::vector<std::size_t> order;
    std::vector<Node> nodes;

    int build(std::size_t begin, std::size_t end){
        if(begin == end) return -1;
        std::array<long double, 3> minimum{{
            std::numeric_limits<long double>::infinity(),
            std::numeric_limits<long double>::infinity(),
            std::numeric_limits<long double>::infinity(),
        }};
        std::array<long double, 3> maximum{{
            -std::numeric_limits<long double>::infinity(),
            -std::numeric_limits<long double>::infinity(),
            -std::numeric_limits<long double>::infinity(),
        }};
        for(std::size_t position = begin; position < end; ++position){
            for(std::size_t axis = 0; axis < 3; ++axis){
                const long double value = coordinate(points[order[position]], axis);
                minimum[axis] = std::min(minimum[axis], value);
                maximum[axis] = std::max(maximum[axis], value);
            }
        }
        std::size_t split_axis = 0;
        for(std::size_t axis = 1; axis < 3; ++axis){
            if(maximum[axis] - minimum[axis] >
               maximum[split_axis] - minimum[split_axis]){
                split_axis = axis;
            }
        }
        const std::size_t middle = begin + (end - begin) / 2;
        std::nth_element(
            order.begin() + static_cast<std::ptrdiff_t>(begin),
            order.begin() + static_cast<std::ptrdiff_t>(middle),
            order.begin() + static_cast<std::ptrdiff_t>(end),
            [&](std::size_t left, std::size_t right){
                const long double left_value = coordinate(points[left], split_axis);
                const long double right_value = coordinate(points[right], split_axis);
                if(left_value != right_value) return left_value < right_value;
                return left < right;
            }
        );
        const int node_index = static_cast<int>(nodes.size());
        nodes.push_back(Node{});
        const int left = build(begin, middle);
        const int right = build(middle + 1, end);
        Node& node = nodes[static_cast<std::size_t>(node_index)];
        node.point_index = order[middle];
        node.left = left;
        node.right = right;
        node.minimum = {
            minimum[0], minimum[1], minimum[2],
        };
        node.maximum = {
            maximum[0], maximum[1], maximum[2],
        };
        return node_index;
    }

    long double box_distance(int node_index, const Point3& point) const{
        if(node_index < 0) return std::numeric_limits<long double>::infinity();
        const Node& node = nodes[static_cast<std::size_t>(node_index)];
        std::array<long double, 3> delta{};
        for(std::size_t axis = 0; axis < 3; ++axis){
            const long double value = coordinate(point, axis);
            const long double low = coordinate(node.minimum, axis);
            const long double high = coordinate(node.maximum, axis);
            if(value < low) delta[axis] = low - value;
            else if(high < value) delta[axis] = value - high;
        }
        return std::hypot(delta[0], delta[1], delta[2]);
    }

    void query(
        int node_index,
        std::size_t query_index,
        long double& best_distance,
        std::size_t& best_index
    ) const{
        if(node_index < 0) return;
        const long double lower_bound = box_distance(
            node_index, points[query_index]
        );
        const long double tolerance = 32 * std::numeric_limits<long double>::epsilon() *
            std::max({1.0L, lower_bound, best_distance});
        if(lower_bound > best_distance + tolerance) return;

        const Node& node = nodes[static_cast<std::size_t>(node_index)];
        if(node.point_index != query_index){
            const long double current_distance = point_distance(
                points[query_index], points[node.point_index]
            );
            if(current_distance < best_distance ||
               (current_distance == best_distance && node.point_index < best_index)){
                best_distance = current_distance;
                best_index = node.point_index;
            }
        }
        int first_child = node.left;
        int second_child = node.right;
        if(box_distance(second_child, points[query_index]) <
           box_distance(first_child, points[query_index])){
            std::swap(first_child, second_child);
        }
        query(first_child, query_index, best_distance, best_index);
        query(second_child, query_index, best_distance, best_index);
    }

public:
    explicit Tree(const std::vector<Point3>& points_): points(points_){
        order.resize(points.size());
        std::iota(order.begin(), order.end(), std::size_t{0});
        nodes.reserve(points.size());
        build(0, points.size());
    }

    std::pair<std::size_t, long double> nearest(std::size_t query_index) const{
        std::size_t best_index = points.size();
        long double best_distance = std::numeric_limits<long double>::infinity();
        query(0, query_index, best_distance, best_index);
        return {best_index, best_distance};
    }
};

} // namespace closest_pair_3d_detail

inline std::optional<ClosestPair3DResult> closest_pair_3d(
    const std::vector<Point3>& points
){
    if(points.size() < 2) return std::nullopt;
    for(const Point3& point: points){
        if(!closest_pair_3d_detail::finite(point))[[unlikely]]{
            throw std::invalid_argument("closest_pair_3d requires finite points");
        }
    }

    std::vector<std::size_t> sorted(points.size());
    std::iota(sorted.begin(), sorted.end(), std::size_t{0});
    std::sort(sorted.begin(), sorted.end(), [&](std::size_t left, std::size_t right){
        if(points[left].x != points[right].x) return points[left].x < points[right].x;
        if(points[left].y != points[right].y) return points[left].y < points[right].y;
        if(points[left].z != points[right].z) return points[left].z < points[right].z;
        return left < right;
    });
    std::optional<std::pair<std::size_t, std::size_t>> duplicate;
    for(std::size_t index = 1; index < sorted.size(); ++index){
        const std::size_t left = sorted[index - 1];
        const std::size_t right = sorted[index];
        if(points[left].x == points[right].x &&
           points[left].y == points[right].y &&
           points[left].z == points[right].z){
            const std::pair<std::size_t, std::size_t> candidate = std::minmax(left, right);
            if(!duplicate || candidate < *duplicate) duplicate = candidate;
        }
    }
    if(duplicate){
        return ClosestPair3DResult{duplicate->first, duplicate->second, 0};
    }

    const closest_pair_3d_detail::Tree tree(points);
    ClosestPair3DResult result{
        0, 1, closest_pair_3d_detail::point_distance(points[0], points[1]),
    };
    for(std::size_t index = 0; index < points.size(); ++index){
        const auto [other, current_distance] = tree.nearest(index);
        if(other == points.size()) continue;
        const auto [first, second] = std::minmax(index, other);
        if(current_distance < result.distance ||
           (current_distance == result.distance &&
            std::pair{first, second} < std::pair{result.first, result.second})){
            result = {first, second, current_distance};
        }
    }
    return result;
}
