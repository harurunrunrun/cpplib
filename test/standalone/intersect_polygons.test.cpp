// competitive-verifier: STANDALONE

#include <cassert>
#include <cstddef>
#include <iostream>
#include <vector>

#include "../../src/algorithm/geometry/2d/predicate/point_set_point_set/intersect_polygons.hpp"

int main(){
    int first_count, second_count;
    std::vector<Point> first;
    std::vector<Point> second;
    if(std::cin >> first_count >> second_count){
        first.resize(static_cast<std::size_t>(first_count));
        second.resize(static_cast<std::size_t>(second_count));
        for(Point& point: first) std::cin >> point.x >> point.y;
        for(Point& point: second) std::cin >> point.x >> point.y;
    }else{
        first = {{0, 0}, {3, 0}, {3, 1}, {1, 1}, {1, 3}, {0, 3}};
        second = {{0.2L, 0.2L}, {0.8L, 0.2L}, {0.8L, 0.8L}, {0.2L, 0.8L}};
    }
    const std::vector<Point> notch{
        {1.2L, 1.2L}, {1.8L, 1.2L}, {1.8L, 1.8L}, {1.2L, 1.8L}
    };
    assert(!intersect_polygons(
        {{0, 0}, {3, 0}, {3, 1}, {1, 1}, {1, 3}, {0, 3}}, notch
    ));
    std::cout << intersect_polygons(first, second) << '\n';
}
