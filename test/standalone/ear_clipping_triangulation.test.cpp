// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/2d/ear_clipping_triangulation.hpp"
#include "geometry_2d_triangulation_test_utils.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <vector>

int main(){
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
