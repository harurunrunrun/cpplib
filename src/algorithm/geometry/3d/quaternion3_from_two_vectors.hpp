#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_QUATERNION3_FROM_TWO_VECTORS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_QUATERNION3_FROM_TWO_VECTORS_HPP_INCLUDED

#include <algorithm>
#include <cmath>
#include <limits>
#include <stdexcept>

#include "base.hpp"
#include "cross.hpp"
#include "dot.hpp"
#include "is_finite.hpp"
#include "quaternion3.hpp"

namespace quaternion3_from_two_vectors_detail{

inline Point3 unit_direction(const Point3& value){
    if(!geometry3d_is_finite(value))[[unlikely]]{
        throw std::invalid_argument("non-finite vector in shortest rotation");
    }
    const long double scale = std::max({
        std::abs(value.x), std::abs(value.y), std::abs(value.z)
    });
    if(scale == 0.0L)[[unlikely]]{
        throw std::invalid_argument("zero vector in shortest rotation");
    }
    const Point3 scaled = value / scale;
    return scaled / std::hypot(scaled.x, scaled.y, scaled.z);
}

inline Point3 deterministic_perpendicular(const Point3& direction){
    Point3 axis;
    if(std::abs(direction.x) <= std::abs(direction.y)
        && std::abs(direction.x) <= std::abs(direction.z)){
        axis = cross(direction, {1, 0, 0});
    }else if(std::abs(direction.y) <= std::abs(direction.z)){
        axis = cross(direction, {0, 1, 0});
    }else{
        axis = cross(direction, {0, 0, 1});
    }
    const long double length = std::hypot(axis.x, axis.y, axis.z);
    return axis / length;
}

}  // namespace quaternion3_from_two_vectors_detail

inline Quaternion3 quaternion3_from_two_vectors(
    const Point3& from,
    const Point3& to
){
    const Point3 first = quaternion3_from_two_vectors_detail::unit_direction(from);
    const Point3 second = quaternion3_from_two_vectors_detail::unit_direction(to);
    const long double cosine = std::clamp(dot(first, second), -1.0L, 1.0L);
    if(cosine >= 1.0L - 8.0L * std::numeric_limits<long double>::epsilon()){
        return Quaternion3::identity();
    }
    if(cosine <= -1.0L + 8.0L * std::numeric_limits<long double>::epsilon()){
        const Point3 axis =
            quaternion3_from_two_vectors_detail::deterministic_perpendicular(first);
        return {0.0L, axis.x, axis.y, axis.z};
    }
    const Point3 axis = cross(first, second);
    return Quaternion3{1.0L + cosine, axis.x, axis.y, axis.z}.normalized();
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_QUATERNION3_FROM_TWO_VECTORS_HPP_INCLUDED
