#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINT_SET_DIAMETER_3D_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINT_SET_DIAMETER_3D_HPP_INCLUDED

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

struct PointSetDiameter3DResult{
    std::size_t first;
    std::size_t second;
    long double distance;
};

namespace point_set_diameter_3d_detail{

inline long double coordinate(const Point3& point, std::size_t axis){
    if(axis == 0) return point.x;
    if(axis == 1) return point.y;
    return point.z;
}

inline void validate(const std::vector<Point3>& points){
    for(const Point3& point: points){
        if(!std::isfinite(point.x) || !std::isfinite(point.y) ||
           !std::isfinite(point.z))[[unlikely]]{
            throw std::invalid_argument(
                "point_set_diameter_3d requires finite points"
            );
        }
    }
}

inline long double distance(const Point3& left, const Point3& right){
    return std::hypot(
        left.x - right.x, left.y - right.y, left.z - right.z
    );
}

inline void improve(
    PointSetDiameter3DResult& result,
    std::size_t first,
    std::size_t second,
    const std::vector<Point3>& points
){
    if(second < first) std::swap(first, second);
    if(first == second) return;
    const long double current_distance = distance(points[first], points[second]);
    if(current_distance > result.distance ||
       (current_distance == result.distance &&
        std::pair{first, second} < std::pair{result.first, result.second})){
        result = {first, second, current_distance};
    }
}

struct AabbNode{
    static constexpr std::size_t no_child =
        std::numeric_limits<std::size_t>::max();

    Point3 minimum{};
    Point3 maximum{};
    std::size_t begin = 0;
    std::size_t end = 0;
    std::size_t left = no_child;
    std::size_t right = no_child;

    [[nodiscard]] bool leaf() const{
        return left == no_child;
    }

    [[nodiscard]] std::size_t size() const{
        return end - begin;
    }
};

class AabbBranchAndBound{
    static constexpr std::size_t leaf_size = 12;

    const std::vector<Point3>& points;
    std::vector<std::size_t> order;
    std::vector<AabbNode> nodes;
    PointSetDiameter3DResult result;

    std::size_t build(std::size_t begin, std::size_t end){
        AabbNode node;
        node.begin = begin;
        node.end = end;
        node.minimum = points[order[begin]];
        node.maximum = points[order[begin]];
        for(std::size_t position = begin + 1; position < end; ++position){
            const Point3& point = points[order[position]];
            node.minimum.x = std::min(node.minimum.x, point.x);
            node.minimum.y = std::min(node.minimum.y, point.y);
            node.minimum.z = std::min(node.minimum.z, point.z);
            node.maximum.x = std::max(node.maximum.x, point.x);
            node.maximum.y = std::max(node.maximum.y, point.y);
            node.maximum.z = std::max(node.maximum.z, point.z);
        }
        const std::size_t node_index = nodes.size();
        nodes.push_back(node);
        if(end - begin <= leaf_size) return node_index;

        std::size_t split_axis = 0;
        for(std::size_t axis = 1; axis < 3; ++axis){
            if(coordinate(node.maximum, axis) - coordinate(node.minimum, axis) >
               coordinate(node.maximum, split_axis) -
                   coordinate(node.minimum, split_axis)){
                split_axis = axis;
            }
        }
        const std::size_t middle = begin + (end - begin) / 2;
        std::nth_element(
            order.begin() + static_cast<std::ptrdiff_t>(begin),
            order.begin() + static_cast<std::ptrdiff_t>(middle),
            order.begin() + static_cast<std::ptrdiff_t>(end),
            [&](std::size_t lhs, std::size_t rhs){
                const long double left_value = coordinate(points[lhs], split_axis);
                const long double right_value = coordinate(points[rhs], split_axis);
                if(left_value != right_value) return left_value < right_value;
                return lhs < rhs;
            }
        );
        const std::size_t left = build(begin, middle);
        const std::size_t right = build(middle, end);
        nodes[node_index].left = left;
        nodes[node_index].right = right;
        return node_index;
    }

    [[nodiscard]] long double upper_bound(
        std::size_t first,
        std::size_t second
    ) const{
        const AabbNode& lhs = nodes[first];
        const AabbNode& rhs = nodes[second];
        std::array<long double, 3> delta{};
        for(std::size_t axis = 0; axis < 3; ++axis){
            delta[axis] = std::max(
                std::abs(
                    coordinate(lhs.minimum, axis) -
                    coordinate(rhs.maximum, axis)
                ),
                std::abs(
                    coordinate(lhs.maximum, axis) -
                    coordinate(rhs.minimum, axis)
                )
            );
        }
        long double bound = std::hypot(delta[0], delta[1], delta[2]);
        for(int iteration = 0;
            iteration < 16 && std::isfinite(bound); ++iteration){
            bound = std::nextafter(
                bound, std::numeric_limits<long double>::infinity()
            );
        }
        return bound;
    }

