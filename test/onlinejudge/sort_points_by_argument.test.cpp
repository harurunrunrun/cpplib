// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/sort_points_by_argument

#include <cstddef>
#include <iostream>
#include <utility>
#include <vector>

#include "../../src/algorithm/geometry/2d/sort_points_by_argument.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int size;
    std::cin >> size;
    std::vector<Point> points(static_cast<std::size_t>(size));
    for(Point& point: points) std::cin >> point.x >> point.y;
    points = sort_points_by_argument(std::move(points));
    for(const Point& point: points){
        std::cout << static_cast<long long>(point.x) << ' '
                  << static_cast<long long>(point.y) << '\n';
    }
}
