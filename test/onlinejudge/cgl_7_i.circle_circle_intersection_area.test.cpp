// competitive-verifier: PROBLEM https://onlinejudge.u-aizu.ac.jp/problems/CGL_7_I
// competitive-verifier: ERROR 1e-8

#include <iomanip>
#include <iostream>

#include "../../src/algorithm/geometry/2d/circle_circle_intersection_area.hpp"

int main(){
    Circle a, b;
    std::cin >> a.center.x >> a.center.y >> a.radius;
    std::cin >> b.center.x >> b.center.y >> b.radius;
    std::cout << std::fixed << std::setprecision(10)
              << circle_circle_intersection_area(a, b) << '\n';
}
