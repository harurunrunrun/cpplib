// competitive-verifier: PROBLEM https://onlinejudge.u-aizu.ac.jp/problems/CGL_4_A

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <vector>

#include "../../src/algorithm/geometry/2d/convex_hull_with_collinear.hpp"

int main(){
    int count;
    std::cin >> count;
    std::vector<Point> points(static_cast<std::size_t>(count));
    for(Point& point: points) std::cin >> point.x >> point.y;
    std::vector<Point> hull = convex_hull_with_collinear(std::move(points));
    if(!hull.empty()){
        const auto start = std::min_element(
            hull.begin(), hull.end(),
            [](const Point& first, const Point& second){
                if(first.y != second.y) return first.y < second.y;
                return first.x < second.x;
            }
        );
        std::rotate(hull.begin(), start, hull.end());
    }
    std::cout << hull.size() << '\n';
    for(const Point& point: hull){
        std::cout << std::llround(point.x) << ' '
                  << std::llround(point.y) << '\n';
    }
}
