#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_OCTREE_3D_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_OCTREE_3D_HPP_INCLUDED

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <functional>
#include <limits>
#include <optional>
#include <queue>
#include <stdexcept>
#include <utility>
#include <vector>

#include "aabb3.hpp"
#include "base.hpp"
#include "is_finite.hpp"

class Octree3D{
    struct Node{
        AABB3 box{};
        std::array<int, 8> children{{-1, -1, -1, -1, -1, -1, -1, -1}};
        std::vector<std::size_t> point_indices;
        Node() = default;
        explicit Node(const AABB3& box_): box(box_){}

        bool leaf() const noexcept{ return children[0] < 0; }
    };

    AABB3 bounds_{};
    std::size_t leaf_capacity_ = 8;
    std::size_t max_depth_ = 32;
    std::vector<Point3> points_;
    std::vector<Node> nodes_;

    static long double point_distance(const Point3& left, const Point3& right){
        return std::hypot(left.x - right.x, left.y - right.y, left.z - right.z);
    }

    static Point3 midpoint(const AABB3& box){
        return {
            box.minimum.x / 2 + box.maximum.x / 2,
            box.minimum.y / 2 + box.maximum.y / 2,
            box.minimum.z / 2 + box.maximum.z / 2,
        };
    }

    static std::size_t child_number(const AABB3& box, const Point3& point){
        const Point3 middle = midpoint(box);
        return (point.x >= middle.x ? std::size_t{1} : 0)
            | (point.y >= middle.y ? std::size_t{2} : 0)
            | (point.z >= middle.z ? std::size_t{4} : 0);
    }

    bool can_subdivide(const AABB3& box) const{
        const Point3 middle = midpoint(box);
        return (box.minimum.x < middle.x && middle.x < box.maximum.x)
            || (box.minimum.y < middle.y && middle.y < box.maximum.y)
            || (box.minimum.z < middle.z && middle.z < box.maximum.z);
    }

    void subdivide(std::size_t node_index){
        const AABB3 parent_box = nodes_[node_index].box;
        const Point3 middle = midpoint(parent_box);
        std::array<int, 8> children{};
        for(std::size_t child = 0; child < 8; ++child){
            Point3 minimum = parent_box.minimum;
            Point3 maximum = parent_box.maximum;
            if(child & 1U) minimum.x = middle.x;
            else maximum.x = middle.x;
            if(child & 2U) minimum.y = middle.y;
            else maximum.y = middle.y;
            if(child & 4U) minimum.z = middle.z;
            else maximum.z = middle.z;
            children[child] = static_cast<int>(nodes_.size());
            nodes_.push_back(Node{AABB3{minimum, maximum}});
        }
        nodes_[node_index].children = children;
    }

    void insert_node(std::size_t node_index, std::size_t point_index, std::size_t depth){
        if(nodes_[node_index].leaf()){
            if(nodes_[node_index].point_indices.size() < leaf_capacity_
               || depth >= max_depth_ || !can_subdivide(nodes_[node_index].box)){
                nodes_[node_index].point_indices.push_back(point_index);
                return;
            }
            std::vector<std::size_t> previous;
            previous.swap(nodes_[node_index].point_indices);
            subdivide(node_index);
            for(const std::size_t old_index: previous){
                const std::size_t child = child_number(nodes_[node_index].box, points_[old_index]);
                insert_node(
                    static_cast<std::size_t>(nodes_[node_index].children[child]),
                    old_index,
                    depth + 1
                );
            }
        }
        const std::size_t child = child_number(nodes_[node_index].box, points_[point_index]);
        insert_node(
            static_cast<std::size_t>(nodes_[node_index].children[child]),
            point_index,
            depth + 1
        );
    }

    void range_search(
        std::size_t node_index,
        const AABB3& range,
        std::vector<std::size_t>& result
    ) const{
        const Node& node = nodes_[node_index];
        if(!node.box.intersects(range)) return;
        if(node.leaf()){
            for(const std::size_t point_index: node.point_indices){
                if(range.contains(points_[point_index])) result.push_back(point_index);
            }
            return;
        }
        for(const int child: node.children){
            range_search(static_cast<std::size_t>(child), range, result);
        }
    }

public:
    explicit Octree3D(
        const AABB3& bounds,
        std::size_t leaf_capacity = 8,
        std::size_t max_depth = 32
    ): bounds_(bounds), leaf_capacity_(leaf_capacity), max_depth_(max_depth){
        bounds_.require_valid();
        if(leaf_capacity_ == 0){
            throw std::invalid_argument("Octree3D leaf capacity must be positive");
        }
        if(max_depth_ > 64){
            throw std::invalid_argument("Octree3D maximum depth must be at most 64");
        }
        nodes_.push_back(Node{bounds_});
    }

    std::size_t insert(const Point3& point){
        if(!geometry3d_is_finite(point) || !bounds_.contains(point)){
            throw std::invalid_argument("Octree3D point must be finite and inside bounds");
        }
        const std::size_t index = points_.size();
        points_.push_back(point);
        insert_node(0, index, 0);
        return index;
    }

    std::size_t size() const noexcept{ return points_.size(); }
    bool empty() const noexcept{ return points_.empty(); }
    const AABB3& bounds() const noexcept{ return bounds_; }
    const Point3& point(std::size_t index) const{ return points_.at(index); }

    std::vector<std::size_t> range_query(const AABB3& range) const{
        range.require_valid();
        std::vector<std::size_t> result;
        range_search(0, range, result);
        std::sort(result.begin(), result.end());
        return result;
    }

    std::optional<std::pair<std::size_t, long double>> nearest(
        const Point3& query
    ) const{
        if(!geometry3d_is_finite(query)){
            throw std::invalid_argument("Octree3D query must be finite");
        }
        if(empty()) return std::nullopt;
        using QueueEntry = std::pair<long double, std::size_t>;
        std::priority_queue<QueueEntry, std::vector<QueueEntry>, std::greater<QueueEntry>> queue;
        queue.push({bounds_.distance(query), 0});
        long double best_distance = std::numeric_limits<long double>::infinity();
        std::size_t best_index = points_.size();
        while(!queue.empty()){
            const auto [lower_bound, node_index] = queue.top();
            queue.pop();
            if(lower_bound > best_distance) break;
            const Node& node = nodes_[node_index];
            if(node.leaf()){
                for(const std::size_t point_index: node.point_indices){
                    const long double current = point_distance(query, points_[point_index]);
                    if(current < best_distance
                       || (current == best_distance && point_index < best_index)){
                        best_distance = current;
                        best_index = point_index;
                    }
                }
            }else{
                for(const int child: node.children){
                    const std::size_t child_index = static_cast<std::size_t>(child);
                    const long double lower = nodes_[child_index].box.distance(query);
                    if(lower <= best_distance) queue.push({lower, child_index});
                }
            }
        }
        return std::pair{best_index, best_distance};
    }
};

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_OCTREE_3D_HPP_INCLUDED
