#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_POINT_COLLECTION_CONVEX_HULL_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_POINT_COLLECTION_CONVEX_HULL_HPP_INCLUDED

#include <utility>
#include <vector>

#include "../detail/convex_hull_construction_detail.hpp"

inline std::vector<Point> convex_hull(std::vector<Point> points){
    return convex_hull_detail::build_convex_hull(std::move(points), false);
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_POINT_COLLECTION_CONVEX_HULL_HPP_INCLUDED
