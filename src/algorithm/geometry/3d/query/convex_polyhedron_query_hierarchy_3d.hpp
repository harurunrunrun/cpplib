#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_QUERY_CONVEX_POLYHEDRON_QUERY_HIERARCHY_3D_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_QUERY_CONVEX_POLYHEDRON_QUERY_HIERARCHY_3D_HPP_INCLUDED

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <limits>
#include <map>
#include <numeric>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../predicate/adaptive_orient3d.hpp"
#include "../point/closest_point.hpp"
#include "../core/convex_polyhedron3.hpp"
#include "../point/convex_polyhedron_closest_point.hpp"
#include "../predicate/convex_polyhedron_contains.hpp"
#include "../predicate/is_finite.hpp"

namespace convex_polyhedron_query_hierarchy_3d_detail{

using Exact = geometry3d_adaptive_detail::ExactDyadic;

inline int compare(const Exact& left, const Exact& right){
    return geometry3d_adaptive_detail::sign(
        geometry3d_adaptive_detail::subtract(left, right)
    );
}

inline Exact minimum(const Exact& left, const Exact& right){
    return compare(left, right) <= 0 ? left : right;
}

inline Exact maximum(const Exact& left, const Exact& right){
    return compare(left, right) >= 0 ? left : right;
}

inline Exact square(const Exact& value){
    return geometry3d_adaptive_detail::multiply(value, value);
}

inline Exact exact_dot(
    const std::array<Exact, 3>& left,
    const std::array<Exact, 3>& right
){
    Exact result{};
    for(std::size_t axis = 0; axis < 3; ++axis){
        result = geometry3d_adaptive_detail::add(
            result,
            geometry3d_adaptive_detail::multiply(left[axis], right[axis])
        );
    }
    return result;
}

inline std::array<Exact, 3> exact_point(const Point3& point){
    return {
        geometry3d_adaptive_detail::exact_dyadic(point.x),
        geometry3d_adaptive_detail::exact_dyadic(point.y),
        geometry3d_adaptive_detail::exact_dyadic(point.z),
    };
}

struct ExactBox3{
    std::array<Exact, 3> minimum{};
    std::array<Exact, 3> maximum{};
};

struct Box3{
    Point3 minimum{};
    Point3 maximum{};
    ExactBox3 exact{};
};

inline long double coordinate(const Point3& point, std::size_t axis){
    if(axis == 0) return point.x;
    if(axis == 1) return point.y;
    return point.z;
}

inline Box3 point_box(const Point3& point){
    const auto exact = exact_point(point);
    return {point, point, {exact, exact}};
}

inline void extend(Box3& box, const Point3& point){
    const auto exact = exact_point(point);
    for(std::size_t axis = 0; axis < 3; ++axis){
        const long double value = coordinate(point, axis);
        if(value < coordinate(box.minimum, axis)){
            if(axis == 0) box.minimum.x = value;
            else if(axis == 1) box.minimum.y = value;
            else box.minimum.z = value;
            box.exact.minimum[axis] = exact[axis];
        }
        if(value > coordinate(box.maximum, axis)){
            if(axis == 0) box.maximum.x = value;
            else if(axis == 1) box.maximum.y = value;
            else box.maximum.z = value;
            box.exact.maximum[axis] = exact[axis];
        }
    }
}

inline Exact exact_squared_distance(const Point3& first, const Point3& second){
    const auto exact_first = exact_point(first);
    const auto exact_second = exact_point(second);
    Exact result{};
    for(std::size_t axis = 0; axis < 3; ++axis){
        const Exact difference = geometry3d_adaptive_detail::subtract(
            exact_first[axis], exact_second[axis]
        );
        result = geometry3d_adaptive_detail::add(result, square(difference));
    }
    return result;
}

inline Exact exact_box_squared_distance(
    const Box3& box,
    const Point3& point,
    const std::array<Exact, 3>& exact_query
){
    Exact result{};
    for(std::size_t axis = 0; axis < 3; ++axis){
        Exact difference{};
        const long double value = coordinate(point, axis);
        if(value < coordinate(box.minimum, axis)){
            difference = geometry3d_adaptive_detail::subtract(
                box.exact.minimum[axis], exact_query[axis]
            );
        }else if(value > coordinate(box.maximum, axis)){
            difference = geometry3d_adaptive_detail::subtract(
                exact_query[axis], box.exact.maximum[axis]
            );
        }
        result = geometry3d_adaptive_detail::add(result, square(difference));
    }
    return result;
}

inline long double approximate_box_distance(const Box3& box, const Point3& point){
    std::array<long double, 3> difference{};
    for(std::size_t axis = 0; axis < 3; ++axis){
        const long double value = coordinate(point, axis);
        if(value < coordinate(box.minimum, axis)){
            difference[axis] = coordinate(box.minimum, axis) - value;
        }else if(value > coordinate(box.maximum, axis)){
            difference[axis] = value - coordinate(box.maximum, axis);
        }
    }
    return std::hypot(difference[0], difference[1], difference[2]);
}

struct ExactPlane{
    std::array<Exact, 4> coefficient{};
};

inline ExactPlane exact_plane(
    const Point3& first,
    const Point3& second,
    const Point3& third
){
    using geometry3d_adaptive_detail::add;
    using geometry3d_adaptive_detail::multiply;
    using geometry3d_adaptive_detail::negate;
    using geometry3d_adaptive_detail::subtract;
    const auto a = exact_point(first);
    const auto b = exact_point(second);
    const auto c = exact_point(third);
    std::array<Exact, 3> first_edge{};
    std::array<Exact, 3> second_edge{};
    for(std::size_t axis = 0; axis < 3; ++axis){
        first_edge[axis] = subtract(b[axis], a[axis]);
        second_edge[axis] = subtract(c[axis], a[axis]);
    }
    ExactPlane plane;
    plane.coefficient[0] = subtract(
        multiply(first_edge[1], second_edge[2]),
        multiply(first_edge[2], second_edge[1])
    );
    plane.coefficient[1] = subtract(
        multiply(first_edge[2], second_edge[0]),
        multiply(first_edge[0], second_edge[2])
    );
    plane.coefficient[2] = subtract(
        multiply(first_edge[0], second_edge[1]),
        multiply(first_edge[1], second_edge[0])
    );
    plane.coefficient[3] = negate(exact_dot(
        {plane.coefficient[0], plane.coefficient[1], plane.coefficient[2]}, a
    ));
    return plane;
}

inline bool nonzero_normal(const ExactPlane& plane){
    return geometry3d_adaptive_detail::sign(plane.coefficient[0]) != 0
        || geometry3d_adaptive_detail::sign(plane.coefficient[1]) != 0
        || geometry3d_adaptive_detail::sign(plane.coefficient[2]) != 0;
}

inline Exact exact_plane_value(
    const ExactPlane& plane,
    const std::array<Exact, 4>& query
){
    Exact result{};
    for(std::size_t axis = 0; axis < 4; ++axis){
        result = geometry3d_adaptive_detail::add(
            result,
            geometry3d_adaptive_detail::multiply(
                plane.coefficient[axis], query[axis]
            )
        );
    }
    return result;
}

inline std::array<long double, 4> plane_key(
    const Point3& first,
    const Point3& second,
    const Point3& third,
    long double coordinate_scale
){
    const long double scale = std::max({
        std::abs(first.x), std::abs(first.y), std::abs(first.z),
        std::abs(second.x), std::abs(second.y), std::abs(second.z),
        std::abs(third.x), std::abs(third.y), std::abs(third.z),
        1.0L,
    });
    const Point3 a = first / scale;
    const Point3 u = second / scale - a;
    const Point3 v = third / scale - a;
    Point3 normal{
        u.y * v.z - u.z * v.y,
        u.z * v.x - u.x * v.z,
        u.x * v.y - u.y * v.x,
    };
    const long double length = std::hypot(normal.x, normal.y, normal.z);
    if(length != 0 && std::isfinite(length)) normal /= length;
    else normal = {};
    const Point3 scaled_first = first / coordinate_scale;
    return {
        normal.x,
        normal.y,
        normal.z,
        -(normal.x * scaled_first.x + normal.y * scaled_first.y
            + normal.z * scaled_first.z),
    };
}

}  // namespace convex_polyhedron_query_hierarchy_3d_detail

