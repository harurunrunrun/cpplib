// competitive-verifier: PROBLEM https://onlinejudge.u-aizu.ac.jp/problems/CGL_7_A

#include <iostream>

#include "../../src/algorithm/geometry/2d/predicate/circle_relation.hpp"

int main(){
    Circle a, b;
    std::cin >> a.center.x >> a.center.y >> a.radius;
    std::cin >> b.center.x >> b.center.y >> b.radius;
    std::cout << circle_relation(a, b) << '\n';
}
