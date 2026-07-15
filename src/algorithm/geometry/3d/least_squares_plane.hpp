#pragma once

#include <cmath>
#include <optional>
#include <stdexcept>
#include <vector>

#include "base.hpp"
#include "principal_axes_3d.hpp"

inline std::optional<Plane3> least_squares_plane(
    const std::vector<Point3>& points,
    long double relative_epsilon = GEOMETRY3D_EPS
){
    if(relative_epsilon < 0 || !std::isfinite(relative_epsilon)){
        throw std::invalid_argument("relative epsilon must be finite and nonnegative");
    }
    for(const Point3& point: points){
        if(!std::isfinite(point.x) || !std::isfinite(point.y) ||
           !std::isfinite(point.z)){
            throw std::invalid_argument("points must be finite");
        }
    }
    if(points.size() < 3) return std::nullopt;
    const PrincipalAxes3 axes = principal_axes_3d(points, relative_epsilon);
    if(!(axes.variances[0] > 0) ||
       axes.variances[1] <= relative_epsilon * axes.variances[0]){
        return std::nullopt;
    }
    return Plane3{axes.centroid, axes.axes[2]};
}