class ConvexPolyhedronQueryHierarchy3D{
    using Detail = convex_polyhedron_query_hierarchy_3d_detail::Exact;

    static constexpr std::size_t no_index =
        std::numeric_limits<std::size_t>::max();

    struct VertexNode{
        convex_polyhedron_query_hierarchy_3d_detail::Box3 box{};
        std::size_t begin = 0;
        std::size_t end = 0;
        std::size_t left = no_index;
        std::size_t right = no_index;
        std::size_t minimum_index = no_index;

        bool leaf() const noexcept{ return left == no_index; }
    };

    struct FaceNode{
        convex_polyhedron_query_hierarchy_3d_detail::Box3 box{};
        std::size_t begin = 0;
        std::size_t end = 0;
        std::size_t left = no_index;
        std::size_t right = no_index;

        bool leaf() const noexcept{ return left == no_index; }
    };

    struct HalfspaceNode{
        std::array<Detail, 4> minimum{};
        std::array<Detail, 4> maximum{};
        std::size_t begin = 0;
        std::size_t end = 0;
        std::size_t left = no_index;
        std::size_t right = no_index;

        bool leaf() const noexcept{ return left == no_index; }
    };

    ConvexPolyhedron3 polyhedron_;
    std::size_t leaf_capacity_ = 8;
    std::vector<std::size_t> vertex_order_;
    std::vector<VertexNode> vertex_nodes_;
    std::vector<std::size_t> closest_face_order_;
    std::vector<FaceNode> face_nodes_;
    std::vector<std::size_t> halfspace_face_order_;
    std::vector<HalfspaceNode> halfspace_nodes_;
    std::vector<convex_polyhedron_query_hierarchy_3d_detail::ExactPlane>
        exact_planes_;
    std::vector<std::array<long double, 4>> plane_keys_;

