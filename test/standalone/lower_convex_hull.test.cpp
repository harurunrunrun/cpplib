// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/2d/lower_convex_hull.hpp"
#include "geometry_2d_test_common.hpp"

#include <vector>

int main(){
    geometry_test_setup_output();
    int queries;
    std::cin >> queries;
    while(queries-- > 0){
        int count, keep;
        std::cin >> count >> keep;
        std::vector<Point> points(static_cast<std::size_t>(count));
        for(Point& point: points) std::cin >> point.x >> point.y;
        const auto chain = lower_convex_hull(std::move(points), keep != 0);
        std::cout << chain.size() << '\n';
        for(const Point& point: chain) geometry_test_print_point(point);
    }
}
