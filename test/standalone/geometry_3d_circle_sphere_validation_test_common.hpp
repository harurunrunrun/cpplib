#ifndef CPPLIB_TEST_STANDALONE_GEOMETRY_3D_CIRCLE_SPHERE_VALIDATION_TEST_COMMON_HPP_INCLUDED
#define CPPLIB_TEST_STANDALONE_GEOMETRY_3D_CIRCLE_SPHERE_VALIDATION_TEST_COMMON_HPP_INCLUDED

#include <limits>
#include <stdexcept>

#include "../../src/algorithm/geometry/3d/base.hpp"

template<class Exception, class Function>
bool geometry3d_throws(Function&& function){
    try{
        function();
    }catch(const Exception&){
        return true;
    }catch(...){
        return false;
    }
    return false;
}

template<class Function>
bool geometry3d_rejects_invalid_circles(Function&& function){
    const long double infinity = std::numeric_limits<long double>::infinity();
    const long double nan = std::numeric_limits<long double>::quiet_NaN();
    const Circle3 invalid[] = {
        {{0, 0, 0}, {0, 0, 1}, -1},
        {{nan, 0, 0}, {0, 0, 1}, 1},
        {{0, 0, 0}, {0, infinity, 1}, 1},
        {{0, 0, 0}, {0, 0, 1}, infinity},
        {{0, 0, 0}, {0, 0, 0}, 1},
    };
    for(const Circle3& circle: invalid){
        if(!geometry3d_throws<std::invalid_argument>([&]{ function(circle); })){
            return false;
        }
    }
    return true;
}

template<class Function>
bool geometry3d_rejects_invalid_spheres(Function&& function){
    const long double infinity = std::numeric_limits<long double>::infinity();
    const long double nan = std::numeric_limits<long double>::quiet_NaN();
    const Sphere3 invalid[] = {
        {{0, 0, 0}, -1},
        {{nan, 0, 0}, 1},
        {{0, infinity, 0}, 1},
        {{0, 0, 0}, infinity},
    };
    for(const Sphere3& sphere: invalid){
        if(!geometry3d_throws<std::invalid_argument>([&]{ function(sphere); })){
            return false;
        }
    }
    return true;
}

#endif  // CPPLIB_TEST_STANDALONE_GEOMETRY_3D_CIRCLE_SPHERE_VALIDATION_TEST_COMMON_HPP_INCLUDED
