#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_DEDUPLICATE_POINTS_3D_RELATIVE_RANGE_TREE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_DEDUPLICATE_POINTS_3D_RELATIVE_RANGE_TREE_HPP_INCLUDED

#include "deduplicate_points_3d_detail.hpp"

inline std::vector<Point3> deduplicate_points_3d_relative_range_tree(
    const std::vector<Point3>& points,
    long double relative_tolerance,
    long double absolute_tolerance = GEOMETRY3D_EPS
){
    if(!geometry3d_is_finite(relative_tolerance)
        || !geometry3d_is_finite(absolute_tolerance)
        || relative_tolerance <= 0.0L
        || absolute_tolerance < 0.0L)[[unlikely]]{
        throw std::invalid_argument(
            "invalid relative 3D point deduplication tolerance"
        );
    }
    for(const Point3& point: points){
        if(!geometry3d_is_finite(point))[[unlikely]]{
            throw std::invalid_argument("non-finite point in 3D point set");
        }
    }
    if(points.empty()) return {};

    std::array<std::vector<long double>, 3> coordinates;
    for(const Point3& point: points){
        coordinates[0].push_back(point.x);
        coordinates[1].push_back(point.y);
        coordinates[2].push_back(point.z);
    }
    for(auto& values: coordinates){
        std::sort(values.begin(), values.end());
        values.erase(std::unique(values.begin(), values.end()), values.end());
    }

    std::vector<deduplicate_points_3d_detail::RankedPoint3> ranked;
    ranked.reserve(points.size());
    for(const Point3& point: points){
        ranked.push_back({
            static_cast<std::size_t>(
                std::lower_bound(
                    coordinates[0].begin(), coordinates[0].end(), point.x
                ) - coordinates[0].begin()
            ),
            static_cast<std::size_t>(
                std::lower_bound(
                    coordinates[1].begin(), coordinates[1].end(), point.y
                ) - coordinates[1].begin()
            ),
            static_cast<std::size_t>(
                std::lower_bound(
                    coordinates[2].begin(), coordinates[2].end(), point.z
                ) - coordinates[2].begin()
            ),
        });
    }

    deduplicate_points_3d_detail::DynamicOrthogonalRangeEmptiness3D index(
        ranked, coordinates[0].size()
    );
    std::vector<Point3> result;
    result.reserve(points.size());
    for(std::size_t point_index = 0;
        point_index < points.size(); ++point_index){
        std::array<std::vector<deduplicate_points_3d_detail::RankInterval>, 3>
            intervals;
        for(std::size_t axis = 0; axis < 3; ++axis){
            intervals[axis] =
                deduplicate_points_3d_detail::coordinate_intervals(
                    coordinates[axis],
                    deduplicate_points_3d_detail::coordinate(
                        points[point_index], axis
                    ),
                    relative_tolerance, absolute_tolerance
                );
        }
        bool duplicate = false;
        for(const auto& x: intervals[0]){
            for(const auto& y: intervals[1]){
                for(const auto& z: intervals[2]){
                    if(index.any(x, y, z)){
                        duplicate = true;
                        break;
                    }
                }
                if(duplicate) break;
            }
            if(duplicate) break;
        }
        if(!duplicate){
            result.push_back(points[point_index]);
            index.add(ranked[point_index]);
        }
    }
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_DEDUPLICATE_POINTS_3D_RELATIVE_RANGE_TREE_HPP_INCLUDED