    static Triangle3 triangle(
        const ConvexPolyhedron3& polyhedron,
        std::size_t face
    ){
        const auto& indices = polyhedron.faces[face];
        return {
            polyhedron.vertices[indices[0]],
            polyhedron.vertices[indices[1]],
            polyhedron.vertices[indices[2]],
        };
    }

    static Point3 triangle_centroid(const Triangle3& value){
        return {
            value.a.x / 3 + value.b.x / 3 + value.c.x / 3,
            value.a.y / 3 + value.b.y / 3 + value.c.y / 3,
            value.a.z / 3 + value.b.z / 3 + value.c.z / 3,
        };
    }

    static convex_polyhedron_query_hierarchy_3d_detail::Box3 triangle_box(
        const Triangle3& value
    ){
        using namespace convex_polyhedron_query_hierarchy_3d_detail;
        Box3 result = point_box(value.a);
        extend(result, value.b);
        extend(result, value.c);
        return result;
    }

    void validate_and_orient(){
        using namespace convex_polyhedron_query_hierarchy_3d_detail;
        if(polyhedron_.vertices.empty()){
            throw std::invalid_argument("empty convex polyhedron query hierarchy");
        }
        if(polyhedron_.affine_dimension < 0 || polyhedron_.affine_dimension > 3){
            throw std::invalid_argument("invalid convex polyhedron affine dimension");
        }
        for(const Point3& point: polyhedron_.vertices){
            if(!geometry3d_is_finite(point)){
                throw std::invalid_argument("non-finite convex polyhedron vertex");
            }
        }
        for(const auto& face: polyhedron_.faces){
            for(const std::size_t vertex: face){
                if(vertex >= polyhedron_.vertices.size()){
                    throw std::out_of_range("convex polyhedron face index");
                }
            }
            if(face[0] == face[1] || face[1] == face[2] || face[2] == face[0]){
                throw std::domain_error("repeated convex polyhedron face vertex");
            }
            if(!nonzero_normal(exact_plane(
                polyhedron_.vertices[face[0]],
                polyhedron_.vertices[face[1]],
                polyhedron_.vertices[face[2]]
            ))){
                throw std::domain_error("degenerate convex polyhedron face");
            }
        }
        if(polyhedron_.affine_dimension == 0){
            if(polyhedron_.vertices.size() != 1 || !polyhedron_.faces.empty()){
                throw std::domain_error("invalid zero-dimensional convex polyhedron");
            }
            return;
        }
        if(polyhedron_.affine_dimension == 1){
            if(polyhedron_.vertices.size() < 2 || !polyhedron_.faces.empty()){
                throw std::domain_error("invalid one-dimensional convex polyhedron");
            }
            return;
        }
        if(polyhedron_.faces.empty()){
            throw std::domain_error("convex polyhedron faces are missing");
        }
        if(polyhedron_.affine_dimension == 2) return;
        if(polyhedron_.vertices.size() < 4 || polyhedron_.faces.size() < 4){
            throw std::domain_error("invalid three-dimensional convex polyhedron");
        }

        struct EdgeRecord{
            std::size_t count = 0;
            int orientation_sum = 0;
            std::array<std::size_t, 2> faces{no_index, no_index};
        };
        std::map<std::pair<std::size_t, std::size_t>, EdgeRecord> edges;
        std::vector<char> vertex_used(polyhedron_.vertices.size(), 0);
        for(std::size_t face_index = 0;
            face_index < polyhedron_.faces.size(); ++face_index){
            const auto& face = polyhedron_.faces[face_index];
            for(const std::size_t vertex: face) vertex_used[vertex] = 1;
            for(std::size_t edge = 0; edge < 3; ++edge){
                const std::size_t first = face[edge];
                const std::size_t second = face[(edge + 1) % 3];
                const auto key = std::minmax(first, second);
                EdgeRecord& record = edges[key];
                if(record.count < record.faces.size()){
                    record.faces[record.count] = face_index;
                }
                ++record.count;
                record.orientation_sum += first < second ? 1 : -1;
            }
        }
        for(const auto& [edge, record]: edges){
            static_cast<void>(edge);
            if(record.count != 2 || record.orientation_sum != 0){
                throw std::domain_error(
                    "convex polyhedron must be a consistently oriented closed mesh"
                );
            }
        }

        if(std::find(vertex_used.begin(), vertex_used.end(), 0)
           != vertex_used.end()){
            throw std::domain_error(
                "convex polyhedron has a vertex not incident to any face"
            );
        }
        std::vector<std::vector<std::size_t>> face_adjacency(
            polyhedron_.faces.size()
        );
        for(const auto& [edge, record]: edges){
            static_cast<void>(edge);
            face_adjacency[record.faces[0]].push_back(record.faces[1]);
            face_adjacency[record.faces[1]].push_back(record.faces[0]);
        }
        std::vector<char> face_seen(polyhedron_.faces.size(), 0);
        std::vector<std::size_t> pending{0};
        face_seen[0] = 1;
        while(!pending.empty()){
            const std::size_t face = pending.back();
            pending.pop_back();
            for(const std::size_t adjacent: face_adjacency[face]){
                if(face_seen[adjacent]) continue;
                face_seen[adjacent] = 1;
                pending.push_back(adjacent);
            }
        }
        if(std::find(face_seen.begin(), face_seen.end(), 0) != face_seen.end()){
            throw std::domain_error(
                "convex polyhedron boundary mesh must be connected"
            );
        }

        Detail signed_six_volume{};
        for(const auto& face: polyhedron_.faces){
            const auto plane = exact_plane(
                polyhedron_.vertices[face[0]],
                polyhedron_.vertices[face[1]],
                polyhedron_.vertices[face[2]]
            );
            signed_six_volume = geometry3d_adaptive_detail::subtract(
                signed_six_volume, plane.coefficient[3]
            );
        }
        const int orientation = geometry3d_adaptive_detail::sign(
            signed_six_volume
        );
        if(orientation == 0){
            throw std::domain_error("zero-volume convex polyhedron mesh");
        }
        if(orientation < 0){
            for(auto& face: polyhedron_.faces) std::swap(face[1], face[2]);
        }
    }

