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
#include "convex_polyhedron_facets.hpp"
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

using ExactVector3 = std::array<Exact, 3>;

inline Exact exact_determinant(
    const ExactVector3& first,
    const ExactVector3& second,
    const ExactVector3& third
){
    using geometry3d_adaptive_detail::add;
    using geometry3d_adaptive_detail::multiply;
    using geometry3d_adaptive_detail::subtract;
    const Exact positive = add(
        multiply(first[0], subtract(
            multiply(second[1], third[2]),
            multiply(second[2], third[1])
        )),
        multiply(first[2], subtract(
            multiply(second[0], third[1]),
            multiply(second[1], third[0])
        ))
    );
    return subtract(
        positive,
        multiply(first[1], subtract(
            multiply(second[0], third[2]),
            multiply(second[2], third[0])
        ))
    );
}

inline bool exact_zero_vector(const ExactVector3& vector){
    return geometry3d_adaptive_detail::sign(vector[0]) == 0
        && geometry3d_adaptive_detail::sign(vector[1]) == 0
        && geometry3d_adaptive_detail::sign(vector[2]) == 0;
}

inline bool cone_contains(
    const ExactVector3& first,
    const ExactVector3& second,
    const ExactVector3& third,
    const ExactVector3& query
){
    const int orientation = geometry3d_adaptive_detail::sign(
        exact_determinant(first, second, third)
    );
    if(orientation == 0) return false;
    const int first_weight = geometry3d_adaptive_detail::sign(
        exact_determinant(query, second, third)
    );
    const int second_weight = geometry3d_adaptive_detail::sign(
        exact_determinant(first, query, third)
    );
    const int third_weight = geometry3d_adaptive_detail::sign(
        exact_determinant(first, second, query)
    );
    if(orientation > 0){
        return first_weight >= 0 && second_weight >= 0
            && third_weight >= 0;
    }
    return first_weight <= 0 && second_weight <= 0
        && third_weight <= 0;
}

inline std::pair<std::size_t, std::size_t> radial_edge_key(
    std::size_t first,
    std::size_t second
){
    if(second < first) std::swap(first, second);
    return {first, second};
}

struct RadialTriangle{
    std::array<std::size_t, 3> vertices{};
    std::size_t label = std::numeric_limits<std::size_t>::max();
    std::vector<std::size_t> children;
};

struct RadialLevel{
    std::vector<RadialTriangle> triangles;
    std::size_t vertex_count = 0;
};

struct RadialLocation{
    std::size_t triangle = std::numeric_limits<std::size_t>::max();
    std::array<int, 3> barycentric_signs{};
};

class RadialHierarchy{
    static constexpr std::size_t no_index =
        std::numeric_limits<std::size_t>::max();
    static constexpr std::size_t maximum_removed_degree = 11;
    static constexpr std::size_t coarse_vertex_limit = 24;

    std::vector<ExactVector3> directions_;
    std::vector<RadialLevel> levels_;
    std::vector<std::size_t> fine_vertex_labels_;
    std::map<std::pair<std::size_t, std::size_t>, std::size_t>
        fine_edge_labels_;
    std::size_t maximum_branching_ = 0;

    std::array<int, 3> barycentric_signs(
        const RadialTriangle& triangle,
        const ExactVector3& query
    ) const;

    bool contains_direction(
        const RadialTriangle& triangle,
        const ExactVector3& query
    ) const;

    std::size_t validate_level(const RadialLevel& level) const;

    std::vector<std::size_t> neighbor_cycle(
        std::size_t removed,
        const RadialLevel& level,
        const std::vector<std::size_t>& incident
    ) const;

    std::vector<std::array<std::size_t, 3>> triangulate_positive_polygon(
        std::vector<std::size_t> polygon,
        const std::map<std::pair<std::size_t, std::size_t>, std::size_t>&
            original_edges
    ) const;

    bool append_coarser_level();

public:
    RadialHierarchy() = default;

    RadialHierarchy(
        std::vector<ExactVector3> directions,
        const std::vector<std::array<std::size_t, 3>>& triangles,
        const std::vector<std::size_t>& labels
    );

    bool empty() const noexcept{ return levels_.empty(); }
    std::size_t depth() const noexcept{ return levels_.size(); }
    std::size_t maximum_branching() const noexcept{
        return maximum_branching_;
    }
    const std::vector<RadialLevel>& levels() const noexcept{ return levels_; }

