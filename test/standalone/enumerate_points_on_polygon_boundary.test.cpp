// competitive-verifier: STANDALONE

#include <iostream>
#include <vector>

#include "../../src/algorithm/geometry/2d/point_collection/enumerate_points_on_polygon_boundary.hpp"

int main(){
    int n;
    int m;
    int unique_flag;
    if(!(std::cin >> n >> m >> unique_flag)) return 0;

    std::vector<Point> polygon(static_cast<std::size_t>(n));
    for(Point& point: polygon) std::cin >> point.x >> point.y;
    std::vector<Point> points(static_cast<std::size_t>(m));
    for(Point& point: points) std::cin >> point.x >> point.y;

    const std::vector<int> result = enumerate_points_on_polygon_boundary(
        polygon,
        points,
        unique_flag != 0
    );
    std::cout << result.size();
    for(int index: result) std::cout << ' ' << index;
    std::cout << '\n';
}
