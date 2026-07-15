#pragma once

#include <array>
#include <cmath>
#include <limits>

#include "../../src/algorithm/geometry/3d/base.hpp"

inline bool geometry3d_extreme_relative_close(
    long double actual,
    long double expected,
    long double epsilon = 2e-17L
){
    if(expected == 0.0L) return actual == 0.0L;
    return std::abs(actual / expected - 1.0L) <= epsilon;
}

inline Triangle3 geometry3d_extreme_right_triangle(long double scale){
    const long double translation = 1e3000L;
    return {
        {0, 0, translation},
        {2 * scale, 0, translation},
        {0, 2 * scale, translation},
    };
}

inline Tetrahedron3 geometry3d_extreme_right_tetrahedron(
    long double scale
){
    return {
        {0, 0, 0},
        {scale, 0, 0},
        {0, scale, 0},
        {0, 0, scale},
    };
}

struct Geometry3DTranslatedTetrahedronCase{
    Tetrahedron3 tetrahedron;
    long double translation = 0.0L;
    long double edge = 0.0L;
};

inline Geometry3DTranslatedTetrahedronCase
geometry3d_extreme_translated_tetrahedron(){
    const long double translation = 1e3000L;
    const long double ulp = std::nextafter(
        translation, std::numeric_limits<long double>::infinity()
    ) - translation;
    const long double edge = 1024 * ulp;
    return {
        {
            {translation, translation, translation},
            {translation + edge, translation, translation},
            {translation, translation + edge, translation},
            {translation, translation, translation + edge},
        },
        translation,
        edge,
    };
}
