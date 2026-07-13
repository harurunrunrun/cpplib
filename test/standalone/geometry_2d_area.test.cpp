// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/2d/area.hpp"
#include "geometry_2d_test_common.hpp"

#include <vector>

int main(){
    geometry_test_setup_output();
    int queries;
    std::cin >> queries;
    while(queries-- > 0){
        int size;
        std::cin >> size;
        std::vector<Point> polygon(static_cast<std::size_t>(size));
        for(Point& point: polygon) std::cin >> point.x >> point.y;
        geometry_test_print_real(area(polygon));
    }
}