    std::size_t build_vertex_node(std::size_t begin, std::size_t end){
        using namespace convex_polyhedron_query_hierarchy_3d_detail;
        const std::size_t node_index = vertex_nodes_.size();
        vertex_nodes_.push_back({});
        VertexNode node;
        node.begin = begin;
        node.end = end;
        node.minimum_index = vertex_order_[begin];
        node.box = point_box(polyhedron_.vertices[vertex_order_[begin]]);
        Point3 minimum = polyhedron_.vertices[vertex_order_[begin]];
        Point3 maximum = minimum;
        for(std::size_t position = begin + 1; position < end; ++position){
            const std::size_t vertex = vertex_order_[position];
            extend(node.box, polyhedron_.vertices[vertex]);
            node.minimum_index = std::min(node.minimum_index, vertex);
            for(std::size_t axis = 0; axis < 3; ++axis){
                const long double value = coordinate(polyhedron_.vertices[vertex], axis);
                if(value < coordinate(minimum, axis)){
                    if(axis == 0) minimum.x = value;
                    else if(axis == 1) minimum.y = value;
                    else minimum.z = value;
                }
                if(value > coordinate(maximum, axis)){
                    if(axis == 0) maximum.x = value;
                    else if(axis == 1) maximum.y = value;
                    else maximum.z = value;
                }
            }
        }
        if(end - begin > leaf_capacity_){
            std::size_t axis = 0;
            for(std::size_t candidate = 1; candidate < 3; ++candidate){
                if(coordinate(maximum, candidate) - coordinate(minimum, candidate)
                   > coordinate(maximum, axis) - coordinate(minimum, axis)){
                    axis = candidate;
                }
            }
            const std::size_t middle = begin + (end - begin) / 2;
            std::nth_element(
                vertex_order_.begin() + static_cast<std::ptrdiff_t>(begin),
                vertex_order_.begin() + static_cast<std::ptrdiff_t>(middle),
                vertex_order_.begin() + static_cast<std::ptrdiff_t>(end),
                [&](std::size_t left, std::size_t right){
                    const long double lhs = coordinate(polyhedron_.vertices[left], axis);
                    const long double rhs = coordinate(polyhedron_.vertices[right], axis);
                    return lhs != rhs ? lhs < rhs : left < right;
                }
            );
            node.left = build_vertex_node(begin, middle);
            node.right = build_vertex_node(middle, end);
        }
        vertex_nodes_[node_index] = std::move(node);
        return node_index;
    }

