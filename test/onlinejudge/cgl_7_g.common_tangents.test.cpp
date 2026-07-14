// competitive-verifier: PROBLEM https://onlinejudge.u-aizu.ac.jp/problems/CGL_7_G

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <vector>

#include "../../src/algorithm/geometry/2d/geometry.hpp"

int main(){
    Circle a, b;
    std::cin >> a.center.x >> a.center.y >> a.radius;
    std::cin >> b.center.x >> b.center.y >> b.radius;
    std::vector<Point> points;
    for(const Line& tangent: common_tangents(a, b)) points.push_back(tangent.a);
    std::sort(points.begin(), points.end());
    std::cout << std::fixed << std::setprecision(12);
    for(const Point& point: points) std::cout << point.x << ' ' << point.y << '\n';
}
