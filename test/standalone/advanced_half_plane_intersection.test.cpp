// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/2d/shape/half_plane_intersection.hpp"
#include "advanced_geometry_test_common.hpp"

#include <stdexcept>
#include <utility>
#include <vector>

int main(){
    int queries;
    std::cin >> queries;
    while(queries-- > 0){
        int count;
        std::cin >> count;
        std::vector<Line> half_planes(static_cast<std::size_t>(count));
        for(Line& half_plane: half_planes){
            std::cin >> half_plane.a.x >> half_plane.a.y
                     >> half_plane.b.x >> half_plane.b.y;
        }
        try{
            const HalfPlaneIntersectionResult result =
                half_plane_intersection(std::move(half_planes));
            if(result.kind == HALF_PLANE_EMPTY) std::cout << "E\n";
            else if(result.kind == HALF_PLANE_UNBOUNDED) std::cout << "U\n";
            else{
                std::cout << "B ";
                advanced_geometry_print_polygon(result.polygon);
            }
        }catch(const std::exception&){
            std::cout << "ERR\n";
        }
    }
}
