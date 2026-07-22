// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/static_convex_hull

#include <cmath>
#include <cstddef>
#include <iostream>
#include <vector>

#include "../../src/algorithm/geometry/2d/point_collection/convex_hull.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int test_count;
    std::cin >> test_count;
    while(test_count-- > 0){
        std::size_t size;
        std::cin >> size;
        std::vector<Point> points(size);
        for(Point& point: points) std::cin >> point.x >> point.y;
        const std::vector<Point> hull = convex_hull(std::move(points));
        std::cout << hull.size() << '\n';
        for(const Point& point: hull){
            std::cout << std::llround(point.x) << ' '
                << std::llround(point.y) << '\n';
        }
    }
}
