#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SHAPE_MINKOWSKI_SUM_3D_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SHAPE_MINKOWSKI_SUM_3D_HPP_INCLUDED

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "../predicate/adaptive_orient3d.hpp"
#include "convex_hull_3d.hpp"
#include "../core/convex_polyhedron3.hpp"
#include "../query/convex_polyhedron_facets.hpp"
#include "../point/cross.hpp"
#include "../scalar/dot.hpp"
#include "../predicate/is_finite.hpp"

namespace minkowski_sum_3d_detail{

inline std::vector<Point3> candidate_points(
    const ConvexPolyhedron3& first,
    const ConvexPolyhedron3& second
){
    if(first.vertices.empty() || second.vertices.empty())[[unlikely]]{
        throw std::invalid_argument("Minkowski sum requires nonempty operands");
    }

    std::vector<Point3> points;
    if(second.vertices.size()
        > points.max_size() / first.vertices.size())[[unlikely]]{
        throw std::length_error(
            "Minkowski sum candidate count exceeds the vector limit"
        );
    }
    points.reserve(first.vertices.size() * second.vertices.size());
    for(const Point3& left: first.vertices){
        for(const Point3& right: second.vertices){
            points.push_back(left + right);
        }
    }
    return points;
}

struct DirectEdgeKey{
    std::size_t first = 0;
    std::size_t second = 0;

    friend bool operator==(
        const DirectEdgeKey&,
        const DirectEdgeKey&
    ) = default;
};

inline DirectEdgeKey direct_edge_key(
    std::size_t first,
    std::size_t second
){
    if(second < first) std::swap(first, second);
    return {first, second};
}

struct DirectEdgeKeyHash{
    std::size_t operator()(const DirectEdgeKey& key) const noexcept{
        const std::size_t salt = static_cast<std::size_t>(
            0x9e3779b97f4a7c15ULL
        );
        return std::hash<std::size_t>{}(key.first)
            ^ (std::hash<std::size_t>{}(key.second) + salt
               + (key.first << 6) + (key.first >> 2));
    }
};

struct DirectVertexPair{
    std::size_t first = 0;
    std::size_t second = 0;

    friend bool operator==(
        const DirectVertexPair&,
        const DirectVertexPair&
    ) = default;
};

struct DirectVertexPairHash{
    std::size_t operator()(const DirectVertexPair& key) const noexcept{
        return DirectEdgeKeyHash{}({key.first, key.second});
    }
};

struct DirectPointKey{
    long double x = 0;
    long double y = 0;
    long double z = 0;