    std::size_t build_face_node(std::size_t begin, std::size_t end){
        using namespace convex_polyhedron_query_hierarchy_3d_detail;
        const std::size_t node_index = face_nodes_.size();
        face_nodes_.push_back({});
        FaceNode node;
        node.begin = begin;
        node.end = end;
        const Triangle3 initial = triangle(polyhedron_, closest_face_order_[begin]);
        node.box = triangle_box(initial);
        Point3 centroid_minimum = triangle_centroid(initial);
        Point3 centroid_maximum = centroid_minimum;
        for(std::size_t position = begin + 1; position < end; ++position){
            const Triangle3 value = triangle(polyhedron_, closest_face_order_[position]);
            extend(node.box, value.a);
            extend(node.box, value.b);
            extend(node.box, value.c);
            const Point3 centroid = triangle_centroid(value);
            for(std::size_t axis = 0; axis < 3; ++axis){
                const long double coordinate_value = coordinate(centroid, axis);
                if(coordinate_value < coordinate(centroid_minimum, axis)){
                    if(axis == 0) centroid_minimum.x = coordinate_value;
                    else if(axis == 1) centroid_minimum.y = coordinate_value;
                    else centroid_minimum.z = coordinate_value;
                }
                if(coordinate_value > coordinate(centroid_maximum, axis)){
                    if(axis == 0) centroid_maximum.x = coordinate_value;
                    else if(axis == 1) centroid_maximum.y = coordinate_value;
                    else centroid_maximum.z = coordinate_value;
                }
            }
        }
        if(end - begin > leaf_capacity_){
            std::size_t axis = 0;
            for(std::size_t candidate = 1; candidate < 3; ++candidate){
                if(coordinate(centroid_maximum, candidate)
                       - coordinate(centroid_minimum, candidate)
                   > coordinate(centroid_maximum, axis)
                       - coordinate(centroid_minimum, axis)){
                    axis = candidate;
                }
            }
            const std::size_t middle = begin + (end - begin) / 2;
            std::nth_element(
                closest_face_order_.begin() + static_cast<std::ptrdiff_t>(begin),
                closest_face_order_.begin() + static_cast<std::ptrdiff_t>(middle),
                closest_face_order_.begin() + static_cast<std::ptrdiff_t>(end),
                [&](std::size_t left, std::size_t right){
                    const long double lhs = coordinate(
                        triangle_centroid(triangle(polyhedron_, left)), axis
                    );
                    const long double rhs = coordinate(
                        triangle_centroid(triangle(polyhedron_, right)), axis
                    );
                    return lhs != rhs ? lhs < rhs : left < right;
                }
            );
            node.left = build_face_node(begin, middle);
            node.right = build_face_node(middle, end);
        }
        face_nodes_[node_index] = std::move(node);
        return node_index;
    }

