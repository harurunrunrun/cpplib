// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/polygon3_to_2d.hpp"
#include "../../src/algorithm/geometry/3d/from_plane_coordinates.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        const Polygon3 polygon{{1, 2, 3}, {3, 2, 3}, {3, 4, 3}, {1, 4, 3}};
        const auto [system, projected] = polygon3_to_2d(polygon);
        if(projected.size() != polygon.size()) return false;
        for(std::size_t index = 0; index < polygon.size(); ++index){
            if(!geometry3d_api_close(
                from_plane_coordinates(system, projected[index]), polygon[index]
            )) return false;
        }
        return true;
    });
}
