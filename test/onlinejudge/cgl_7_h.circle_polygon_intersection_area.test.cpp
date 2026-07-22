// competitive-verifier: PROBLEM https://onlinejudge.u-aizu.ac.jp/problems/CGL_7_H
// competitive-verifier: ERROR 1e-8

#include <iomanip>
#include <iostream>
#include <vector>

#include "../../src/algorithm/geometry/2d/scalar/circle_polygon_intersection_area.hpp"

int main(){
    int count;
    std::cin >> count;
    Circle circle{{0.0L, 0.0L}, 0.0L};
    std::cin >> circle.radius;
    std::vector<Point> polygon(static_cast<std::size_t>(count));
    for(Point& point: polygon) std::cin >> point.x >> point.y;
    std::cout << std::fixed << std::setprecision(12)
              << circle_polygon_intersection_area(circle, polygon) << '\n';
}