    std::size_t build_halfspace_node(std::size_t begin, std::size_t end){
        using namespace convex_polyhedron_query_hierarchy_3d_detail;
        const std::size_t node_index = halfspace_nodes_.size();
        halfspace_nodes_.push_back({});
        HalfspaceNode node;
        node.begin = begin;
        node.end = end;
        node.minimum = exact_planes_[halfspace_face_order_[begin]].coefficient;
        node.maximum = node.minimum;
        std::array<long double, 4> key_minimum =
            plane_keys_[halfspace_face_order_[begin]];
        std::array<long double, 4> key_maximum = key_minimum;
        for(std::size_t position = begin + 1; position < end; ++position){
            const std::size_t face = halfspace_face_order_[position];
            for(std::size_t axis = 0; axis < 4; ++axis){
                node.minimum[axis] = minimum(
                    node.minimum[axis], exact_planes_[face].coefficient[axis]
                );
                node.maximum[axis] = maximum(
                    node.maximum[axis], exact_planes_[face].coefficient[axis]
                );
                key_minimum[axis] = std::min(
                    key_minimum[axis], plane_keys_[face][axis]
                );
                key_maximum[axis] = std::max(
                    key_maximum[axis], plane_keys_[face][axis]
                );
            }
        }
        if(end - begin > leaf_capacity_){
            std::size_t axis = 0;
            for(std::size_t candidate = 1; candidate < 4; ++candidate){
                if(key_maximum[candidate] - key_minimum[candidate]
                   > key_maximum[axis] - key_minimum[axis]) axis = candidate;
            }
            const std::size_t middle = begin + (end - begin) / 2;
            std::nth_element(
                halfspace_face_order_.begin() + static_cast<std::ptrdiff_t>(begin),
                halfspace_face_order_.begin() + static_cast<std::ptrdiff_t>(middle),
                halfspace_face_order_.begin() + static_cast<std::ptrdiff_t>(end),
                [&](std::size_t left, std::size_t right){
                    const long double lhs = plane_keys_[left][axis];
                    const long double rhs = plane_keys_[right][axis];
                    return lhs != rhs ? lhs < rhs : left < right;
                }
            );
            node.left = build_halfspace_node(begin, middle);
            node.right = build_halfspace_node(middle, end);
        }
        halfspace_nodes_[node_index] = std::move(node);
        return node_index;
    }

    Detail support_upper_bound(
        const VertexNode& node,
        const std::array<Detail, 3>& direction
    ) const{
        Detail result{};
        for(std::size_t axis = 0; axis < 3; ++axis){
            const Detail& coordinate = geometry3d_adaptive_detail::sign(
                direction[axis]
            ) >= 0 ? node.box.exact.maximum[axis]
                : node.box.exact.minimum[axis];
            result = geometry3d_adaptive_detail::add(
                result,
                geometry3d_adaptive_detail::multiply(coordinate, direction[axis])
            );
        }
        return result;
    }

    long double approximate_support_upper_bound(
        const VertexNode& node,
        const Point3& direction
    ) const{
        const Point3 point{
            direction.x >= 0 ? node.box.maximum.x : node.box.minimum.x,
            direction.y >= 0 ? node.box.maximum.y : node.box.minimum.y,
            direction.z >= 0 ? node.box.maximum.z : node.box.minimum.z,
        };
        return point.x * direction.x + point.y * direction.y
            + point.z * direction.z;
    }

    void support_search(
        std::size_t node_index,
        const Point3& direction,
        const std::array<Detail, 3>& exact_direction,
        std::size_t& answer,
        Detail& best
    ) const{
        using namespace convex_polyhedron_query_hierarchy_3d_detail;
        const VertexNode& node = vertex_nodes_[node_index];
        const Detail upper = support_upper_bound(node, exact_direction);
        const int bound_order = compare(upper, best);
        if(bound_order < 0
           || (bound_order == 0 && node.minimum_index >= answer)) return;
        if(node.leaf()){
            for(std::size_t position = node.begin; position < node.end; ++position){
                const std::size_t vertex = vertex_order_[position];
                const Detail objective = exact_dot(
                    exact_point(polyhedron_.vertices[vertex]), exact_direction
                );
                const int order = compare(objective, best);
                if(order > 0 || (order == 0 && vertex < answer)){
                    answer = vertex;
                    best = objective;
                }
            }
            return;
        }
        std::size_t first = node.left;
        std::size_t second = node.right;
        if(approximate_support_upper_bound(vertex_nodes_[second], direction)
           > approximate_support_upper_bound(vertex_nodes_[first], direction)){
            std::swap(first, second);
        }
        support_search(first, direction, exact_direction, answer, best);
        support_search(second, direction, exact_direction, answer, best);
    }

