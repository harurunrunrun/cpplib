// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/minimum_enclosing_circle

#include <cstddef>
#include <iostream>
#include <vector>

#include "../../src/algorithm/geometry/2d/query/minimum_enclosing_circle_boundary_flags.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int size;
    std::cin >> size;
    std::vector<Point> points(static_cast<std::size_t>(size));
    for(Point& point: points) std::cin >> point.x >> point.y;
    const auto answer = minimum_enclosing_circle_boundary_flags(points);
    for(bool value: answer) std::cout << static_cast<int>(value);
    std::cout << '\n';
}
