#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINTS_IN_HEMISPHERE_DEFAULT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINTS_IN_HEMISPHERE_DEFAULT_HPP_INCLUDED

#include <vector>

#include "points_in_hemisphere_with_seed.hpp"

inline bool points_in_hemisphere(const std::vector<Point3>& points){
    return points_in_hemisphere_with_seed(
        points, 0xD1B54A32D192ED03ULL
    );
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINTS_IN_HEMISPHERE_DEFAULT_HPP_INCLUDED
