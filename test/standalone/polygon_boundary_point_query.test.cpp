// competitive-verifier: STANDALONE

#include <iostream>
#include <utility>
#include <vector>

#include "../../src/algorithm/geometry/polygon_boundary_points/polygon_boundary_point_query.hpp"

int main(){
    int n;
    int m;
    int unique_flag;
    if(!(std::cin >> n >> m >> unique_flag)) return 0;

    std::vector<Point> polygon(static_cast<std::size_t>(n));
    for(Point& point: polygon) std::cin >> point.x >> point.y;
    std::vector<Point> points(static_cast<std::size_t>(m));
    for(Point& point: points) std::cin >> point.x >> point.y;

    const std::vector<Point> original = polygon;
    const PolygonBoundaryPointQuery query(std::move(polygon));
    if(query.polygon() != original) return 2;

    const std::vector<int> result = query.enumerate(points, unique_flag != 0);
    std::cout << (query.uses_convex_fast_path() ? 1 : 0) << ' ' << result.size();
    for(int index: result) std::cout << ' ' << index;
    std::cout << '\n';
}