    RadialLocation locate(const ExactVector3& query) const;

    const RadialTriangle& finest_triangle(std::size_t index) const{
        return levels_.front().triangles[index];
    }

    std::size_t fine_vertex_label(std::size_t vertex) const{
        return fine_vertex_labels_.at(vertex);
    }

    std::size_t fine_edge_label(std::size_t first, std::size_t second) const{
        const auto iterator = fine_edge_labels_.find(
            radial_edge_key(first, second)
        );
        return iterator == fine_edge_labels_.end() ? no_index
            : iterator->second;
    }
};

inline std::array<int, 3> RadialHierarchy::barycentric_signs(
    const RadialTriangle& triangle,
    const ExactVector3& query
) const{
    const ExactVector3& first = directions_[triangle.vertices[0]];
    const ExactVector3& second = directions_[triangle.vertices[1]];
    const ExactVector3& third = directions_[triangle.vertices[2]];
    return {
        geometry3d_adaptive_detail::sign(
            exact_determinant(query, second, third)
        ),
        geometry3d_adaptive_detail::sign(
            exact_determinant(first, query, third)
        ),
        geometry3d_adaptive_detail::sign(
            exact_determinant(first, second, query)
        ),
    };
}

inline bool RadialHierarchy::contains_direction(
    const RadialTriangle& triangle,
    const ExactVector3& query
) const{
    const auto signs = barycentric_signs(triangle, query);
    return signs[0] >= 0 && signs[1] >= 0 && signs[2] >= 0;
}

inline std::size_t RadialHierarchy::validate_level(
    const RadialLevel& level
) const{
    if(level.triangles.empty()){
        throw std::logic_error("empty radial hierarchy level");
    }
    std::vector<char> used(directions_.size(), 0);
    std::map<std::pair<std::size_t, std::size_t>, std::size_t> edge_count;
    for(const RadialTriangle& triangle: level.triangles){
        for(const std::size_t vertex: triangle.vertices){
            if(vertex >= directions_.size()){
                throw std::logic_error("radial hierarchy vertex index");
            }
            used[vertex] = 1;
        }
        if(geometry3d_adaptive_detail::sign(exact_determinant(
            directions_[triangle.vertices[0]],
            directions_[triangle.vertices[1]],
            directions_[triangle.vertices[2]]
        )) <= 0){
            throw std::domain_error(
                "radial hierarchy has a non-positive cone triangle"
            );
        }
        for(std::size_t edge = 0; edge < 3; ++edge){
            ++edge_count[radial_edge_key(
                triangle.vertices[edge],
                triangle.vertices[(edge + 1) % 3]
            )];
        }
    }
    for(const auto& [edge, count]: edge_count){
        static_cast<void>(edge);
        if(count != 2){
            throw std::domain_error(
                "radial hierarchy is not a closed triangulation"
            );
        }
    }
    const std::size_t vertex_count = static_cast<std::size_t>(
        std::count(used.begin(), used.end(), char{1})
    );
    if(vertex_count < 4
        || level.triangles.size() != vertex_count * 2 - 4){
        throw std::domain_error(
            "radial hierarchy violates the sphere Euler invariant"
        );
    }
    return vertex_count;
}

