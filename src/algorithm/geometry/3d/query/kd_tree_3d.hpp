#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_QUERY_KD_TREE_3D_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_QUERY_KD_TREE_3D_HPP_INCLUDED

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <limits>
#include <numeric>
#include <optional>
#include <queue>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../core/aabb3.hpp"
#include "../core/geometry_primitives.hpp"
#include "../predicate/is_finite.hpp"

class KdTree3D{
    struct Node{
        std::size_t point_index = 0;
        int left = -1;
        int right = -1;
        AABB3 box{};
    };

    std::vector<Point3> points_;
    std::vector<std::size_t> order_;
    std::vector<Node> nodes_;
    int root_ = -1;

    static long double coordinate(const Point3& point, std::size_t axis){
        if(axis == 0) return point.x;
        if(axis == 1) return point.y;
        return point.z;
    }

    static long double point_distance(const Point3& left, const Point3& right){
        return std::hypot(left.x - right.x, left.y - right.y, left.z - right.z);
    }

    int build_node(std::size_t begin, std::size_t end){
        if(begin == end) return -1;
        Point3 minimum{
            std::numeric_limits<long double>::infinity(),
            std::numeric_limits<long double>::infinity(),
            std::numeric_limits<long double>::infinity(),
        };
        Point3 maximum{-minimum.x, -minimum.y, -minimum.z};
        for(std::size_t position = begin; position < end; ++position){
            const Point3& point = points_[order_[position]];
            minimum.x = std::min(minimum.x, point.x);
            minimum.y = std::min(minimum.y, point.y);
            minimum.z = std::min(minimum.z, point.z);
            maximum.x = std::max(maximum.x, point.x);
            maximum.y = std::max(maximum.y, point.y);
            maximum.z = std::max(maximum.z, point.z);
        }
        const std::array<long double, 3> spread{{
            maximum.x - minimum.x,
            maximum.y - minimum.y,
            maximum.z - minimum.z,
        }};
        std::size_t axis = 0;
        if(spread[1] > spread[axis]) axis = 1;
        if(spread[2] > spread[axis]) axis = 2;
        const std::size_t middle = begin + (end - begin) / 2;
        std::nth_element(
            order_.begin() + static_cast<std::ptrdiff_t>(begin),
            order_.begin() + static_cast<std::ptrdiff_t>(middle),
            order_.begin() + static_cast<std::ptrdiff_t>(end),
            [&](std::size_t left, std::size_t right){
                const long double lhs = coordinate(points_[left], axis);
                const long double rhs = coordinate(points_[right], axis);
                return lhs != rhs ? lhs < rhs : left < right;
            }
        );
        const int node_index = static_cast<int>(nodes_.size());
        nodes_.push_back(Node{});
        const int left = build_node(begin, middle);
        const int right = build_node(middle + 1, end);
        Node& node = nodes_[static_cast<std::size_t>(node_index)];
        node.point_index = order_[middle];
        node.left = left;
        node.right = right;
        node.box = {minimum, maximum};
        return node_index;
    }

    static bool better(
        long double distance,
        std::size_t index,
        long double best_distance,
        std::size_t best_index
    ){
        return distance < best_distance
            || (distance == best_distance && index < best_index);
    }

    void nearest_search(
        int node_index,
        const Point3& query,
        long double& best_distance,
        std::size_t& best_index
    ) const{
        if(node_index < 0) return;
        const Node& node = nodes_[static_cast<std::size_t>(node_index)];
        if(node.box.distance(query) > best_distance) return;
        const long double current = point_distance(query, points_[node.point_index]);
        if(better(current, node.point_index, best_distance, best_index)){
            best_distance = current;
            best_index = node.point_index;
        }
        int first = node.left;
        int second = node.right;
        const long double left_distance = first < 0
            ? std::numeric_limits<long double>::infinity()
            : nodes_[static_cast<std::size_t>(first)].box.distance(query);
        const long double right_distance = second < 0
            ? std::numeric_limits<long double>::infinity()
            : nodes_[static_cast<std::size_t>(second)].box.distance(query);
        if(right_distance < left_distance) std::swap(first, second);
        nearest_search(first, query, best_distance, best_index);
        nearest_search(second, query, best_distance, best_index);
    }

    using HeapEntry = std::pair<long double, std::size_t>;

