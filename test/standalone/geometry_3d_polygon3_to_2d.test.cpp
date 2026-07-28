// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/type/definition/polygon3_to_2d_with_plane_coordinate_system.hpp"
#include "../../src/algorithm/geometry/3d/type/definition/polygon3_to_2d_without_plane_coordinate_system.hpp"
#include "../../src/algorithm/geometry/3d/aggregate/all/polygon3_to_2d.hpp"
#include "../../src/algorithm/geometry/3d/point/point/from_plane_coordinates.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        const Polygon3 polygon{{1, 2, 3}, {3, 2, 3}, {3, 4, 3}, {1, 4, 3}};
        const auto [system, automatic] = polygon3_to_2d(polygon);
        const std::vector<Point> specified = polygon3_to_2d(polygon, system);
        if(automatic.size() != polygon.size() ||
            specified.size() != polygon.size()){
            return false;
        }
        for(std::size_t index = 0; index < polygon.size(); ++index){
            if(!geometry3d_api_close(automatic[index].x, specified[index].x) ||
                !geometry3d_api_close(automatic[index].y, specified[index].y) ||
                !geometry3d_api_close(
                    from_plane_coordinates(system, automatic[index]),
                    polygon[index]
                )){
                return false;
            }
        }
        try{
            (void)polygon3_to_2d(Polygon3{{}, {1, 0, 0}});
            return false;
        }catch(const std::invalid_argument&){
        }
        return true;
    });
}
