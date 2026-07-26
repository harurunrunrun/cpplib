// competitive-verifier: STANDALONE

#include <cassert>
#include <cstddef>
#include <iostream>
#include <vector>

#include "../../src/algorithm/geometry/2d/minimum_enclosing_circle_result.hpp"
#include "../../src/algorithm/geometry/2d/minimum_enclosing_circle_points.hpp"
#include "../../src/algorithm/geometry/2d/minimum_enclosing_circle.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    const MinimumEnclosingCircleResult empty_result;
    const Point origin{0, 0};
    assert(empty_result.support.size() == 3);
    assert(empty_result.support_size == 0);
    assert(
        empty_result.containment(origin)
        == MinimumEnclosingCircleContainment::OUTSIDE
    );
    assert(!empty_result.contains(origin));
    assert(!empty_result.on_boundary(origin));
    const Circle empty_circle = empty_result.circle();
    assert(empty_circle.center == origin && empty_circle.radius == 0);

    int size;
    std::cin >> size;
    std::vector<Point> points(static_cast<std::size_t>(size));
    for(Point& point: points) std::cin >> point.x >> point.y;
    const MinimumEnclosingCircleResult result = minimum_enclosing_circle(points);
    const Circle circle = result.circle();
    assert(circle.radius >= 0);
    for(const Point& point: points) assert(result.contains(point));
    for(const Point& point: points){
        std::cout << (result.on_boundary(point) ? '1' : '0');
    }
    std::cout << '\n';
}