    Detail halfspace_upper_bound(
        const HalfspaceNode& node,
        const std::array<Detail, 4>& query
    ) const{
        Detail result{};
        for(std::size_t axis = 0; axis < 4; ++axis){
            const Detail& coefficient = geometry3d_adaptive_detail::sign(
                query[axis]
            ) >= 0 ? node.maximum[axis] : node.minimum[axis];
            result = geometry3d_adaptive_detail::add(
                result,
                geometry3d_adaptive_detail::multiply(coefficient, query[axis])
            );
        }
        return result;
    }

    bool contains_search(
        std::size_t node_index,
        const std::array<Detail, 4>& exact_query
    ) const{
        const HalfspaceNode& node = halfspace_nodes_[node_index];
        if(geometry3d_adaptive_detail::sign(
            halfspace_upper_bound(node, exact_query)
        ) <= 0) return true;
        if(node.leaf()){
            for(std::size_t position = node.begin; position < node.end; ++position){
                const std::size_t face = halfspace_face_order_[position];
                if(geometry3d_adaptive_detail::sign(
                    convex_polyhedron_query_hierarchy_3d_detail::exact_plane_value(
                        exact_planes_[face], exact_query
                    )
                ) > 0) return false;
            }
            return true;
        }
        return contains_search(node.left, exact_query)
            && contains_search(node.right, exact_query);
    }

    void closest_search(
        std::size_t node_index,
        const Point3& point,
        const std::array<Detail, 3>& exact_query,
        Point3& answer,
        Detail& answer_best,
        Detail& pruning_upper
    ) const{
        using namespace convex_polyhedron_query_hierarchy_3d_detail;
        const FaceNode& node = face_nodes_[node_index];
        if(compare(
            exact_box_squared_distance(node.box, point, exact_query),
            pruning_upper
        ) >= 0) return;
        if(node.leaf()){
            for(std::size_t position = node.begin; position < node.end; ++position){
                const std::size_t face_index = closest_face_order_[position];
                const auto& indices = polyhedron_.faces[face_index];
                for(const std::size_t vertex: indices){
                    const Detail vertex_distance = exact_squared_distance(
                        polyhedron_.vertices[vertex], point
                    );
                    if(compare(vertex_distance, pruning_upper) < 0){
                        pruning_upper = vertex_distance;
                    }
                }
                const Triangle3 face = triangle(polyhedron_, face_index);
                const Point3 candidate = ::closest_point(face, point);
                const Detail candidate_distance = exact_squared_distance(
                    candidate, point
                );
                if(compare(candidate_distance, answer_best) < 0){
                    answer = candidate;
                    answer_best = candidate_distance;
                }
            }
            return;
        }
        std::size_t first = node.left;
        std::size_t second = node.right;
        if(approximate_box_distance(face_nodes_[second].box, point)
           < approximate_box_distance(face_nodes_[first].box, point)){
            std::swap(first, second);
        }
        closest_search(
            first, point, exact_query, answer, answer_best, pruning_upper
        );
        closest_search(
            second, point, exact_query, answer, answer_best, pruning_upper
        );
    }

