#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POLYHEDRON_POINT_SET_CONVEX_HULL_3D_DEFAULT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POLYHEDRON_POINT_SET_CONVEX_HULL_3D_DEFAULT_HPP_INCLUDED

#include <utility>
#include <vector>

#include "../../detail/point_set/convex_hull_3d_detail.hpp"

inline ConvexPolyhedron3 convex_hull_3d(std::vector<Point3> input){
    return convex_hull_3d_detail::build_with_seed(
        std::move(input), convex_hull_3d_detail::default_random_seed
    );
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POLYHEDRON_POINT_SET_CONVEX_HULL_3D_DEFAULT_HPP_INCLUDED
