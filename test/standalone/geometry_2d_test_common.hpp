#ifndef CPPLIB_TEST_STANDALONE_GEOMETRY_2D_TEST_COMMON_HPP_INCLUDED
#define CPPLIB_TEST_STANDALONE_GEOMETRY_2D_TEST_COMMON_HPP_INCLUDED

#include <cmath>
#include <iomanip>
#include <iostream>

inline void geometry_test_setup_output(){
    std::cout << std::fixed << std::setprecision(10);
}

inline long double geometry_test_clean(long double value){
    return std::fabs(value) < 5e-12L ? 0.0L : value;
}

inline void geometry_test_print_real(long double value){
    std::cout << geometry_test_clean(value) << '\n';
}

template<class PointLike>
inline void geometry_test_print_point(const PointLike& point){
    std::cout << geometry_test_clean(point.x) << ' '
              << geometry_test_clean(point.y) << '\n';
}

#endif  // CPPLIB_TEST_STANDALONE_GEOMETRY_2D_TEST_COMMON_HPP_INCLUDED
