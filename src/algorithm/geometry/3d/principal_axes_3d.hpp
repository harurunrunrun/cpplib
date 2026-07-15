#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <stdexcept>
#include <vector>

#include "base.hpp"
#include "symmetric_eigen_3x3.hpp"

struct PrincipalAxes3{
    Point3 centroid;
    std::array<Point3, 3> axes{};
    std::array<long double, 3> variances{};
};

inline PrincipalAxes3 principal_axes_3d(
    const std::vector<Point3>& points,
    long double relative_epsilon = GEOMETRY3D_EPS
){
    if(points.empty()) throw std::invalid_argument("point set must be nonempty");
    if(relative_epsilon < 0 || !std::isfinite(relative_epsilon)){
        throw std::invalid_argument("relative epsilon must be finite and nonnegative");
    }
    long double scale = 0;
    for(const Point3& point: points){
        if(!std::isfinite(point.x) || !std::isfinite(point.y) ||
           !std::isfinite(point.z)){
            throw std::invalid_argument("points must be finite");
        }
        scale = std::max({scale, std::abs(point.x), std::abs(point.y),
                          std::abs(point.z)});
    }
    if(scale == 0){
        PrincipalAxes3 result;
        result.axes = {Point3{1, 0, 0}, Point3{0, 1, 0}, Point3{0, 0, 1}};
        return result;
    }

    Point3 normalized_centroid;
    std::size_t count = 0;
    for(const Point3& point: points){
        ++count;
        const Point3 normalized = point / scale;
        normalized_centroid +=
            (normalized - normalized_centroid) / static_cast<long double>(count);
    }
    Matrix3 covariance;
    for(const Point3& point: points){
        const Point3 difference = point / scale - normalized_centroid;
        const std::array<long double, 3> values{{
            difference.x, difference.y, difference.z
        }};
        for(std::size_t row = 0; row < 3; ++row){
            for(std::size_t column = row; column < 3; ++column){
                covariance[row][column] += values[row] * values[column];
            }
        }
    }
    for(std::size_t row = 0; row < 3; ++row){
        for(std::size_t column = row; column < 3; ++column){
            covariance[row][column] /= static_cast<long double>(points.size());
            covariance[column][row] = covariance[row][column];
        }
    }
    const SymmetricEigen3 eigen = symmetric_eigen_3x3(covariance, relative_epsilon);

    PrincipalAxes3 result;
    normalized_centroid.x = std::clamp(normalized_centroid.x, -1.0L, 1.0L);
    normalized_centroid.y = std::clamp(normalized_centroid.y, -1.0L, 1.0L);
    normalized_centroid.z = std::clamp(normalized_centroid.z, -1.0L, 1.0L);
    result.centroid = normalized_centroid * scale;
    const long double zero_tolerance = 64 * std::max(
        relative_epsilon, std::numeric_limits<long double>::epsilon()
    );
    for(std::size_t index = 0; index < 3; ++index){
        result.axes[index] = eigen.eigenvector(index);
        long double normalized_variance = eigen.eigenvalues[index];
        if(normalized_variance < 0 &&
           std::abs(normalized_variance) <= zero_tolerance){
            normalized_variance = 0;
        }
        if(normalized_variance > 0 &&
           scale > std::sqrt(std::numeric_limits<long double>::max() /
                             normalized_variance)){
            throw std::overflow_error("principal variance is not representable");
        }
        result.variances[index] = normalized_variance * scale;
        result.variances[index] *= scale;
        if(!std::isfinite(result.variances[index])){
            throw std::overflow_error("principal variance is not representable");
        }
    }
    return result;
}
