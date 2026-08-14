#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_SCALAR_POINT_SET_POINT_SET_POLYGON_MINIMUM_DISTANCE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_SCALAR_POINT_SET_POINT_SET_POLYGON_MINIMUM_DISTANCE_HPP_INCLUDED

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <limits>
#include <queue>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../../integer/point_set_point/contains.hpp"
#include "../segment_segment/segment_segment_distance.hpp"

namespace polygon_minimum_distance_detail{

struct BoxedSegment{
    Segment segment;
    long double minimum_x;
    long double maximum_x;
    long double minimum_y;
    long double maximum_y;
};

struct Node{
    long double minimum_x = 0.0L;
    long double maximum_x = 0.0L;
    long double minimum_y = 0.0L;
    long double maximum_y = 0.0L;
    std::size_t begin = 0;
    std::size_t end = 0;
    int left = -1;
    int right = -1;

    bool leaf() const noexcept{ return left == -1; }
};

inline std::vector<BoxedSegment> polygon_segments(
    const std::vector<Point>& polygon
){
    if(polygon.empty())[[unlikely]]{
        throw std::invalid_argument("minimum distance to an empty polygon");
    }
    for(const Point& point: polygon){
        if(!std::isfinite(point.x) || !std::isfinite(point.y))[[unlikely]]{
            throw std::invalid_argument("non-finite polygon vertex");
        }
    }

    std::vector<BoxedSegment> result;
    const std::size_t edge_count = polygon.size() <= 2 ? 1 : polygon.size();
    result.reserve(edge_count);
    for(std::size_t index = 0; index < edge_count; ++index){
        const Point& first = polygon[index % polygon.size()];
        const Point& second = polygon.size() == 1
            ? first
            : polygon[(index + 1) % polygon.size()];
        result.push_back({
            {first, second},
            std::min(first.x, second.x), std::max(first.x, second.x),
            std::min(first.y, second.y), std::max(first.y, second.y),
        });
    }
    return result;
}

inline int build(
    std::vector<BoxedSegment>& segments,
    std::vector<Node>& nodes,
    std::size_t begin,
    std::size_t end
){
    Node node;
    node.begin = begin;
    node.end = end;
    node.minimum_x = node.minimum_y =
        std::numeric_limits<long double>::infinity();
    node.maximum_x = node.maximum_y =
        -std::numeric_limits<long double>::infinity();
    for(std::size_t index = begin; index < end; ++index){
        node.minimum_x = std::min(node.minimum_x, segments[index].minimum_x);
        node.maximum_x = std::max(node.maximum_x, segments[index].maximum_x);
        node.minimum_y = std::min(node.minimum_y, segments[index].minimum_y);
        node.maximum_y = std::max(node.maximum_y, segments[index].maximum_y);
    }
    const int result = static_cast<int>(nodes.size());
    nodes.push_back(node);
    if(end - begin <= 8) return result;

    const bool split_x = node.maximum_x - node.minimum_x
        >= node.maximum_y - node.minimum_y;
    const std::size_t middle = begin + (end - begin) / 2;
    std::nth_element(
        segments.begin() + static_cast<std::ptrdiff_t>(begin),
        segments.begin() + static_cast<std::ptrdiff_t>(middle),
        segments.begin() + static_cast<std::ptrdiff_t>(end),
        [split_x](const BoxedSegment& left, const BoxedSegment& right){
            const long double left_center = split_x
                ? left.minimum_x + left.maximum_x
                : left.minimum_y + left.maximum_y;
            const long double right_center = split_x
                ? right.minimum_x + right.maximum_x
                : right.minimum_y + right.maximum_y;
            return left_center < right_center;
        }
    );
    const int left = build(segments, nodes, begin, middle);
    const int right = build(segments, nodes, middle, end);
    nodes[static_cast<std::size_t>(result)].left = left;
    nodes[static_cast<std::size_t>(result)].right = right;
    return result;
}

inline long double box_distance_squared(const Node& first, const Node& second){
    const long double dx = first.maximum_x < second.minimum_x
        ? second.minimum_x - first.maximum_x
        : second.maximum_x < first.minimum_x
            ? first.minimum_x - second.maximum_x
            : 0.0L;
    const long double dy = first.maximum_y < second.minimum_y
        ? second.minimum_y - first.maximum_y
        : second.maximum_y < first.minimum_y
            ? first.minimum_y - second.maximum_y
            : 0.0L;
    return dx * dx + dy * dy;
}

struct NodePair{
    long double lower_bound;
    int first;
    int second;

    bool operator>(const NodePair& other) const noexcept{
        return lower_bound > other.lower_bound;
    }
};

inline long double boundary_distance(
    std::vector<BoxedSegment> first_segments,
    std::vector<BoxedSegment> second_segments
){
    std::vector<Node> first_nodes;
    std::vector<Node> second_nodes;
    first_nodes.reserve(first_segments.size() * 2);
    second_nodes.reserve(second_segments.size() * 2);
    const int first_root = build(
        first_segments, first_nodes, 0, first_segments.size()
    );
    const int second_root = build(
        second_segments, second_nodes, 0, second_segments.size()
    );

    long double best_squared = std::numeric_limits<long double>::infinity();
    std::priority_queue<
        NodePair,
        std::vector<NodePair>,
        std::greater<NodePair>
    > queue;
    queue.push({box_distance_squared(
        first_nodes[static_cast<std::size_t>(first_root)],
        second_nodes[static_cast<std::size_t>(second_root)]
    ), first_root, second_root});

    while(!queue.empty()){
        const NodePair current = queue.top();
        queue.pop();
        if(current.lower_bound >= best_squared) break;
        const Node& first = first_nodes[static_cast<std::size_t>(current.first)];
        const Node& second = second_nodes[static_cast<std::size_t>(current.second)];
        if(first.leaf() && second.leaf()){
            for(std::size_t left = first.begin; left < first.end; ++left){
                for(std::size_t right = second.begin; right < second.end; ++right){
                    const long double candidate = distance(
                        first_segments[left].segment,
                        second_segments[right].segment
                    );
                    best_squared = std::min(
                        best_squared, candidate * candidate
                    );
                    if(best_squared == 0.0L) return 0.0L;
                }
            }
            continue;
        }

        const bool split_first = !first.leaf() && (
            second.leaf() || first.end - first.begin >= second.end - second.begin
        );
        if(split_first){
            for(const int child: {first.left, first.right}){
                const long double lower = box_distance_squared(
                    first_nodes[static_cast<std::size_t>(child)], second
                );
                if(lower < best_squared){
                    queue.push({lower, child, current.second});
                }
            }
        }else{
            for(const int child: {second.left, second.right}){
                const long double lower = box_distance_squared(
                    first, second_nodes[static_cast<std::size_t>(child)]
                );
                if(lower < best_squared){
                    queue.push({lower, current.first, child});
                }
            }
        }
    }
    return std::sqrt(best_squared);
}

}  // namespace polygon_minimum_distance_detail

inline long double polygon_minimum_distance(
    const std::vector<Point>& first,
    const std::vector<Point>& second
){
    using namespace polygon_minimum_distance_detail;
    std::vector<BoxedSegment> first_segments = polygon_segments(first);
    std::vector<BoxedSegment> second_segments = polygon_segments(second);
    if(first.size() >= 3 && contains(first, second.front()) != 0) return 0.0L;
    if(second.size() >= 3 && contains(second, first.front()) != 0) return 0.0L;
    return boundary_distance(
        std::move(first_segments), std::move(second_segments)
    );
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_SCALAR_POINT_SET_POINT_SET_POLYGON_MINIMUM_DISTANCE_HPP_INCLUDED
