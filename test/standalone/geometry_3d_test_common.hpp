#pragma once

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <random>

#include "../../src/algorithm/geometry/3d/base.hpp"
#include "geometry_3d_property_tests.hpp"

inline bool geometry3d_close_value(
    long double left,
    long double right,
    long double epsilon = 1e-9L
){
    return std::abs(left - right) <= epsilon *
        std::max({1.0L, std::abs(left), std::abs(right)});
}

inline bool geometry3d_close_point(
    const Point3& left,
    const Point3& right,
    long double epsilon = 1e-9L
){
    return geometry3d_close_value(left.x, right.x, epsilon) &&
        geometry3d_close_value(left.y, right.y, epsilon) &&
        geometry3d_close_value(left.z, right.z, epsilon);
}

template<class Function>
int geometry3d_test_main_impl(const char* source_file, Function function){
    std::uint64_t seed = 0;
    if(!(std::cin >> seed)) return 2;
    std::size_t rounds = 0;
    if(!(std::cin >> rounds)){
        std::cin.clear();
        rounds = 32 + static_cast<std::size_t>(seed % 17);
    }
    if(rounds == 0 || rounds > 10000) return 2;
    if(!function()) return 1;
    std::mt19937_64 random(seed);
    if(!geometry3d_property_suite(source_file, random, rounds)){
        std::cerr << "property failure: " << source_file
                  << " seed=" << seed << " rounds=" << rounds << '\n';
        return 1;
    }
    std::cout << "OK\n";
    return 0;
}

#define geometry3d_test_main(...) \
    geometry3d_test_main_impl(__FILE__, (__VA_ARGS__))