inline std::vector<std::size_t> RadialHierarchy::neighbor_cycle(
    std::size_t removed,
    const RadialLevel& level,
    const std::vector<std::size_t>& incident
) const{
    std::map<std::size_t, std::vector<std::size_t>> cycle_adjacency;
    for(const std::size_t triangle_index: incident){
        const auto& vertices = level.triangles[triangle_index].vertices;
        std::array<std::size_t, 2> other{};
        std::size_t count = 0;
        for(const std::size_t vertex: vertices){
            if(vertex != removed) other[count++] = vertex;
        }
        if(count != 2){
            throw std::logic_error("invalid radial vertex incidence");
        }
        cycle_adjacency[other[0]].push_back(other[1]);
        cycle_adjacency[other[1]].push_back(other[0]);
    }
    if(cycle_adjacency.size() != incident.size()){
        throw std::domain_error("non-simple radial vertex link");
    }
    for(auto& [vertex, adjacent]: cycle_adjacency){
        static_cast<void>(vertex);
        std::sort(adjacent.begin(), adjacent.end());
        adjacent.erase(
            std::unique(adjacent.begin(), adjacent.end()), adjacent.end()
        );
        if(adjacent.size() != 2){
            throw std::domain_error("radial vertex link is not a cycle");
        }
    }

    std::vector<std::size_t> cycle;
    cycle.reserve(cycle_adjacency.size());
    const std::size_t start = cycle_adjacency.begin()->first;
    std::size_t previous = no_index;
    std::size_t current = start;
    do{
        cycle.push_back(current);
        const auto& adjacent = cycle_adjacency.at(current);
        const std::size_t next =
            adjacent[0] != previous ? adjacent[0] : adjacent[1];
        previous = current;
        current = next;
        if(cycle.size() > cycle_adjacency.size()){
            throw std::domain_error("radial vertex link repeats a vertex");
        }
    }while(current != start);
    if(cycle.size() != cycle_adjacency.size()){
        throw std::domain_error("radial vertex link is disconnected");
    }

    if(geometry3d_adaptive_detail::sign(exact_determinant(
        directions_[removed], directions_[cycle[0]], directions_[cycle[1]]
    )) < 0){
        std::reverse(cycle.begin() + 1, cycle.end());
    }
    if(geometry3d_adaptive_detail::sign(exact_determinant(
        directions_[removed], directions_[cycle[0]], directions_[cycle[1]]
    )) <= 0){
        throw std::domain_error("degenerate radial vertex link");
    }
    return cycle;
}

inline std::vector<std::array<std::size_t, 3>>
RadialHierarchy::triangulate_positive_polygon(
    std::vector<std::size_t> polygon,
    const std::map<std::pair<std::size_t, std::size_t>, std::size_t>&
        original_edges
) const{
    std::vector<std::array<std::size_t, 3>> result;
    if(polygon.size() < 3){
        throw std::logic_error("radial polygon has fewer than three vertices");
    }
    result.reserve(polygon.size() - 2);
    while(polygon.size() > 3){
        bool found = false;
        for(std::size_t index = 0; index < polygon.size(); ++index){
            const std::size_t previous =
                polygon[(index + polygon.size() - 1) % polygon.size()];
            const std::size_t current = polygon[index];
            const std::size_t next =
                polygon[(index + 1) % polygon.size()];
            if(geometry3d_adaptive_detail::sign(exact_determinant(
                directions_[previous],
                directions_[current],
                directions_[next]
            )) <= 0) continue;
            if(original_edges.contains(radial_edge_key(previous, next))){
                continue;
            }
            bool covers_vertex = false;
            for(const std::size_t vertex: polygon){
                if(vertex == previous || vertex == current || vertex == next){
                    continue;
                }
                if(cone_contains(
                    directions_[previous],
                    directions_[current],
                    directions_[next],
                    directions_[vertex]
                )){
                    covers_vertex = true;
                    break;
                }
            }
            if(covers_vertex) continue;
            result.push_back({previous, current, next});
            polygon.erase(
                polygon.begin() + static_cast<std::ptrdiff_t>(index)
            );
            found = true;
            break;
        }
        if(!found){
            throw std::domain_error(
                "radial vertex link cannot be triangulated exactly"
            );
        }
    }
    if(geometry3d_adaptive_detail::sign(exact_determinant(
        directions_[polygon[0]],
        directions_[polygon[1]],
        directions_[polygon[2]]
    )) <= 0){
        throw std::domain_error("degenerate final radial triangle");
    }
    result.push_back({polygon[0], polygon[1], polygon[2]});
    return result;
}

