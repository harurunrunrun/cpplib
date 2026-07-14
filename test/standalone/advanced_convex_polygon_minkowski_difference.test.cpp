// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/2d/advanced/convex_polygon_minkowski_difference.hpp"
#include "geometry_2d_test_common.hpp"

#include <vector>

static std::vector<Point> read_polygon(){
    int count;
    std::cin >> count;
    std::vector<Point> polygon(static_cast<std::size_t>(count));
    for(Point& point: polygon) std::cin >> point.x >> point.y;
    return polygon;
}

int main(){
    geometry_test_setup_output();
    int queries;
    std::cin >> queries;
    while(queries-- > 0){
        std::vector<Point> first = read_polygon();
        std::vector<Point> second = read_polygon();
        const auto result = convex_polygon_minkowski_difference(
            std::move(first), std::move(second)
        );
        std::cout << result.size() << '\n';
        for(const Point& point: result) geometry_test_print_point(point);
    }
}
