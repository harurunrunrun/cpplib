// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/2d/shape/minimum_enclosing_rectangle.hpp"
#include "../../src/algorithm/geometry/2d/scalar/distance.hpp"
#include "geometry_2d_test_common.hpp"

#include <cmath>
#include <vector>

int main(){
    geometry_test_setup_output();
    int queries;
    std::cin >> queries;
    while(queries-- > 0){
        int count;
        std::cin >> count;
        std::vector<Point> polygon(static_cast<std::size_t>(count));
        for(Point& point: polygon) std::cin >> point.x >> point.y;
        const auto result = minimum_enclosing_rectangle(std::move(polygon));
        const bool consistent = !result.valid || (
            std::abs(result.width * result.height - result.area) < 1e-8L
            && std::abs(distance(result.corners[0], result.corners[1])
                - result.width) < 1e-8L
            && std::abs(distance(result.corners[1], result.corners[2])
                - result.height) < 1e-8L
        );
        std::cout << result.valid << ' ' << consistent << ' '
                  << geometry_test_clean(result.area) << ' '
                  << geometry_test_clean(result.width) << ' '
                  << geometry_test_clean(result.height) << '\n';
    }
}