inline bool RadialHierarchy::append_coarser_level(){
    const RadialLevel& fine = levels_.back();
    const std::size_t active_count = fine.vertex_count;
    if(active_count <= coarse_vertex_limit) return false;

    std::vector<std::vector<std::size_t>> incident(directions_.size());
    std::vector<std::vector<std::size_t>> neighbors(directions_.size());
    std::map<std::pair<std::size_t, std::size_t>, std::size_t> edge_count;
    for(std::size_t triangle_index = 0;
        triangle_index < fine.triangles.size(); ++triangle_index){
        const auto& vertices = fine.triangles[triangle_index].vertices;
        for(const std::size_t vertex: vertices){
            incident[vertex].push_back(triangle_index);
        }
        for(std::size_t edge = 0; edge < 3; ++edge){
            const std::size_t first = vertices[edge];
            const std::size_t second = vertices[(edge + 1) % 3];
            neighbors[first].push_back(second);
            neighbors[second].push_back(first);
            ++edge_count[radial_edge_key(first, second)];
        }
    }
    for(auto& adjacent: neighbors){
        std::sort(adjacent.begin(), adjacent.end());
        adjacent.erase(
            std::unique(adjacent.begin(), adjacent.end()), adjacent.end()
        );
    }

    std::vector<std::size_t> candidates;
    candidates.reserve(active_count);
    for(std::size_t vertex = 0; vertex < directions_.size(); ++vertex){
        if(!incident[vertex].empty()
            && neighbors[vertex].size() <= maximum_removed_degree){
            candidates.push_back(vertex);
        }
    }
    if(candidates.size() <= active_count / 2){
        throw std::logic_error(
            "radial low-degree candidate fraction invariant failed"
        );
    }
    std::vector<char> blocked(directions_.size(), 0);
    std::vector<char> removed(directions_.size(), 0);
    std::vector<std::size_t> independent;
    for(const std::size_t vertex: candidates){
        if(blocked[vertex]) continue;
        removed[vertex] = 1;
        independent.push_back(vertex);
        blocked[vertex] = 1;
        for(const std::size_t adjacent: neighbors[vertex]){
            blocked[adjacent] = 1;
        }
    }
    const std::size_t independent_divisor = maximum_removed_degree + 1;
    const std::size_t required_independent =
        candidates.size() / independent_divisor
        + static_cast<std::size_t>(
            candidates.size() % independent_divisor != 0
        );
    if(independent.size() < required_independent){
        throw std::logic_error(
            "radial low-degree independent set invariant failed"
        );
    }

    RadialLevel coarse;
    coarse.vertex_count = active_count - independent.size();
    if(independent.size() * 2 > fine.triangles.size()){
        throw std::logic_error("radial face count underflow");
    }
    coarse.triangles.reserve(
        fine.triangles.size() - independent.size() * 2
    );
    for(std::size_t triangle_index = 0;
        triangle_index < fine.triangles.size(); ++triangle_index){
        const auto& triangle = fine.triangles[triangle_index];
        if(removed[triangle.vertices[0]]
            || removed[triangle.vertices[1]]
            || removed[triangle.vertices[2]]) continue;
        RadialTriangle unchanged;
        unchanged.vertices = triangle.vertices;
        unchanged.children.push_back(triangle_index);
        maximum_branching_ = std::max(maximum_branching_, std::size_t{1});
        coarse.triangles.push_back(std::move(unchanged));
    }
    for(const std::size_t vertex: independent){
        if(incident[vertex].size() < 3
            || incident[vertex].size() > maximum_removed_degree){
            throw std::logic_error("invalid removable radial degree");
        }
        const auto cycle = neighbor_cycle(vertex, fine, incident[vertex]);
        const auto cap = triangulate_positive_polygon(cycle, edge_count);
        for(const auto& vertices: cap){
            RadialTriangle triangle;
            triangle.vertices = vertices;
            triangle.children = incident[vertex];
            maximum_branching_ = std::max(
                maximum_branching_, triangle.children.size()
            );
            coarse.triangles.push_back(std::move(triangle));
        }
    }
    if(coarse.triangles.size() + independent.size() * 2
        != fine.triangles.size()){
        throw std::logic_error("radial face reduction invariant failed");
    }
    for(const RadialTriangle& triangle: coarse.triangles){
        if(triangle.children.empty()
            || triangle.children.size() > maximum_removed_degree){
            throw std::logic_error(
                "invalid radial hierarchy child count"
            );
        }
        for(const std::size_t child: triangle.children){
            if(child >= fine.triangles.size()){
                throw std::logic_error("radial hierarchy child index");
            }
        }
    }
    const std::size_t verified_count = validate_level(coarse);
    if(verified_count != coarse.vertex_count){
        throw std::logic_error("radial vertex reduction invariant failed");
    }
    levels_.push_back(std::move(coarse));
    return true;
}

