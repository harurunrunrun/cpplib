#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PRINCIPAL_AXES3_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PRINCIPAL_AXES3_HPP_INCLUDED

#include <array>

#include "point3.hpp"

struct PrincipalAxes3{
    Point3 centroid;
    std::array<Point3, 3> axes{};
    std::array<long double, 3> variances{};
};

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PRINCIPAL_AXES3_HPP_INCLUDED
