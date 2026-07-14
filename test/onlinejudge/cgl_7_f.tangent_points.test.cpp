// competitive-verifier: PROBLEM https://onlinejudge.u-aizu.ac.jp/problems/CGL_7_F

#include <iomanip>
#include <iostream>

#include "../../src/algorithm/geometry/2d/geometry.hpp"

int main(){
    Circle circle;
    Point point;
    std::cin >> circle.center.x >> circle.center.y >> circle.radius;
    std::cin >> point.x >> point.y;
    auto points = tangent_points(circle, point);
    if(points.size() == 1) points.push_back(points[0]);
    std::cout << std::fixed << std::setprecision(12)
              << points[0].x << ' ' << points[0].y << '\n'
              << points[1].x << ' ' << points[1].y << '\n';
}