inline RadialHierarchy::RadialHierarchy(
    std::vector<ExactVector3> directions,
    const std::vector<std::array<std::size_t, 3>>& triangles,
    const std::vector<std::size_t>& labels
): directions_(std::move(directions)){
    if(triangles.size() != labels.size()){
        throw std::logic_error("radial triangle label count");
    }
    for(const ExactVector3& direction: directions_){
        if(exact_zero_vector(direction)){
            throw std::domain_error("zero radial hierarchy direction");
        }
    }
    RadialLevel finest;
    finest.triangles.reserve(triangles.size());
    for(std::size_t index = 0; index < triangles.size(); ++index){
        if(labels[index] == no_index){
            throw std::logic_error("radial triangle has no label");
        }
        RadialTriangle triangle;
        triangle.vertices = triangles[index];
        triangle.label = labels[index];
        finest.triangles.push_back(std::move(triangle));
    }
    finest.vertex_count = validate_level(finest);
    if(finest.vertex_count != directions_.size()){
        throw std::domain_error("unused finest radial vertex");
    }
    levels_.push_back(std::move(finest));

    fine_vertex_labels_.assign(directions_.size(), no_index);
    for(const RadialTriangle& triangle: levels_.front().triangles){
        for(const std::size_t vertex: triangle.vertices){
            fine_vertex_labels_[vertex] = std::min(
                fine_vertex_labels_[vertex], triangle.label
            );
        }
        for(std::size_t edge = 0; edge < 3; ++edge){
            const auto key = radial_edge_key(
                triangle.vertices[edge],
                triangle.vertices[(edge + 1) % 3]
            );
            auto [iterator, inserted] =
                fine_edge_labels_.try_emplace(key, triangle.label);
            if(!inserted){
                iterator->second = std::min(
                    iterator->second, triangle.label
                );
            }
        }
    }
    while(append_coarser_level()){}
}

