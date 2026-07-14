// competitive-verifier: PROBLEM https://onlinejudge.u-aizu.ac.jp/problems/CGL_7_C
// competitive-verifier: ERROR 1e-8

#include <iomanip>
#include <iostream>

#include "../../src/algorithm/geometry/2d/geometry.hpp"

int main(){
    Point a, b, c;
    std::cin >> a.x >> a.y >> b.x >> b.y >> c.x >> c.y;
    const Circle answer = circumcircle(a, b, c);
    std::cout << std::fixed << std::setprecision(10)
              << answer.center.x << ' ' << answer.center.y << ' '
              << answer.radius << '\n';
}
