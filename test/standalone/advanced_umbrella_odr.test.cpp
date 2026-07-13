// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/advanced_convex_geometry.hpp"
#include "../../src/algorithm/geometry/advanced_convex_geometry.hpp"
#include "../../src/algorithm/geometry/advanced/base.hpp"
#include "../../src/algorithm/geometry/advanced/detail.hpp"
#include "../../src/algorithm/geometry/advanced/half_plane_contains.hpp"
#include "../../src/algorithm/geometry/advanced/normalize_convex_polygon.hpp"
#include "../../src/algorithm/geometry/advanced/convex_polygon_minkowski_sum.hpp"
#include "../../src/algorithm/geometry/advanced/half_plane_intersection.hpp"
#include "../../src/algorithm/geometry/advanced/convex_polygon_intersection.hpp"

#include <cstddef>
#include <iostream>
#include <vector>

bool advanced_umbrella_contains_a(const Line&, const Point&);
std::size_t advanced_umbrella_normalized_size_b(std::vector<Point>);

int main(){
    int queries;
    std::cin >> queries;
    while(queries-- > 0){
        const bool contains = advanced_umbrella_contains_a(
            {{0, 0}, {2, 0}}, {0, 1}
        );
        const std::size_t normalized_size = advanced_umbrella_normalized_size_b({
            {0, 0}, {2, 0}, {2, 2}, {0, 2}
        });
        const std::vector<Point> sum = convex_polygon_minkowski_sum(
            {{1, 1}}, {{2, 3}}
        );
        const HalfPlaneIntersectionResult whole_plane = half_plane_intersection({});
        const std::vector<Point> intersection = convex_polygon_intersection(
            {{1, 2}}, {{1, 2}}
        );
        std::cout << contains << ' ' << normalized_size << ' ' << sum.size()
                  << ' ' << std::llround(sum[0].x * 1000000.0L)
                  << ' ' << std::llround(sum[0].y * 1000000.0L)
                  << ' ' << static_cast<int>(whole_plane.kind)
                  << ' ' << intersection.size() << '\n';
    }
}