    friend bool operator==(
        const DirectPointKey&,
        const DirectPointKey&
    ) = default;
};

struct DirectPointKeyHash{
    std::size_t operator()(const DirectPointKey& point) const noexcept{
        const std::size_t salt = static_cast<std::size_t>(
            0x9e3779b97f4a7c15ULL
        );
        const std::size_t x = std::hash<long double>{}(point.x);
        const std::size_t y = std::hash<long double>{}(point.y);
        const std::size_t z = std::hash<long double>{}(point.z);
        const std::size_t xy = x ^ (y + salt + (x << 6) + (x >> 2));
        return xy ^ (z + salt + (xy << 6) + (xy >> 2));
    }
};

struct DirectTriangleHash{
    std::size_t operator()(
        const std::array<std::size_t, 3>& triangle
    ) const noexcept{
        const std::size_t salt = static_cast<std::size_t>(
            0x9e3779b97f4a7c15ULL
        );
        std::size_t value = triangle[0];
        for(std::size_t index = 1; index < 3; ++index){
            value ^= triangle[index] + salt + (value << 6) + (value >> 2);
        }
        return value;
    }
};

struct DirectFacet{
    std::vector<std::size_t> boundary;
    std::size_t witness_triangle = 0;
    Point3 normal;
};

struct DirectEdge{
    std::size_t first = 0;
    std::size_t second = 0;
    std::array<std::size_t, 2> facets{};
    std::size_t facet_count = 0;
    Point3 direction;
};

struct DirectOperand{
    const ConvexPolyhedron3* polyhedron = nullptr;
    std::vector<DirectFacet> facets;
    std::vector<DirectEdge> edges;
};

inline Point3 direct_unit_direction(
    const Point3& from,
    const Point3& to,
    long double margin,
    const char* message
){
    const Geometry3DNormalizedDifference difference =
        geometry3d_normalized_difference(to, from);
    const long double length = std::hypot(
        difference.value.x, difference.value.y, difference.value.z
    );
    if(!std::isfinite(length) || length * length <= margin)[[unlikely]]{
        throw std::domain_error(message);
    }
    return difference.value / length;
}

inline Point3 direct_triangle_normal(
    const ConvexPolyhedron3& polyhedron,
    const std::array<std::size_t, 3>& face,
    long double margin
){
    const Point3 first = direct_unit_direction(
        polyhedron.vertices[face[0]], polyhedron.vertices[face[1]], margin,
        "Minkowski direct construction has an unstable triangle edge"
    );
    const Point3 second = direct_unit_direction(
        polyhedron.vertices[face[0]], polyhedron.vertices[face[2]], margin,
        "Minkowski direct construction has an unstable triangle edge"
    );
    const Point3 normal = cross(first, second);
    const long double length = std::hypot(normal.x, normal.y, normal.z);
    if(!std::isfinite(length) || length <= margin)[[unlikely]]{
        throw std::domain_error(
            "Minkowski direct construction requires stable triangles"
        );
    }
    return normal / length;
}

inline DirectOperand prepare_direct_operand(
    const ConvexPolyhedron3& polyhedron,
    long double margin
){
    if(polyhedron.affine_dimension != 3 || polyhedron.vertices.size() < 4
        || polyhedron.faces.size() < 4)[[unlikely]]{
        throw std::invalid_argument(
            "Minkowski direct construction requires full-dimensional operands"
        );
    }
    for(const Point3& point: polyhedron.vertices){
        if(!geometry3d_is_finite(point))[[unlikely]]{
            throw std::invalid_argument(
                "Minkowski direct construction requires finite vertices"
            );
        }
    }

    const std::vector<ConvexPolyhedronFacet3> source_facets =
        convex_polyhedron_facets(polyhedron);
    if(source_facets.size() < 4)[[unlikely]]{
        throw std::domain_error(
            "Minkowski direct construction requires a closed 3D boundary"
        );
    }
    DirectOperand result;
    result.polyhedron = &polyhedron;
    result.facets.reserve(source_facets.size());
    for(const ConvexPolyhedronFacet3& facet: source_facets){
        if(facet.boundary.size() < 3 || facet.triangles.empty())[[unlikely]]{
            throw std::domain_error(
                "Minkowski direct construction requires polygonal facets"
            );
        }
        const std::size_t triangle = facet.triangles.front();
        if(triangle >= polyhedron.faces.size())[[unlikely]]{
            throw std::out_of_range("convex polyhedron facet triangle index");
        }
        result.facets.push_back({
            facet.boundary,
            triangle,
            direct_triangle_normal(polyhedron, polyhedron.faces[triangle], margin)
        });
    }

    std::unordered_map<DirectEdgeKey, std::size_t, DirectEdgeKeyHash>
        edge_indices;
    edge_indices.reserve(polyhedron.faces.size() * 2 + 1);
    for(std::size_t facet = 0; facet < result.facets.size(); ++facet){
        const std::vector<std::size_t>& boundary =
            result.facets[facet].boundary;
        for(std::size_t index = 0; index < boundary.size(); ++index){
            const DirectEdgeKey key = direct_edge_key(
                boundary[index], boundary[(index + 1) % boundary.size()]
            );
            auto [iterator, inserted] = edge_indices.emplace(
                key, result.edges.size()
            );
            if(inserted){
                result.edges.push_back({key.first, key.second, {}, 0, {}});
            }
            DirectEdge& edge = result.edges[iterator->second];
            if(edge.facet_count == 2)[[unlikely]]{
                throw std::domain_error(
                    "Minkowski direct construction requires manifold facets"
                );
            }
            edge.facets[edge.facet_count++] = facet;
        }
    }

    for(DirectEdge& edge: result.edges){
        if(edge.facet_count != 2)[[unlikely]]{
            throw std::domain_error(
                "Minkowski direct construction requires a closed operand"
            );
        }
        edge.direction = direct_unit_direction(
            polyhedron.vertices[edge.first], polyhedron.vertices[edge.second],
            margin, "Minkowski direct construction has an unstable edge"
        );
        const Point3& first_normal = result.facets[edge.facets[0]].normal;
        const Point3& second_normal = result.facets[edge.facets[1]].normal;
        const Point3 normal_cross = cross(first_normal, second_normal);
        const long double sine = std::hypot(
            normal_cross.x, normal_cross.y, normal_cross.z
        );
        if(!std::isfinite(sine) || sine * sine <= margin
            || std::abs(dot(edge.direction, first_normal)) > margin * 8
            || std::abs(dot(edge.direction, second_normal)) > margin * 8)
            [[unlikely]]{
            throw std::domain_error(
                "Minkowski direct construction requires stable dihedral angles"
            );
        }
    }

    for(std::size_t facet = 0; facet < result.facets.size(); ++facet){
        const DirectFacet& current = result.facets[facet];
        const DirectEdgeKey key = direct_edge_key(
            current.boundary[0], current.boundary[1]
        );
        const DirectEdge& edge = result.edges[edge_indices.at(key)];
        const std::size_t adjacent = edge.facets[0] == facet
            ? edge.facets[1] : edge.facets[0];
        const auto& face = polyhedron.faces[current.witness_triangle];
        int interior_side = 0;
        for(const std::size_t vertex: result.facets[adjacent].boundary){
            interior_side = adaptive_orient3d(
                polyhedron.vertices[face[0]],
                polyhedron.vertices[face[1]],
                polyhedron.vertices[face[2]],
                polyhedron.vertices[vertex]
            );
            if(interior_side != 0) break;
        }
        if(interior_side >= 0)[[unlikely]]{
            throw std::domain_error(
                "Minkowski direct construction requires outward convex facets"
            );
        }
    }
    return result;
}

inline std::size_t direct_unique_support_vertex(
    const DirectOperand& operand,
    const Point3& normal,
    long double margin
){
    const std::vector<Point3>& vertices = operand.polyhedron->vertices;
    std::size_t support = 0;
    for(std::size_t vertex = 1; vertex < vertices.size(); ++vertex){
        const Geometry3DNormalizedDifference difference =
            geometry3d_normalized_difference(vertices[vertex], vertices[support]);
        if(dot(normal, difference.value) > 0.0L) support = vertex;
    }
    for(std::size_t vertex = 0; vertex < vertices.size(); ++vertex){
        if(vertex == support) continue;
        const Geometry3DNormalizedDifference difference =
            geometry3d_normalized_difference(vertices[support], vertices[vertex]);
        if(dot(normal, difference.value) <= margin)[[unlikely]]{
            throw std::domain_error(
                "Minkowski direct construction requires unique support vertices"
            );
        }
    }
    return support;
}

enum class DirectConeLocation: unsigned char{
    outside,
    boundary,
    inside,
};

inline DirectConeLocation direct_edge_cone_location(
    const DirectOperand& operand,
    const DirectEdge& edge,
    const Point3& normal,
    long double margin
){
    const Point3& first = operand.facets[edge.facets[0]].normal;
    const Point3& second = operand.facets[edge.facets[1]].normal;
    const long double mutual = dot(first, second);
    const long double determinant = std::max(
        0.0L, 1.0L - mutual * mutual
    );
    const long double first_coefficient = (
        dot(normal, first) - mutual * dot(normal, second)
    ) / determinant;
    const long double second_coefficient = (
        dot(normal, second) - mutual * dot(normal, first)
    ) / determinant;
    if(first_coefficient > margin && second_coefficient > margin){
        return DirectConeLocation::inside;
    }
    if(first_coefficient >= -margin && second_coefficient >= -margin){
        return DirectConeLocation::boundary;
    }
    return DirectConeLocation::outside;
}

struct DirectBuilder{
    const DirectOperand& first;
    const DirectOperand& second;
    ConvexPolyhedron3 result{3, {}, {}};
    std::unordered_map<
        DirectVertexPair,
        std::size_t,
        DirectVertexPairHash
    > vertex_indices;
    std::unordered_map<
        DirectPointKey,
        DirectVertexPair,
        DirectPointKeyHash
    > coordinate_owners;