    void compare_leaves(std::size_t first, std::size_t second){
        const AabbNode& lhs = nodes[first];
        const AabbNode& rhs = nodes[second];
        if(first == second){
            for(std::size_t left = lhs.begin; left < lhs.end; ++left){
                for(std::size_t right = left + 1; right < lhs.end; ++right){
                    improve(result, order[left], order[right], points);
                }
            }
            return;
        }
        for(std::size_t left = lhs.begin; left < lhs.end; ++left){
            for(std::size_t right = rhs.begin; right < rhs.end; ++right){
                improve(result, order[left], order[right], points);
            }
        }
    }

    void search(std::size_t first, std::size_t second){
        if(upper_bound(first, second) < result.distance) return;
        const AabbNode& lhs = nodes[first];
        const AabbNode& rhs = nodes[second];
        if(lhs.leaf() && rhs.leaf()){
            compare_leaves(first, second);
            return;
        }

        struct ChildPair{
            std::size_t first;
            std::size_t second;
            long double bound;
        };
        std::array<ChildPair, 3> children{};
        std::size_t child_count = 0;
        const auto add = [&](
            std::size_t child_first,
            std::size_t child_second
        ){
            children[child_count++] = {
                child_first,
                child_second,
                upper_bound(child_first, child_second),
            };
        };
        if(first == second){
            add(lhs.left, lhs.left);
            add(lhs.left, lhs.right);
            add(lhs.right, lhs.right);
        }else if(!lhs.leaf() && (rhs.leaf() || lhs.size() >= rhs.size())){
            add(lhs.left, second);
            add(lhs.right, second);
        }else{
            add(first, rhs.left);
            add(first, rhs.right);
        }
        for(std::size_t index = 1; index < child_count; ++index){
            const ChildPair value = children[index];
            std::size_t position = index;
            while(position != 0 &&
                  value.bound > children[position - 1].bound){
                children[position] = children[position - 1];
                --position;
            }
            children[position] = value;
        }
        for(std::size_t index = 0; index < child_count; ++index){
            if(children[index].bound < result.distance) continue;
            search(children[index].first, children[index].second);
        }
    }

public:
    explicit AabbBranchAndBound(const std::vector<Point3>& points_):
        points(points_), order(points.size()), result{
            0, 1, distance(points[0], points[1])
        }
    {
        std::iota(order.begin(), order.end(), std::size_t{0});
        nodes.reserve(2 * points.size() / leaf_size + 1);
        const std::size_t root = build(0, points.size());
        for(std::size_t axis = 0; axis < 3; ++axis){
            std::size_t minimum_index = 0;
            std::size_t maximum_index = 0;
            for(std::size_t index = 1; index < points.size(); ++index){
                if(coordinate(points[index], axis) <
                   coordinate(points[minimum_index], axis)){
                    minimum_index = index;
                }
                if(coordinate(points[index], axis) >
                   coordinate(points[maximum_index], axis)){
                    maximum_index = index;
                }
            }
            improve(result, minimum_index, maximum_index, points);
        }
        search(root, root);
    }

    [[nodiscard]] PointSetDiameter3DResult get_result() const{
        return result;
    }
};

}  // namespace point_set_diameter_3d_detail

inline std::optional<PointSetDiameter3DResult> point_set_diameter_3d(
    const std::vector<Point3>& points
){
    point_set_diameter_3d_detail::validate(points);
    if(points.size() < 2) return std::nullopt;
    PointSetDiameter3DResult result{
        0,
        1,
        point_set_diameter_3d_detail::distance(points[0], points[1]),
    };
    for(std::size_t first = 0; first < points.size(); ++first){
        for(std::size_t second = first + 1; second < points.size(); ++second){
            point_set_diameter_3d_detail::improve(
                result, first, second, points
            );
        }
    }
    return result;
}

inline std::optional<PointSetDiameter3DResult>
point_set_diameter_3d_aabb_branch_and_bound(
    const std::vector<Point3>& points
){
    point_set_diameter_3d_detail::validate(points);
    if(points.size() < 2) return std::nullopt;
    return point_set_diameter_3d_detail::AabbBranchAndBound(points).get_result();
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINT_SET_DIAMETER_3D_HPP_INCLUDED
