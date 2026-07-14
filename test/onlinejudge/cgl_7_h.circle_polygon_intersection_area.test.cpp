// competitive-verifier: PROBLEM https://onlinejudge.u-aizu.ac.jp/problems/CGL_7_H
// competitive-verifier: ERROR 1e-8

#include <iomanip>
#include <iostream>
#include <vector>

#include "../../src/algorithm/geometry/2d/circle_polygon_intersection_area.hpp"

int main(){
    int count;
    std::cin >> count;
    Circle circle;
    std::cin >> circle.center.x >> circle.center.y >> circle.radius;
    std::vector<Point> polygon(static_cast<std::size_t>(count));
    for(Point& point: polygon) std::cin >> point.x >> point.y;
    std::cout << std::fixed << std::setprecision(12)
              << circle_polygon_intersection_area(circle, polygon) << '\n';
}
