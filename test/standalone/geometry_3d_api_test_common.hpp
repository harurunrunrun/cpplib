#ifndef CPPLIB_TEST_STANDALONE_GEOMETRY_3D_API_TEST_COMMON_HPP_INCLUDED
#define CPPLIB_TEST_STANDALONE_GEOMETRY_3D_API_TEST_COMMON_HPP_INCLUDED

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <random>

#include "../../src/algorithm/geometry/3d/core/geometry_primitives.hpp"

inline bool geometry3d_api_close(
    long double left,
    long double right,
    long double epsilon = 1e-9L
){
    return std::abs(left - right) <= epsilon *
        std::max({1.0L, std::abs(left), std::abs(right)});
}

inline bool geometry3d_api_close(
    const Point3& left,
    const Point3& right,
    long double epsilon = 1e-9L
){
    return geometry3d_api_close(left.x, right.x, epsilon) &&
        geometry3d_api_close(left.y, right.y, epsilon) &&
        geometry3d_api_close(left.z, right.z, epsilon);
}

template<class Function>
int geometry3d_api_test_main(Function function){
    std::uint64_t seed;
    std::size_t rounds;
    if(!(std::cin >> seed >> rounds) || rounds == 0 || rounds > 10000) return 2;
    std::mt19937_64 random(seed);
    if(!function(random, rounds)) return 1;
    std::cout << "OK\n";
    return 0;
}

#endif  // CPPLIB_TEST_STANDALONE_GEOMETRY_3D_API_TEST_COMMON_HPP_INCLUDED
