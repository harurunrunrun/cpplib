#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RESULT_POINT_SET_POINT_SET_DIAMETER_3D_DEFAULT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RESULT_POINT_SET_POINT_SET_DIAMETER_3D_DEFAULT_HPP_INCLUDED

#include <cstddef>
#include <optional>
#include <vector>

#include "../../detail/point_set/point_set_diameter_3d_detail.hpp"

inline std::optional<PointSetDiameter3DResult> point_set_diameter_3d(
    const std::vector<Point3>& points
){
    point_set_diameter_3d_detail::validate(points);
    if(points.size() < 2) return std::nullopt;
    PointSetDiameter3DResult result{
        0,
        1,
        point_set_diameter_3d_detail::scaled_euclidean_distance(
            points[0], points[1]
        ),
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

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RESULT_POINT_SET_POINT_SET_DIAMETER_3D_DEFAULT_HPP_INCLUDED
