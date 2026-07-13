#define PROBLEM "https://onlinejudge.u-aizu.ac.jp/problems/CGL_7_B"

#include <iomanip>
#include <iostream>

#include "../../src/algorithm/geometry/geometry.hpp"

int main(){
    Point a, b, c;
    std::cin >> a.x >> a.y >> b.x >> b.y >> c.x >> c.y;
    const Circle answer = incircle(a, b, c);
    std::cout << std::fixed << std::setprecision(12)
              << answer.center.x << ' ' << answer.center.y << ' '
              << answer.radius << '\n';
}
