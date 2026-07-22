#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_POINT_SET_DIAMETER_3D_VIA_CONVEX_HULL_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_POINT_SET_DIAMETER_3D_VIA_CONVEX_HULL_HPP_INCLUDED

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <map>
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../shape/convex_hull_3d.hpp"
#include "point_set_diameter_3d.hpp"

namespace point_set_diameter_3d_via_convex_hull_detail{

inline std::optional<PointSetDiameter3DResult> recover_original_indices(
    const std::vector<Point3>& points,
    const ConvexPolyhedron3& hull
){
    if(points.size() < 2) return std::nullopt;
    if(hull.vertices.empty())[[unlikely]]{
        throw std::logic_error(
            "convex hull of a nonempty point set has no vertex"
        );
    }
    if(hull.vertices.size() == 1){
        return PointSetDiameter3DResult{
            0, 1,
            point_set_diameter_3d_detail::scaled_euclidean_distance(
                points[0], points[1]
            ),
        };
    }

    std::map<Point3, std::size_t> first_original_index;
    for(std::size_t index = 0; index < points.size(); ++index){
        first_original_index.try_emplace(points[index], index);
    }
    std::vector<std::pair<std::size_t, Point3>> indexed_vertices;
    indexed_vertices.reserve(hull.vertices.size());
    for(const Point3& vertex: hull.vertices){
        const auto iterator = first_original_index.find(vertex);
        if(iterator == first_original_index.end())[[unlikely]]{
            throw std::logic_error(
                "convex hull vertex does not match an input point"
            );
        }
        indexed_vertices.emplace_back(iterator->second, vertex);
    }
    std::sort(indexed_vertices.begin(), indexed_vertices.end());

    std::vector<Point3> hull_points;
    std::vector<std::size_t> original_indices;
    hull_points.reserve(indexed_vertices.size());
    original_indices.reserve(indexed_vertices.size());
    for(const auto& [index, vertex]: indexed_vertices){
        original_indices.push_back(index);
        hull_points.push_back(vertex);
    }
    const auto hull_result =
        point_set_diameter_3d_aabb_branch_and_bound(hull_points);
    if(!hull_result)[[unlikely]]{
        throw std::logic_error("convex hull diameter has fewer than two points");
    }
    if(!std::isfinite(hull_result->distance)){
        // Several mathematically different distances can all be represented
        // as infinity.  The full search preserves the public API's
        // lexicographic tie rule in that case, including pairs with an
        // interior endpoint.
        return point_set_diameter_3d_aabb_branch_and_bound(points);
    }
    return PointSetDiameter3DResult{
        original_indices[hull_result->first],
        original_indices[hull_result->second],
        hull_result->distance,
    };
}

}  // namespace point_set_diameter_3d_via_convex_hull_detail

inline std::optional<PointSetDiameter3DResult>
point_set_diameter_3d_via_convex_hull_with_seed(
    const std::vector<Point3>& points,
    std::uint64_t random_seed
){
    return point_set_diameter_3d_via_convex_hull_detail::
        recover_original_indices(
            points, convex_hull_3d_with_seed(points, random_seed)
        );
}

inline std::optional<PointSetDiameter3DResult>
point_set_diameter_3d_via_convex_hull(
    const std::vector<Point3>& points
){
    return point_set_diameter_3d_via_convex_hull_detail::
        recover_original_indices(points, convex_hull_3d(points));
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_POINT_SET_DIAMETER_3D_VIA_CONVEX_HULL_HPP_INCLUDED
