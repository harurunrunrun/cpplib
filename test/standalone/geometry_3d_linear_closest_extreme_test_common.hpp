#ifndef CPPLIB_TEST_STANDALONE_GEOMETRY_3D_LINEAR_CLOSEST_EXTREME_TEST_COMMON_HPP_INCLUDED
#define CPPLIB_TEST_STANDALONE_GEOMETRY_3D_LINEAR_CLOSEST_EXTREME_TEST_COMMON_HPP_INCLUDED

#include <cmath>

#include "../../src/algorithm/geometry/3d/core/geometry_primitives.hpp"

inline constexpr long double GEOMETRY3D_LINEAR_TEST_TRANSLATION =
    1e3000L;
inline constexpr long double GEOMETRY3D_LINEAR_TEST_LOCAL =
    1e-3000L;

inline bool geometry3d_linear_test_close(
    long double actual,
    long double expected,
    long double epsilon = 2e-17L
){
    if(expected == 0.0L) return actual == 0.0L;
    return std::abs(actual / expected - 1.0L) <= epsilon;
}

inline bool geometry3d_linear_test_close(
    const Point3& actual,
    const Point3& expected
){
    return geometry3d_linear_test_close(actual.x, expected.x)
        && geometry3d_linear_test_close(actual.y, expected.y)
        && geometry3d_linear_test_close(actual.z, expected.z);
}

inline Line3 geometry3d_linear_test_line(){
    const long double r = GEOMETRY3D_LINEAR_TEST_LOCAL;
    const long double t = GEOMETRY3D_LINEAR_TEST_TRANSLATION;
    return {{0, 0, t}, {r, 0, t}};
}

inline Ray3 geometry3d_linear_test_ray(){
    const Line3 line = geometry3d_linear_test_line();
    return {line.a, line.b};
}

inline Segment3 geometry3d_linear_test_segment(){
    const Line3 line = geometry3d_linear_test_line();
    return {line.a, line.b};
}

inline Line3 geometry3d_linear_test_parallel_line(){
    const long double r = GEOMETRY3D_LINEAR_TEST_LOCAL;
    const long double t = GEOMETRY3D_LINEAR_TEST_TRANSLATION;
    return {{0, r, t}, {r, r, t}};
}

inline Triangle3 geometry3d_linear_test_triangle(){
    const long double r = GEOMETRY3D_LINEAR_TEST_LOCAL;
    const long double t = GEOMETRY3D_LINEAR_TEST_TRANSLATION;
    return {
        {0, 0, t},
        {r, 0, t},
        {0, r, t},
    };
}

#endif  // CPPLIB_TEST_STANDALONE_GEOMETRY_3D_LINEAR_CLOSEST_EXTREME_TEST_COMMON_HPP_INCLUDED