inline RadialLocation RadialHierarchy::locate(
    const ExactVector3& query
) const{
    if(levels_.empty() || exact_zero_vector(query)){
        throw std::logic_error("invalid radial hierarchy query");
    }
    std::size_t triangle = no_index;
    const RadialLevel& coarsest = levels_.back();
    for(std::size_t index = 0; index < coarsest.triangles.size(); ++index){
        if(contains_direction(coarsest.triangles[index], query)){
            triangle = index;
            break;
        }
    }
    if(triangle == no_index){
        throw std::logic_error("radial hierarchy does not cover direction");
    }
    for(std::size_t level = levels_.size() - 1; level > 0; --level){
        const auto& children = levels_[level].triangles[triangle].children;
        std::size_t child = no_index;
        for(const std::size_t candidate: children){
            if(candidate >= levels_[level - 1].triangles.size()){
                throw std::logic_error("radial hierarchy child index");
            }
            if(contains_direction(
                levels_[level - 1].triangles[candidate], query
            )){
                child = candidate;
                break;
            }
        }
        if(child == no_index){
            throw std::logic_error("radial hierarchy child coverage failed");
        }
        triangle = child;
    }
    return {
        triangle,
        barycentric_signs(levels_.front().triangles[triangle], query),
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


    ConvexPolyhedron3 polyhedron_;
    std::size_t leaf_capacity_ = 8;
    std::vector<std::size_t> vertex_order_;
    std::vector<VertexNode> vertex_nodes_;
    std::vector<std::size_t> closest_face_order_;
    std::vector<FaceNode> face_nodes_;
    std::vector<convex_polyhedron_query_hierarchy_3d_detail::ExactPlane>
        exact_planes_;
    convex_polyhedron_query_hierarchy_3d_detail::ExactVector3 vertex_sum_{};
    Detail vertex_count_factor_{};
    convex_polyhedron_query_hierarchy_3d_detail::RadialHierarchy
        membership_hierarchy_;
    convex_polyhedron_query_hierarchy_3d_detail::RadialHierarchy
        support_hierarchy_;
    std::vector<std::size_t> support_facet_minimum_vertex_;
    std::map<std::pair<std::size_t, std::size_t>, std::size_t>
        support_edge_minimum_vertex_;

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

    convex_polyhedron_query_hierarchy_3d_detail::ExactVector3
        centered_direction(const Point3& point) const;

    void build_membership_hierarchy();
    void build_support_hierarchy();

    std::size_t logarithmic_support_index(
        const convex_polyhedron_query_hierarchy_3d_detail::ExactVector3&
            direction
    ) const;

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
        if(polyhedron_.affine_dimension != 3){
            vertex_order_.resize(polyhedron_.vertices.size());
            std::iota(
                vertex_order_.begin(), vertex_order_.end(), std::size_t{0}
            );
            vertex_nodes_.clear();
            vertex_nodes_.reserve(polyhedron_.vertices.size() * 2);
            build_vertex_node(0, vertex_order_.size());
        }

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
        for(const auto& face: polyhedron_.faces){
            exact_planes_.push_back(
                convex_polyhedron_query_hierarchy_3d_detail::exact_plane(
                    polyhedron_.vertices[face[0]], polyhedron_.vertices[face[1]],
                    polyhedron_.vertices[face[2]]
                )
            );
        }
        build_membership_hierarchy();
        build_support_hierarchy();
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
    std::size_t support_hierarchy_depth() const noexcept{
        return support_hierarchy_.depth();
    }
    std::size_t contains_hierarchy_depth() const noexcept{
        return membership_hierarchy_.depth();
    }
    std::size_t hierarchy_maximum_branching() const noexcept{
        return std::max(
            support_hierarchy_.maximum_branching(),
            membership_hierarchy_.maximum_branching()
        );
    }

    Point3 support_point(const Point3& direction) const{
        using namespace convex_polyhedron_query_hierarchy_3d_detail;
        if(!geometry3d_is_finite(direction)){
            throw std::invalid_argument("non-finite support direction");
        }
        if(direction.x == 0 && direction.y == 0 && direction.z == 0){
            throw std::invalid_argument("zero support direction");
        }
        const auto exact_direction = exact_point(direction);
        if(polyhedron_.affine_dimension == 3){
            return polyhedron_.vertices[logarithmic_support_index(exact_direction)];
        }
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
        const ExactVector3 direction = centered_direction(point);
        if(exact_zero_vector(direction)) return true;
        const RadialLocation location =
            membership_hierarchy_.locate(direction);
        const std::size_t face = membership_hierarchy_.finest_triangle(
            location.triangle
        ).label;
        if(face >= exact_planes_.size()){
            throw std::logic_error("membership hierarchy face label");
        }
        const ExactVector3 exact = exact_point(point);
        const std::array<Detail, 4> query{
            exact[0], exact[1], exact[2],
            geometry3d_adaptive_detail::exact_dyadic(1.0L),
        };
        return geometry3d_adaptive_detail::sign(
            exact_plane_value(exact_planes_[face], query)
        ) <= 0;
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

inline convex_polyhedron_query_hierarchy_3d_detail::ExactVector3
ConvexPolyhedronQueryHierarchy3D::centered_direction(
    const Point3& point
) const{
    using namespace convex_polyhedron_query_hierarchy_3d_detail;
    const ExactVector3 exact = exact_point(point);
    ExactVector3 result{};
    for(std::size_t axis = 0; axis < 3; ++axis){
        result[axis] = geometry3d_adaptive_detail::subtract(
            geometry3d_adaptive_detail::multiply(
                vertex_count_factor_, exact[axis]
            ),
            vertex_sum_[axis]
        );
    }
    return result;
}

inline void ConvexPolyhedronQueryHierarchy3D::build_membership_hierarchy(){
    using namespace convex_polyhedron_query_hierarchy_3d_detail;
    vertex_sum_ = {};
    for(const Point3& point: polyhedron_.vertices){
        const ExactVector3 exact = exact_point(point);
        for(std::size_t axis = 0; axis < 3; ++axis){
            vertex_sum_[axis] = geometry3d_adaptive_detail::add(
                vertex_sum_[axis], exact[axis]
            );
        }
    }
    vertex_count_factor_ = geometry3d_adaptive_detail::exact_dyadic(
        static_cast<long double>(polyhedron_.vertices.size())
    );

    std::vector<ExactVector3> directions;
    directions.reserve(polyhedron_.vertices.size());
    for(const Point3& point: polyhedron_.vertices){
        directions.push_back(centered_direction(point));
    }
    std::vector<std::size_t> labels(polyhedron_.faces.size());
    std::iota(labels.begin(), labels.end(), std::size_t{0});
    membership_hierarchy_ = RadialHierarchy(
        std::move(directions), polyhedron_.faces, labels
    );
}

inline void ConvexPolyhedronQueryHierarchy3D::build_support_hierarchy(){
    using namespace convex_polyhedron_query_hierarchy_3d_detail;
    const std::vector<ConvexPolyhedronFacet3> facets =
        convex_polyhedron_facets(polyhedron_);
    if(facets.size() < 4){
        throw std::domain_error(
            "three-dimensional convex polyhedron has fewer than four facets"
        );
    }

    std::vector<std::size_t> face_to_facet(
        polyhedron_.faces.size(), no_index
    );
    std::vector<ExactVector3> directions;
    directions.reserve(facets.size());
    support_facet_minimum_vertex_.assign(facets.size(), no_index);
    std::vector<std::vector<std::size_t>> vertex_facets(
        polyhedron_.vertices.size()
    );
    std::map<std::pair<std::size_t, std::size_t>, std::vector<std::size_t>>
        edge_facets;

    for(std::size_t facet = 0; facet < facets.size(); ++facet){
        if(facets[facet].triangles.empty()){
            throw std::logic_error("convex polyhedron facet has no triangle");
        }
        const auto& plane = exact_planes_[facets[facet].triangles.front()];
        directions.push_back({
            plane.coefficient[0],
            plane.coefficient[1],
            plane.coefficient[2],
        });
        for(const std::size_t face: facets[facet].triangles){
            if(face >= face_to_facet.size()
                || face_to_facet[face] != no_index){
                throw std::logic_error("invalid facet triangle partition");
            }
            face_to_facet[face] = facet;
            for(const std::size_t vertex: polyhedron_.faces[face]){
                support_facet_minimum_vertex_[facet] = std::min(
                    support_facet_minimum_vertex_[facet], vertex
                );
                vertex_facets[vertex].push_back(facet);
            }
        }
        if(facets[facet].boundary.size() < 3){
            throw std::domain_error("convex polyhedron facet boundary");
        }
        for(std::size_t edge = 0;
            edge < facets[facet].boundary.size(); ++edge){
            edge_facets[radial_edge_key(
                facets[facet].boundary[edge],
                facets[facet].boundary[
                    (edge + 1) % facets[facet].boundary.size()
                ]
            )].push_back(facet);
        }
    }
    if(std::find(face_to_facet.begin(), face_to_facet.end(), no_index)
        != face_to_facet.end()){
        throw std::logic_error("facet partition misses a triangle");
    }
    for(auto& incident: vertex_facets){
        std::sort(incident.begin(), incident.end());
        incident.erase(
            std::unique(incident.begin(), incident.end()), incident.end()
        );
    }

    std::vector<std::vector<std::pair<std::size_t, std::size_t>>>
        vertex_links(polyhedron_.vertices.size());
    support_edge_minimum_vertex_.clear();
    for(auto& [edge, incident]: edge_facets){
        std::sort(incident.begin(), incident.end());
        incident.erase(
            std::unique(incident.begin(), incident.end()), incident.end()
        );
        if(incident.size() != 2){
            throw std::domain_error(
                "geometric polyhedron edge has invalid facet incidence"
            );
        }
        const auto facet_pair = radial_edge_key(incident[0], incident[1]);
        for(const std::size_t vertex: {edge.first, edge.second}){
            vertex_links[vertex].push_back(facet_pair);
            auto [iterator, inserted] =
                support_edge_minimum_vertex_.try_emplace(
                    facet_pair, vertex
                );
            if(!inserted){
                iterator->second = std::min(iterator->second, vertex);
            }
        }
    }

    std::vector<std::array<std::size_t, 3>> polar_triangles;
    std::vector<std::size_t> polar_labels;
    for(std::size_t vertex = 0;
        vertex < polyhedron_.vertices.size(); ++vertex){
        const auto& incident = vertex_facets[vertex];
        if(incident.size() < 3) continue;

        std::map<std::size_t, std::vector<std::size_t>> adjacency;
        for(const auto& [first, second]: vertex_links[vertex]){
            adjacency[first].push_back(second);
            adjacency[second].push_back(first);
        }
        for(const std::size_t facet: incident){
            auto& adjacent = adjacency[facet];
            std::sort(adjacent.begin(), adjacent.end());
            adjacent.erase(
                std::unique(adjacent.begin(), adjacent.end()), adjacent.end()
            );
            if(adjacent.size() != 2){
                throw std::domain_error(
                    "incident facets do not form a vertex cycle"
                );
            }
        }

        std::vector<std::size_t> cycle;
        cycle.reserve(incident.size());
        const std::size_t start = *std::min_element(
            incident.begin(), incident.end()
        );
        std::size_t previous = no_index;
        std::size_t current = start;
        do{
            cycle.push_back(current);
            const auto& adjacent = adjacency.at(current);
            const std::size_t next =
                adjacent[0] != previous ? adjacent[0] : adjacent[1];
            previous = current;
            current = next;
            if(cycle.size() > incident.size()){
                throw std::domain_error("facet cycle repeats");
            }
        }while(current != start);
        if(cycle.size() != incident.size()){
            throw std::domain_error("facet cycle is disconnected");
        }

        int orientation = 0;
        for(std::size_t index = 1;
            index + 1 < cycle.size() && orientation == 0; ++index){
            orientation = geometry3d_adaptive_detail::sign(
                exact_determinant(
                    directions[cycle[0]],
                    directions[cycle[index]],
                    directions[cycle[index + 1]]
                )
            );
        }
        if(orientation == 0){
            throw std::domain_error("degenerate polar normal fan cell");
        }
        if(orientation < 0){
            std::reverse(cycle.begin() + 1, cycle.end());
        }
        for(std::size_t index = 1; index + 1 < cycle.size(); ++index){
            const std::array<std::size_t, 3> triangle{
                cycle[0], cycle[index], cycle[index + 1]
            };
            if(geometry3d_adaptive_detail::sign(exact_determinant(
                directions[triangle[0]],
                directions[triangle[1]],
                directions[triangle[2]]
            )) <= 0){
                throw std::domain_error(
                    "polar normal fan is not strictly triangulated"
                );
            }
            polar_triangles.push_back(triangle);
            polar_labels.push_back(vertex);
        }
    }
    support_hierarchy_ = RadialHierarchy(
        std::move(directions), polar_triangles, polar_labels
    );
}

inline std::size_t
ConvexPolyhedronQueryHierarchy3D::logarithmic_support_index(
    const convex_polyhedron_query_hierarchy_3d_detail::ExactVector3& direction
) const{
    using namespace convex_polyhedron_query_hierarchy_3d_detail;
    const RadialLocation location = support_hierarchy_.locate(direction);
    const RadialTriangle& triangle =
        support_hierarchy_.finest_triangle(location.triangle);
    std::size_t zero_count = 0;
    for(const int sign: location.barycentric_signs){
        if(sign < 0){
            throw std::logic_error("negative located polar barycentric weight");
        }
        if(sign == 0) ++zero_count;
    }

    std::size_t answer = no_index;
    if(zero_count >= 2){
        std::size_t corner = no_index;
        for(std::size_t index = 0; index < 3; ++index){
            if(location.barycentric_signs[index] != 0){
                corner = index;
                break;
            }
        }
        if(corner == no_index){
            throw std::logic_error("zero polar support direction");
        }
        answer = support_facet_minimum_vertex_.at(
            triangle.vertices[corner]
        );
    }else if(zero_count == 1){
        std::array<std::size_t, 2> edge{};
        std::size_t count = 0;
        for(std::size_t index = 0; index < 3; ++index){
            if(location.barycentric_signs[index] != 0){
                edge[count++] = triangle.vertices[index];
            }
        }
        if(count != 2){
            throw std::logic_error("invalid polar edge coordinates");
        }
        answer = support_hierarchy_.fine_edge_label(edge[0], edge[1]);
        const auto iterator = support_edge_minimum_vertex_.find(
            radial_edge_key(edge[0], edge[1])
        );
        if(iterator != support_edge_minimum_vertex_.end()){
            answer = std::min(answer, iterator->second);
        }
    }else{
        answer = triangle.label;
    }
    if(answer == no_index || answer >= polyhedron_.vertices.size()){
        throw std::logic_error("polar support label is missing");
    }
    return answer;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_QUERY_CONVEX_POLYHEDRON_QUERY_HIERARCHY_3D_HPP_INCLUDED