    void k_nearest_search(
        int node_index,
        const Point3& query,
        std::size_t count,
        std::priority_queue<HeapEntry>& heap
    ) const{
        if(node_index < 0) return;
        const Node& node = nodes_[static_cast<std::size_t>(node_index)];
        if(heap.size() == count && node.box.distance(query) > heap.top().first) return;
        const HeapEntry current{
            point_distance(query, points_[node.point_index]), node.point_index,
        };
        if(heap.size() < count) heap.push(current);
        else if(current < heap.top()){
            heap.pop();
            heap.push(current);
        }
        int first = node.left;
        int second = node.right;
        const long double left_distance = first < 0
            ? std::numeric_limits<long double>::infinity()
            : nodes_[static_cast<std::size_t>(first)].box.distance(query);
        const long double right_distance = second < 0
            ? std::numeric_limits<long double>::infinity()
            : nodes_[static_cast<std::size_t>(second)].box.distance(query);
        if(right_distance < left_distance) std::swap(first, second);
        k_nearest_search(first, query, count, heap);
        k_nearest_search(second, query, count, heap);
    }

    void radius_search(
        int node_index,
        const Point3& query,
        long double radius,
        std::vector<std::pair<std::size_t, long double>>& result
    ) const{
        if(node_index < 0) return;
        const Node& node = nodes_[static_cast<std::size_t>(node_index)];
        if(node.box.distance(query) > radius) return;
        const long double current = point_distance(query, points_[node.point_index]);
        if(current <= radius) result.push_back({node.point_index, current});
        radius_search(node.left, query, radius, result);
        radius_search(node.right, query, radius, result);
    }

    static void require_query(const Point3& query){
        if(!geometry3d_is_finite(query)){
            throw std::invalid_argument("KdTree3D query must be finite");
        }
    }

    static void sort_neighbors(
        std::vector<std::pair<std::size_t, long double>>& neighbors
    ){
        std::sort(neighbors.begin(), neighbors.end(), [](const auto& left, const auto& right){
            return left.second != right.second ? left.second < right.second
                : left.first < right.first;
        });
    }

public:
    KdTree3D() = default;

    explicit KdTree3D(const std::vector<Point3>& points){ build(points); }

    void build(const std::vector<Point3>& points){
        for(const Point3& point: points){
            if(!geometry3d_is_finite(point)){
                throw std::invalid_argument("KdTree3D points must be finite");
            }
        }
        points_ = points;
        order_.resize(points_.size());
        std::iota(order_.begin(), order_.end(), std::size_t{0});
        nodes_.clear();
        nodes_.reserve(points_.size());
        root_ = build_node(0, points_.size());
    }

    std::size_t size() const noexcept{ return points_.size(); }
    bool empty() const noexcept{ return points_.empty(); }
    const Point3& point(std::size_t index) const{ return points_.at(index); }

    std::optional<std::pair<std::size_t, long double>> nearest(
        const Point3& query
    ) const{
        require_query(query);
        if(empty()) return std::nullopt;
        long double best_distance = std::numeric_limits<long double>::infinity();
        std::size_t best_index = points_.size();
        nearest_search(root_, query, best_distance, best_index);
        return std::pair{best_index, best_distance};
    }

    std::vector<std::pair<std::size_t, long double>> k_nearest(
        const Point3& query,
        std::size_t count
    ) const{
        require_query(query);
        if(count == 0 || empty()) return {};
        count = std::min(count, size());
        std::priority_queue<HeapEntry> heap;
        k_nearest_search(root_, query, count, heap);
        std::vector<std::pair<std::size_t, long double>> result;
        result.reserve(heap.size());
        while(!heap.empty()){
            result.push_back({heap.top().second, heap.top().first});
            heap.pop();
        }
        sort_neighbors(result);
        return result;
    }

    std::vector<std::pair<std::size_t, long double>> radius_query(
        const Point3& query,
        long double radius
    ) const{
        require_query(query);
        if(!std::isfinite(radius) || radius < 0){
            throw std::invalid_argument("KdTree3D radius must be finite and nonnegative");
        }
        std::vector<std::pair<std::size_t, long double>> result;
        radius_search(root_, query, radius, result);
        sort_neighbors(result);
        return result;
    }
};

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_QUERY_KD_TREE_3D_HPP_INCLUDED
