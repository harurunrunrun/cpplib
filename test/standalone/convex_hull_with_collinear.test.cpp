// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/2d/point_collection/convex_hull_with_collinear.hpp"
#include "geometry_2d_test_common.hpp"

#include <vector>

int main(){
    geometry_test_setup_output();
    int queries;
    std::cin >> queries;
    while(queries-- > 0){
        int count;
        std::cin >> count;
        std::vector<Point> points(static_cast<std::size_t>(count));
        for(Point& point: points) std::cin >> point.x >> point.y;
        const auto hull = convex_hull_with_collinear(std::move(points));
        std::cout << hull.size() << '\n';
        for(const Point& point: hull) geometry_test_print_point(point);
    }
}
