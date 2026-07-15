// competitive-verifier: PROBLEM https://onlinejudge.u-aizu.ac.jp/problems/CGL_7_F
// competitive-verifier: ERROR 1e-8

#include <iomanip>
#include <iostream>

#include "../../src/algorithm/geometry/2d/tangent_points.hpp"

int main(){
    Circle circle;
    Point point;
    std::cin >> point.x >> point.y;
    std::cin >> circle.center.x >> circle.center.y >> circle.radius;
    auto points = tangent_points(circle, point);
    if(points.size() == 1) points.push_back(points[0]);
    std::cout << std::fixed << std::setprecision(10)
              << points[0].x << ' ' << points[0].y << '\n'
              << points[1].x << ' ' << points[1].y << '\n';
}
