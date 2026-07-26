// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/2d/minimum_enclosing_rectangle_result.hpp"
#include "../../src/algorithm/geometry/2d/minimum_enclosing_rectangle_normalized.hpp"
#include "../../src/algorithm/geometry/2d/minimum_enclosing_rectangle_points.hpp"
#include "../../src/algorithm/geometry/2d/minimum_enclosing_rectangle.hpp"
#include "../../src/algorithm/geometry/2d/distance.hpp"
#include "geometry_2d_test_common.hpp"

#include <cmath>
#include <stdexcept>
#include <vector>

namespace{

void self_check(){
    const MinimumEnclosingRectangleResult invalid;
    if(invalid.valid || invalid.width != 0.0L || invalid.height != 0.0L
        || invalid.area != 0.0L || invalid.corners.size() != 4){
        throw std::runtime_error("a default rectangle result must be invalid");
    }

    const NormalizedConvexPolygon polygon(
        std::vector<Point>{{0, 0}, {4, 0}, {4, 2}, {0, 2}}
    );
    const MinimumEnclosingRectangleResult normalized_result =
        minimum_enclosing_rectangle(polygon);
    if(!normalized_result.valid
        || std::abs(normalized_result.area - 8.0L) > 1e-8L
        || std::abs(normalized_result.width * normalized_result.height
            - normalized_result.area) > 1e-8L){
        throw std::runtime_error("the normalized rectangle overload failed");
    }
}

}  // namespace

int main(){
    geometry_test_setup_output();
    self_check();
    int queries;
    std::cin >> queries;
    while(queries-- > 0){
        int count;
        std::cin >> count;
        std::vector<Point> polygon(static_cast<std::size_t>(count));
        for(Point& point: polygon) std::cin >> point.x >> point.y;
        const MinimumEnclosingRectangleResult result =
            minimum_enclosing_rectangle(std::move(polygon));
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
