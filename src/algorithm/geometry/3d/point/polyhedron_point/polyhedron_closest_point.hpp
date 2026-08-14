#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINT_POLYHEDRON_POINT_POLYHEDRON_CLOSEST_POINT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINT_POLYHEDRON_POINT_POLYHEDRON_CLOSEST_POINT_HPP_INCLUDED

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <limits>
#include <queue>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../../detail/scalar/linear_closest_detail.hpp"
#include "../../predicate/polyhedron_point/polyhedron_contains.hpp"
#include "../../result/point_point/point3_normalized_difference_points.hpp"
#include "../../type/definition/polyhedron3.hpp"
#include "../point_point/point3_safe_normalized_difference.hpp"
#include "../triangle_point/triangle_point_closest_point.hpp"

namespace polyhedron_closest_point_detail{

struct BoxedFace{
    std::size_t face = 0;
    Point3 minimum{};
    Point3 maximum{};
};

struct Node{
    Point3 minimum{};
    Point3 maximum{};
    std::size_t begin = 0;
    std::size_t end = 0;
    int left = -1;
    int right = -1;
    bool leaf() const noexcept{ return left == -1; }
};

inline int build(
    std::vector<BoxedFace>& faces,
    std::vector<Node>& nodes,
    std::size_t begin,
    std::size_t end
){
    Node node;
    node.begin = begin;
    node.end = end;
    node.minimum = {
        std::numeric_limits<long double>::infinity(),
        std::numeric_limits<long double>::infinity(),
        std::numeric_limits<long double>::infinity(),
    };
    node.maximum = -node.minimum;
    for(std::size_t index = begin; index < end; ++index){
        const BoxedFace& face = faces[index];
        node.minimum.x = std::min(node.minimum.x, face.minimum.x);
        node.minimum.y = std::min(node.minimum.y, face.minimum.y);
        node.minimum.z = std::min(node.minimum.z, face.minimum.z);
        node.maximum.x = std::max(node.maximum.x, face.maximum.x);
        node.maximum.y = std::max(node.maximum.y, face.maximum.y);
        node.maximum.z = std::max(node.maximum.z, face.maximum.z);
    }
    const int result = static_cast<int>(nodes.size());
    nodes.push_back(node);
    if(end - begin <= 8) return result;
    const Point3 extent = node.maximum - node.minimum;
    int axis = extent.y > extent.x ? 1 : 0;
    if((axis == 0 ? extent.x : extent.y) < extent.z) axis = 2;
    const std::size_t middle = begin + (end - begin) / 2;
    std::nth_element(
        faces.begin() + static_cast<std::ptrdiff_t>(begin),
        faces.begin() + static_cast<std::ptrdiff_t>(middle),
        faces.begin() + static_cast<std::ptrdiff_t>(end),
        [axis](const BoxedFace& left, const BoxedFace& right){
            const auto center = [axis](const BoxedFace& face){
                if(axis == 0) return (face.minimum.x + face.maximum.x) / 2.0L;
                if(axis == 1) return (face.minimum.y + face.maximum.y) / 2.0L;
                return (face.minimum.z + face.maximum.z) / 2.0L;
            };
            return center(left) < center(right);
        }
    );
    const int left = build(faces, nodes, begin, middle);
    const int right = build(faces, nodes, middle, end);
    nodes[static_cast<std::size_t>(result)].left = left;
    nodes[static_cast<std::size_t>(result)].right = right;
    return result;
}

inline long double origin_distance_squared(const Node& node){
    const auto gap = [](long double minimum, long double maximum){
        return minimum > 0.0L ? minimum : maximum < 0.0L ? -maximum : 0.0L;
    };
    const long double x = gap(node.minimum.x, node.maximum.x);
    const long double y = gap(node.minimum.y, node.maximum.y);
    const long double z = gap(node.minimum.z, node.maximum.z);
    return x * x + y * y + z * z;
}

struct QueueEntry{
    long double lower_bound;
    int node;
    bool operator>(const QueueEntry& other) const noexcept{
        return lower_bound > other.lower_bound;
    }
};

}  // namespace polyhedron_closest_point_detail

inline Point3 polyhedron_closest_point(
    const Polyhedron3& polyhedron,
    const Point3& point
){
    using namespace polyhedron_closest_point_detail;
    geometry3d_validate(polyhedron);
    if(!geometry3d_is_finite(point))[[unlikely]]{
        throw std::invalid_argument("non-finite polyhedron query point");
    }
    if(polyhedron.faces.empty())[[unlikely]]{
        throw std::invalid_argument("closest point on empty polyhedron surface");
    }
    if(polyhedron_contains_detail::unchecked(polyhedron, point)) return point;

    long double scale = 0.0L;
    for(const Point3& vertex: polyhedron.vertices){
        scale = std::max(
            scale, geometry3d_normalized_difference(vertex, point).scale
        );
    }
    Polyhedron3 local = polyhedron;
    for(std::size_t index = 0; index < local.vertices.size(); ++index){
        local.vertices[index] = geometry3d_safe_normalized_difference(
            polyhedron.vertices[index], point, scale
        );
    }

    std::vector<BoxedFace> faces;
    faces.reserve(local.faces.size());
    for(std::size_t index = 0; index < local.faces.size(); ++index){
        const auto& face = local.faces[index];
        const Point3& first = local.vertices[face[0]];
        const Point3& second = local.vertices[face[1]];
        const Point3& third = local.vertices[face[2]];
        faces.push_back({
            index,
            {std::min({first.x, second.x, third.x}),
             std::min({first.y, second.y, third.y}),
             std::min({first.z, second.z, third.z})},
            {std::max({first.x, second.x, third.x}),
             std::max({first.y, second.y, third.y}),
             std::max({first.z, second.z, third.z})},
        });
    }
    std::vector<Node> nodes;
    nodes.reserve(faces.size() * 2);
    const int root = build(faces, nodes, 0, faces.size());
    std::priority_queue<QueueEntry, std::vector<QueueEntry>, std::greater<QueueEntry>>
        queue;
    queue.push({origin_distance_squared(nodes[static_cast<std::size_t>(root)]), root});
    long double best_squared = std::numeric_limits<long double>::infinity();
    Point3 answer{};
    while(!queue.empty()){
        const QueueEntry current = queue.top();
        queue.pop();
        if(current.lower_bound >= best_squared) break;
        const Node& node = nodes[static_cast<std::size_t>(current.node)];
        if(node.leaf()){
            for(std::size_t position = node.begin; position < node.end; ++position){
                const auto& face = local.faces[faces[position].face];
                const Point3 candidate = closest_point(Triangle3{
                    local.vertices[face[0]], local.vertices[face[1]],
                    local.vertices[face[2]]
                }, Point3{});
                const long double candidate_squared =
                    candidate.x * candidate.x + candidate.y * candidate.y
                    + candidate.z * candidate.z;
                if(candidate_squared < best_squared){
                    best_squared = candidate_squared;
                    answer = candidate;
                }
            }
        }else{
            for(const int child: {node.left, node.right}){
                const long double lower = origin_distance_squared(
                    nodes[static_cast<std::size_t>(child)]
                );
                if(lower < best_squared) queue.push({lower, child});
            }
        }
    }
    return geometry3d_linear_closest_detail::restore(answer, point, scale);
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINT_POLYHEDRON_POINT_POLYHEDRON_CLOSEST_POINT_HPP_INCLUDED
