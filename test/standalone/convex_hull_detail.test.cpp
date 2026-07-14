// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/2d/convex_hull_detail.hpp"
#include "geometry_2d_test_common.hpp"

#include <vector>

int main(){
    geometry_test_setup_output();
    int count;
    std::cin >> count;
    std::vector<Point> points(static_cast<std::size_t>(count));
    for(Point& point: points) std::cin >> point.x >> point.y;
    const auto sorted = convex_hull_detail::sorted_unique_points(
        std::move(points)
    );
    const auto lower = convex_hull_detail::build_chain_from_sorted(
        sorted, false, true
    );
    const auto upper = convex_hull_detail::build_chain_from_sorted(
        sorted, true, true
    );
    const auto hull = convex_hull_detail::build_convex_hull(sorted, false);
    for(const auto* sequence: {&sorted, &lower, &upper, &hull}){
        std::cout << sequence->size() << '\n';
        for(const Point& point: *sequence) geometry_test_print_point(point);
    }
}