    void build(){
        vertex_order_.resize(polyhedron_.vertices.size());
        std::iota(vertex_order_.begin(), vertex_order_.end(), std::size_t{0});
        vertex_nodes_.clear();
        vertex_nodes_.reserve(polyhedron_.vertices.size() * 2);
        build_vertex_node(0, vertex_order_.size());

        if(polyhedron_.faces.empty()) return;
        closest_face_order_.resize(polyhedron_.faces.size());
        std::iota(
            closest_face_order_.begin(), closest_face_order_.end(), std::size_t{0}
        );
        face_nodes_.clear();
        face_nodes_.reserve(polyhedron_.faces.size() * 2);
        build_face_node(0, closest_face_order_.size());

        if(polyhedron_.affine_dimension != 3) return;
        exact_planes_.reserve(polyhedron_.faces.size());
        long double coordinate_scale = 1;
        for(const Point3& point: polyhedron_.vertices){
            coordinate_scale = std::max({
                coordinate_scale, std::abs(point.x), std::abs(point.y),
                std::abs(point.z),
            });
        }
        plane_keys_.reserve(polyhedron_.faces.size());
        for(const auto& face: polyhedron_.faces){
            exact_planes_.push_back(
                convex_polyhedron_query_hierarchy_3d_detail::exact_plane(
                    polyhedron_.vertices[face[0]], polyhedron_.vertices[face[1]],
                    polyhedron_.vertices[face[2]]
                )
            );
            plane_keys_.push_back(
                convex_polyhedron_query_hierarchy_3d_detail::plane_key(
                    polyhedron_.vertices[face[0]], polyhedron_.vertices[face[1]],
                    polyhedron_.vertices[face[2]], coordinate_scale
                )
            );
        }
        halfspace_face_order_.resize(polyhedron_.faces.size());
        std::iota(
            halfspace_face_order_.begin(), halfspace_face_order_.end(),
            std::size_t{0}
        );
        halfspace_nodes_.clear();
        halfspace_nodes_.reserve(polyhedron_.faces.size() * 2);
        build_halfspace_node(0, halfspace_face_order_.size());
    }

public:
    explicit ConvexPolyhedronQueryHierarchy3D(
        ConvexPolyhedron3 polyhedron,
        std::size_t leaf_capacity = 8
    ): polyhedron_(std::move(polyhedron)), leaf_capacity_(leaf_capacity){
        if(leaf_capacity_ == 0){
            throw std::invalid_argument(
                "convex polyhedron query hierarchy leaf capacity must be positive"
            );
        }
        validate_and_orient();
        build();
    }

    const ConvexPolyhedron3& polyhedron() const noexcept{ return polyhedron_; }
    std::size_t vertex_count() const noexcept{ return polyhedron_.vertices.size(); }
    std::size_t face_count() const noexcept{ return polyhedron_.faces.size(); }

    Point3 support_point(const Point3& direction) const{
        using namespace convex_polyhedron_query_hierarchy_3d_detail;
        if(!geometry3d_is_finite(direction)){
            throw std::invalid_argument("non-finite support direction");
        }
        if(direction.x == 0 && direction.y == 0 && direction.z == 0){
            throw std::invalid_argument("zero support direction");
        }
        const auto exact_direction = exact_point(direction);
        std::size_t answer = 0;
        Detail best = exact_dot(exact_point(polyhedron_.vertices[0]), exact_direction);
        support_search(0, direction, exact_direction, answer, best);
        return polyhedron_.vertices[answer];
    }

    bool contains(const Point3& point) const{
        if(!geometry3d_is_finite(point)){
            throw std::invalid_argument("non-finite convex polyhedron query point");
        }
        if(polyhedron_.affine_dimension != 3){
            return convex_polyhedron_contains(polyhedron_, point);
        }
        using namespace convex_polyhedron_query_hierarchy_3d_detail;
        const auto exact = exact_point(point);
        const std::array<Detail, 4> query{
            exact[0], exact[1], exact[2],
            geometry3d_adaptive_detail::exact_dyadic(1.0L),
        };
        return contains_search(0, query);
    }

    Point3 closest_point(const Point3& point) const{
        if(!geometry3d_is_finite(point)){
            throw std::invalid_argument("non-finite convex polyhedron query point");
        }
        if(polyhedron_.affine_dimension < 2){
            return convex_polyhedron_closest_point(polyhedron_, point);
        }
        if(contains(point)) return point;
        const Triangle3 first_face = triangle(polyhedron_, 0);
        Point3 answer = ::closest_point(first_face, point);
        Detail answer_best =
            convex_polyhedron_query_hierarchy_3d_detail::exact_squared_distance(
                answer, point
            );
        const auto exact_query =
            convex_polyhedron_query_hierarchy_3d_detail::exact_point(point);
        Detail pruning_upper =
            convex_polyhedron_query_hierarchy_3d_detail::exact_squared_distance(
                polyhedron_.vertices[polyhedron_.faces[0][0]], point
            );
        for(std::size_t corner = 1; corner < 3; ++corner){
            const Detail vertex_distance =
                convex_polyhedron_query_hierarchy_3d_detail::exact_squared_distance(
                    polyhedron_.vertices[polyhedron_.faces[0][corner]], point
                );
            if(convex_polyhedron_query_hierarchy_3d_detail::compare(
                vertex_distance, pruning_upper
            ) < 0){
                pruning_upper = vertex_distance;
            }
        }
        closest_search(
            0, point, exact_query, answer, answer_best, pruning_upper
        );
        return answer;
    }
};

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_QUERY_CONVEX_POLYHEDRON_QUERY_HIERARCHY_3D_HPP_INCLUDED
