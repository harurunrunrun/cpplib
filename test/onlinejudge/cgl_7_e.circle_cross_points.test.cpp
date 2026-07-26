// competitive-verifier: PROBLEM https://onlinejudge.u-aizu.ac.jp/problems/CGL_7_E
// competitive-verifier: ERROR 1e-8

#include <iomanip>
#include <iostream>

#include "../../src/algorithm/geometry/2d/circle_circle_cross_points.hpp"

int main(){
    Circle a, b;
    std::cin >> a.center.x >> a.center.y >> a.radius;
    std::cin >> b.center.x >> b.center.y >> b.radius;
    auto points = circle_circle_cross_points(a, b);
    if(points.size() == 1) points.push_back(points[0]);
    std::cout << std::fixed << std::setprecision(10)
              << points[0].x << ' ' << points[0].y << ' '
              << points[1].x << ' ' << points[1].y << '\n';
}
