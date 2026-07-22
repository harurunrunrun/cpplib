// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <vector>

#include "../../src/algorithm/geometry/2d/query/minimum_enclosing_circle_boundary_flags.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int size;
    if(!(std::cin >> size)){
        assert(minimum_enclosing_circle_boundary_flags({}).empty());
        const std::vector<Point> points = {{-1, 0}, {1, 0}, {0, 0}};
        const auto result = minimum_enclosing_circle_boundary_flags(points);
        assert(result[0] && result[1] && !result[2]);
        return 0;
    }
    std::vector<Point> points(static_cast<std::size_t>(size));
    for(Point& point: points) std::cin >> point.x >> point.y;
    const auto answer = minimum_enclosing_circle_boundary_flags(points);
    for(bool value: answer) std::cout << static_cast<int>(value);
    std::cout << '\n';
}
