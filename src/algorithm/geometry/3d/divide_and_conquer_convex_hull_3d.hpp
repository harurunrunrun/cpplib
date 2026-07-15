#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <map>
#include <set>
#include <stdexcept>
#include <utility>
#include <vector>

#include "adaptive_orient3d.hpp"
#include "convex_polyhedron3.hpp"
#include "convex_polyhedron_edges.hpp"
#include "convex_polyhedron_facets.hpp"
#include "is_finite.hpp"

namespace divide_and_conquer_convex_hull_3d_detail{

inline bool same_point(const Point3& left, const Point3& right){
    return left.x == right.x && left.y == right.y && left.z == right.z;
}

inline int exact_turn(
    long double origin_x,
    long double origin_y,
    long double first_x,
    long double first_y,
    long double second_x,
    long double second_y
){
    using namespace geometry3d_adaptive_detail;
    const ExactDyadic first_dx = subtract(
        exact_dyadic(first_x), exact_dyadic(origin_x)
    );
    const ExactDyadic first_dy = subtract(
        exact_dyadic(first_y), exact_dyadic(origin_y)
    );
    const ExactDyadic second_dx = subtract(
        exact_dyadic(second_x), exact_dyadic(origin_x)
    );
    const ExactDyadic second_dy = subtract(
        exact_dyadic(second_y), exact_dyadic(origin_y)
    );
    return sign(subtract(
        multiply(first_dx, second_dy), multiply(first_dy, second_dx)
    ));
}

inline bool non_collinear(
    const Point3& first,
    const Point3& second,
    const Point3& third
){
    return exact_turn(
        first.x, first.y, second.x, second.y, third.x, third.y
    ) != 0 || exact_turn(
        first.x, first.z, second.x, second.z, third.x, third.z
    ) != 0 || exact_turn(
        first.y, first.z, second.y, second.z, third.y, third.z
    ) != 0;
}

inline std::vector<std::array<std::size_t, 2>> geometric_edges(
    const ConvexPolyhedron3& hull
){
    if(hull.affine_dimension == 1 && hull.vertices.size() == 2){
        return {{{0, 1}}};
    }
    if(hull.affine_dimension < 2) return {};
    return convex_polyhedron_edges(hull);
}

struct ProjectedPoint{
    long double first = 0.0L;
    long double second = 0.0L;
    std::size_t index = 0;
};

inline int projected_turn(
    const ProjectedPoint& origin,
    const ProjectedPoint& first,
    const ProjectedPoint& second
){
    return exact_turn(
        origin.first, origin.second, first.first, first.second,
        second.first, second.second
    );
}

inline std::vector<std::size_t> planar_boundary(
    const std::vector<Point3>& points,
    const std::vector<std::size_t>& coplanar,
    const std::array<std::size_t, 3>& plane
){
    const Point3& origin = points[plane[0]];
    const Point3& first = points[plane[1]];
    const Point3& second = points[plane[2]];
    std::size_t dropped = 2;
    if(exact_turn(
        origin.y, origin.z, first.y, first.z, second.y, second.z
    ) != 0){
        dropped = 0;
    }else if(exact_turn(
        origin.x, origin.z, first.x, first.z, second.x, second.z
    ) != 0){
        dropped = 1;
    }

    std::vector<ProjectedPoint> projected;
    projected.reserve(coplanar.size());
    for(std::size_t index: coplanar){
        const Point3& point = points[index];
        if(dropped == 0) projected.push_back({point.y, point.z, index});
        if(dropped == 1) projected.push_back({point.x, point.z, index});
        if(dropped == 2) projected.push_back({point.x, point.y, index});
    }
    std::sort(projected.begin(), projected.end(), [](const auto& left,
                                                     const auto& right){
        if(left.first != right.first) return left.first < right.first;
        if(left.second != right.second) return left.second < right.second;
        return left.index < right.index;
    });

    std::vector<ProjectedPoint> boundary;
    boundary.reserve(projected.size() * 2);
    for(const ProjectedPoint& point: projected){
        while(boundary.size() >= 2 && projected_turn(
            boundary[boundary.size() - 2], boundary.back(), point
        ) <= 0) boundary.pop_back();
        boundary.push_back(point);
    }
    const std::size_t lower_size = boundary.size();
    for(std::size_t index = projected.size(); index-- > 0;){
        const ProjectedPoint& point = projected[index];
        while(boundary.size() > lower_size && projected_turn(
            boundary[boundary.size() - 2], boundary.back(), point
        ) <= 0) boundary.pop_back();
        boundary.push_back(point);
    }
    if(boundary.size() > 1) boundary.pop_back();

    std::vector<std::size_t> result;
    result.reserve(boundary.size());
    for(const ProjectedPoint& point: boundary) result.push_back(point.index);
    return result;
}

inline ConvexPolyhedron3 lower_dimensional_hull(
    const std::vector<Point3>& points
){
    if(points.size() <= 1){
        return {points.empty() ? -1 : 0, points, {}};
    }
    std::size_t third = points.size();
    for(std::size_t index = 2; index < points.size(); ++index){
        if(non_collinear(points[0], points[1], points[index])){
            third = index;
            break;
        }
    }
    if(third == points.size()) return {1, {points.front(), points.back()}, {}};

    std::vector<std::size_t> all(points.size());
    for(std::size_t index = 0; index < points.size(); ++index) all[index] = index;
    const std::array<std::size_t, 3> plane{0, 1, third};
    const std::vector<std::size_t> boundary = planar_boundary(
        points, all, plane
    );
    std::vector<Point3> vertices;
    vertices.reserve(boundary.size());
    for(std::size_t index: boundary) vertices.push_back(points[index]);
    std::vector<std::array<std::size_t, 3>> faces;
    for(std::size_t index = 1; index + 1 < vertices.size(); ++index){
        faces.push_back({0, index, index + 1});
    }
    return {2, std::move(vertices), std::move(faces)};
}

struct SupportingFacet{
    std::array<std::size_t, 3> plane{};
};

inline void add_supporting_facet(
    const std::array<std::size_t, 3>& candidate,
    const std::vector<Point3>& points,
    std::map<std::vector<std::size_t>, SupportingFacet>& facets
){
    if(!non_collinear(
        points[candidate[0]], points[candidate[1]], points[candidate[2]]
    )) return;
    bool has_positive = false;
    bool has_negative = false;
    std::vector<std::size_t> coplanar;
    coplanar.reserve(points.size());
    for(std::size_t index = 0; index < points.size(); ++index){
        const int side = adaptive_orient3d(
            points[candidate[0]], points[candidate[1]], points[candidate[2]],
            points[index]
        );
        has_positive = has_positive || side > 0;
        has_negative = has_negative || side < 0;
        if(side == 0) coplanar.push_back(index);
        if(has_positive && has_negative) return;
    }
    if(!has_positive && !has_negative) return;
    std::array<std::size_t, 3> outward = candidate;
    if(has_positive) std::swap(outward[1], outward[2]);
    facets.try_emplace(std::move(coplanar), SupportingFacet{outward});
}

inline void append_child_face_candidates(
    const ConvexPolyhedron3& child,
    std::size_t offset,
    const std::vector<Point3>& points,
    std::map<std::vector<std::size_t>, SupportingFacet>& facets
){
    for(const ConvexPolyhedronFacet3& facet: convex_polyhedron_facets(child)){
        const auto& face = child.faces[facet.triangles.front()];
        add_supporting_facet({
            offset + face[0], offset + face[1], offset + face[2]
        }, points, facets);
    }
}

inline void append_bridge_candidates(
    const ConvexPolyhedron3& edge_child,
    std::size_t edge_offset,
    const ConvexPolyhedron3& vertex_child,
    std::size_t vertex_offset,
    const std::vector<Point3>& points,
    std::map<std::vector<std::size_t>, SupportingFacet>& facets
){
    const auto edges = geometric_edges(edge_child);
    for(const auto& edge: edges){
        for(std::size_t vertex = 0; vertex < vertex_child.vertices.size(); ++vertex){
            add_supporting_facet({
                edge_offset + edge[0], edge_offset + edge[1],
                vertex_offset + vertex
            }, points, facets);
        }
    }
}

inline ConvexPolyhedron3 merge(
    const ConvexPolyhedron3& left,
    const ConvexPolyhedron3& right
){
    std::vector<Point3> points = left.vertices;
    points.insert(points.end(), right.vertices.begin(), right.vertices.end());
    std::sort(points.begin(), points.end());
    points.erase(std::unique(points.begin(), points.end(), same_point), points.end());
    if(points.size() < 4) return lower_dimensional_hull(points);

    std::size_t third = points.size();
    for(std::size_t index = 2; index < points.size(); ++index){
        if(non_collinear(points[0], points[1], points[index])){
            third = index;
            break;
        }
    }
    if(third == points.size()) return lower_dimensional_hull(points);
    bool full_dimensional = false;
    for(std::size_t index = 0; index < points.size(); ++index){
        if(adaptive_orient3d(
            points[0], points[1], points[third], points[index]
        ) != 0){
            full_dimensional = true;
            break;
        }
    }
    if(!full_dimensional) return lower_dimensional_hull(points);

    // Candidate indices refer to the children before the deterministic sort.
    std::vector<Point3> concatenated = left.vertices;
    concatenated.insert(
        concatenated.end(), right.vertices.begin(), right.vertices.end()
    );
    std::map<std::vector<std::size_t>, SupportingFacet> concatenated_facets;
    append_child_face_candidates(
        left, 0, concatenated, concatenated_facets
    );
    append_child_face_candidates(
        right, left.vertices.size(), concatenated, concatenated_facets
    );
    append_bridge_candidates(
        left, 0, right, left.vertices.size(), concatenated,
        concatenated_facets
    );
    append_bridge_candidates(
        right, left.vertices.size(), left, 0, concatenated,
        concatenated_facets
    );

    std::map<std::vector<std::size_t>, SupportingFacet> facets;
    for(const auto& [coplanar, facet]: concatenated_facets){
        static_cast<void>(coplanar);
        std::array<std::size_t, 3> remapped{};
        for(std::size_t index = 0; index < 3; ++index){
            const auto iterator = std::lower_bound(
                points.begin(), points.end(), concatenated[facet.plane[index]]
            );
            if(iterator == points.end()
                || !same_point(*iterator, concatenated[facet.plane[index]]))[[unlikely]]{
                throw std::logic_error("divide-and-conquer hull remap failed");
            }
            remapped[index] = static_cast<std::size_t>(iterator - points.begin());
        }
        add_supporting_facet(remapped, points, facets);
    }
    if(facets.empty())[[unlikely]]{
        throw std::logic_error("divide-and-conquer hull merge found no facets");
    }

    std::vector<std::array<std::size_t, 3>> point_faces;
    for(const auto& [coplanar, facet]: facets){
        std::vector<std::size_t> boundary = planar_boundary(
            points, coplanar, facet.plane
        );
        if(boundary.size() < 3) continue;
        bool reversed = false;
        for(std::size_t index = 0; index < points.size(); ++index){
            const int side = adaptive_orient3d(
                points[boundary[0]], points[boundary[1]], points[boundary[2]],
                points[index]
            );
            if(side != 0){
                reversed = side > 0;
                break;
            }
        }
        if(reversed) std::reverse(boundary.begin() + 1, boundary.end());
        for(std::size_t index = 1; index + 1 < boundary.size(); ++index){
            point_faces.push_back({
                boundary[0], boundary[index], boundary[index + 1]
            });
        }
    }
    if(point_faces.empty())[[unlikely]]{
        throw std::logic_error("divide-and-conquer hull triangulation failed");
    }

    std::set<std::size_t> used;
    for(const auto& face: point_faces) used.insert(face.begin(), face.end());
    std::vector<std::size_t> remap(points.size(), points.size());
    std::vector<Point3> vertices;
    vertices.reserve(used.size());
    for(std::size_t index: used){
        remap[index] = vertices.size();
        vertices.push_back(points[index]);
    }
    std::vector<std::array<std::size_t, 3>> faces;
    faces.reserve(point_faces.size());
    for(const auto& face: point_faces){
        faces.push_back({remap[face[0]], remap[face[1]], remap[face[2]]});
    }
    return {3, std::move(vertices), std::move(faces)};
}

inline ConvexPolyhedron3 solve(
    const std::vector<Point3>& points,
    std::size_t begin,
    std::size_t end
){
    if(begin == end) return {};
    if(end - begin == 1) return {0, {points[begin]}, {}};
    const std::size_t middle = begin + (end - begin) / 2;
    return merge(solve(points, begin, middle), solve(points, middle, end));
}

}  // namespace divide_and_conquer_convex_hull_3d_detail

inline ConvexPolyhedron3 divide_and_conquer_convex_hull_3d(
    std::vector<Point3> points
){
    for(const Point3& point: points){
        if(!geometry3d_is_finite(point))[[unlikely]]{
            throw std::invalid_argument(
                "divide_and_conquer_convex_hull_3d requires finite points"
            );
        }
    }
    std::sort(points.begin(), points.end());
    points.erase(std::unique(
        points.begin(), points.end(),
        divide_and_conquer_convex_hull_3d_detail::same_point
    ), points.end());
    return divide_and_conquer_convex_hull_3d_detail::solve(
        points, 0, points.size()
    );
}