    DirectBuilder(
        const DirectOperand& first_operand,
        const DirectOperand& second_operand
    ): first(first_operand), second(second_operand){}

    std::size_t vertex(std::size_t left, std::size_t right){
        const DirectVertexPair pair{left, right};
        const auto existing = vertex_indices.find(pair);
        if(existing != vertex_indices.end()) return existing->second;
        const Point3 point = first.polyhedron->vertices[left]
            + second.polyhedron->vertices[right];
        if(!geometry3d_is_finite(point))[[unlikely]]{
            throw std::overflow_error(
                "Minkowski direct construction produced a non-finite vertex"
            );
        }
        const DirectPointKey key{point.x, point.y, point.z};
        const auto [owner, unique] = coordinate_owners.emplace(key, pair);
        if(!unique && !(owner->second == pair))[[unlikely]]{
            throw std::domain_error(
                "Minkowski direct construction requires unique vertex sums"
            );
        }
        const std::size_t index = result.vertices.size();
        result.vertices.push_back(point);
        vertex_indices.emplace(pair, index);
        return index;
    }

    void polygon(const std::vector<DirectVertexPair>& boundary){
        if(boundary.size() < 3)[[unlikely]]{
            throw std::logic_error("Minkowski direct facet is degenerate");
        }
        const std::size_t anchor = vertex(
            boundary[0].first, boundary[0].second
        );
        for(std::size_t index = 1; index + 1 < boundary.size(); ++index){
            const std::size_t second_vertex = vertex(
                boundary[index].first, boundary[index].second
            );
            const std::size_t third_vertex = vertex(
                boundary[index + 1].first, boundary[index + 1].second
            );
            if(anchor == second_vertex || second_vertex == third_vertex
                || third_vertex == anchor)[[unlikely]]{
                throw std::domain_error(
                    "Minkowski direct construction produced a degenerate facet"
                );
            }
            result.faces.push_back({anchor, second_vertex, third_vertex});
        }
    }
};

struct DirectOutputEdgeUse{
    std::size_t count = 0;
    int orientation_sum = 0;
};

inline void validate_direct_result(const ConvexPolyhedron3& result){
    if(result.vertices.size() < 4 || result.faces.size() < 4)[[unlikely]]{
        throw std::domain_error(
            "Minkowski direct construction did not produce a 3D boundary"
        );
    }
    std::vector<bool> used(result.vertices.size(), false);
    std::unordered_map<
        DirectEdgeKey,
        DirectOutputEdgeUse,
        DirectEdgeKeyHash
    > edges;
    edges.reserve(result.faces.size() * 2 + 1);
    std::unordered_set<
        std::array<std::size_t, 3>,
        DirectTriangleHash
    > triangles;
    triangles.reserve(result.faces.size() * 2 + 1);
    for(const auto& face: result.faces){
        std::array<std::size_t, 3> sorted = face;
        std::sort(sorted.begin(), sorted.end());
        if(sorted[0] == sorted[1] || sorted[1] == sorted[2]
            || !triangles.insert(sorted).second)[[unlikely]]{
            throw std::domain_error(
                "Minkowski direct construction produced duplicate triangles"
            );
        }
        for(std::size_t local = 0; local < 3; ++local){
            used[face[local]] = true;
            const std::size_t from = face[local];
            const std::size_t to = face[(local + 1) % 3];
            DirectOutputEdgeUse& incidence =
                edges[direct_edge_key(from, to)];
            ++incidence.count;
            incidence.orientation_sum += from < to ? 1 : -1;
        }
    }
    for(const auto& [edge, incidence]: edges){
        static_cast<void>(edge);
        if(incidence.count != 2 || incidence.orientation_sum != 0)[[unlikely]]{
            throw std::domain_error(
                "Minkowski direct construction failed the manifold check"
            );
        }
    }
    if(std::find(used.begin(), used.end(), false) != used.end()
        || result.vertices.size() + result.faces.size() != edges.size() + 2)
        [[unlikely]]{
        throw std::domain_error(
            "Minkowski direct construction failed the sphere topology check"
        );
    }
}

inline ConvexPolyhedron3 direct_sum(
    const ConvexPolyhedron3& first_polyhedron,
    const ConvexPolyhedron3& second_polyhedron,
    long double margin
){
    if(!std::isfinite(margin) || margin <= 0.0L || margin >= 0.01L)
        [[unlikely]]{
        throw std::invalid_argument(
            "Minkowski direct construction margin must be in (0, 0.01)"
        );
    }
    const DirectOperand first = prepare_direct_operand(
        first_polyhedron, margin
    );
    const DirectOperand second = prepare_direct_operand(
        second_polyhedron, margin
    );
    DirectBuilder builder{first, second};
    builder.vertex_indices.reserve(
        first.polyhedron->vertices.size() + second.polyhedron->vertices.size()
    );
    builder.coordinate_owners.reserve(
        first.polyhedron->vertices.size() + second.polyhedron->vertices.size()
    );

    for(const DirectFacet& facet: first.facets){
        const std::size_t support = direct_unique_support_vertex(
            second, facet.normal, margin
        );
        std::vector<DirectVertexPair> boundary;
        boundary.reserve(facet.boundary.size());
        for(const std::size_t vertex: facet.boundary){
            boundary.push_back({vertex, support});
        }
        builder.polygon(boundary);
    }
    for(const DirectFacet& facet: second.facets){
        const std::size_t support = direct_unique_support_vertex(
            first, facet.normal, margin
        );
        std::vector<DirectVertexPair> boundary;
        boundary.reserve(facet.boundary.size());
        for(const std::size_t vertex: facet.boundary){
            boundary.push_back({support, vertex});
        }
        builder.polygon(boundary);
    }

    for(const DirectEdge& first_edge: first.edges){
        for(const DirectEdge& second_edge: second.edges){
            Point3 normal = cross(first_edge.direction, second_edge.direction);
            const long double sine = std::hypot(
                normal.x, normal.y, normal.z
            );
            if(!std::isfinite(sine) || sine * sine <= margin)[[unlikely]]{
                throw std::domain_error(
                    "Minkowski direct construction requires transverse edge directions"
                );
            }
            normal /= sine;
            DirectConeLocation first_location = direct_edge_cone_location(
                first, first_edge, normal, margin
            );
            DirectConeLocation second_location = direct_edge_cone_location(
                second, second_edge, normal, margin
            );
            bool reverse = false;
            if(first_location == DirectConeLocation::outside
                || second_location == DirectConeLocation::outside){
                normal = -normal;
                first_location = direct_edge_cone_location(
                    first, first_edge, normal, margin
                );
                second_location = direct_edge_cone_location(
                    second, second_edge, normal, margin
                );
                reverse = true;
            }
            if(first_location == DirectConeLocation::outside
                || second_location == DirectConeLocation::outside){
                continue;
            }
            if(first_location != DirectConeLocation::inside
                || second_location != DirectConeLocation::inside)[[unlikely]]{
                throw std::domain_error(
                    "Minkowski direct construction encountered a non-transverse normal fan"
                );
            }
            if(!reverse){
                builder.polygon({
                    {first_edge.first, second_edge.first},
                    {first_edge.second, second_edge.first},
                    {first_edge.second, second_edge.second},
                    {first_edge.first, second_edge.second},
                });
            }else{
                builder.polygon({
                    {first_edge.first, second_edge.first},
                    {first_edge.first, second_edge.second},
                    {first_edge.second, second_edge.second},
                    {first_edge.second, second_edge.first},
                });
            }
        }
    }
    validate_direct_result(builder.result);
    return builder.result;
}

}  // namespace minkowski_sum_3d_detail

inline ConvexPolyhedron3 minkowski_sum_3d_with_seed(
    const ConvexPolyhedron3& first,
    const ConvexPolyhedron3& second,
    std::uint64_t random_seed
){
    return convex_hull_3d_with_seed(
        minkowski_sum_3d_detail::candidate_points(first, second),
        random_seed
    );
}

inline ConvexPolyhedron3 minkowski_sum_3d(
    const ConvexPolyhedron3& first,
    const ConvexPolyhedron3& second
){
    return convex_hull_3d(
        minkowski_sum_3d_detail::candidate_points(first, second)
    );
}

inline ConvexPolyhedron3 minkowski_sum_3d_direct(
    const ConvexPolyhedron3& first,
    const ConvexPolyhedron3& second,
    long double general_position_margin = 1.0e-9L
){
    return minkowski_sum_3d_detail::direct_sum(
        first, second, general_position_margin
    );
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SHAPE_MINKOWSKI_SUM_3D_HPP_INCLUDED
