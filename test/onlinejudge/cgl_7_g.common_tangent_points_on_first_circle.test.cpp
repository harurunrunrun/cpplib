// competitive-verifier: PROBLEM https://onlinejudge.u-aizu.ac.jp/problems/CGL_7_G
// competitive-verifier: ERROR 1e-8

#include <iomanip>
#include <iostream>

#include "../../src/algorithm/geometry/2d/common_tangent_points_on_first_circle.hpp"

int main(){
    Circle first, second;
    std::cin >> first.center.x >> first.center.y >> first.radius;
    std::cin >> second.center.x >> second.center.y >> second.radius;
    const auto points = common_tangent_points_on_first_circle(first, second);
    std::cout << std::fixed << std::setprecision(10);
    for(const Point& point: points){
        std::cout << point.x << ' ' << point.y << '\n';
    }
}
