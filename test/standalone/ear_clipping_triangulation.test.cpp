// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/2d/ear_clipping_triangle.hpp"
#include "../../src/algorithm/geometry/2d/ear_clipping_triangulation_detail.hpp"
#include "../../src/algorithm/geometry/2d/ear_clipping_triangulation_points.hpp"
#include "../../src/algorithm/geometry/2d/ear_clipping_triangulation.hpp"
#include "geometry_2d_triangulation_test_utils.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <vector>

int main(){
    const EarClippingTriangle sample{0, 1, 2};
    assert(sample[0] == 0);
    assert(sample[1] == 1);
    assert(sample[2] == 2);

    int queries;
    std::cin >> queries;
    while(queries-- > 0){
        int count;
        std::cin >> count;
        std::vector<Point> polygon(static_cast<std::size_t>(count));
        for(Point& point: polygon) std::cin >> point.x >> point.y;
        try{
            const auto triangles = ear_clipping_triangulation(polygon);
            const bool valid =
                geometry_2d_triangulation_test::validate(
                    polygon, triangles
                );
            std::cout << triangles.size() << ' ' << valid << '\n';
        }catch(const std::invalid_argument&){
            std::cout << "ERROR\n";
        }
    }
}
