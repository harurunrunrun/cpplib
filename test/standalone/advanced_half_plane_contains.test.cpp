// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/2d/advanced/half_plane_contains.hpp"
#include "advanced_geometry_test_common.hpp"

#include <stdexcept>

int main(){
    int queries;
    std::cin >> queries;
    while(queries-- > 0){
        Line half_plane;
        Point point;
        std::cin >> half_plane.a.x >> half_plane.a.y
                 >> half_plane.b.x >> half_plane.b.y
                 >> point.x >> point.y;
        try{
            std::cout << half_plane_contains(half_plane, point) << '\n';
        }catch(const std::invalid_argument&){
            std::cout << "ERR\n";
        }
    }
}
