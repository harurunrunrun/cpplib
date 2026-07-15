// competitive-verifier: STANDALONE

#include <variant>
#include "../../src/algorithm/geometry/3d/planar_intersection3.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        PlanarIntersection3 result = Polygon3{{}, {1, 0, 0}, {0, 1, 0}};
        return std::holds_alternative<Polygon3>(result) &&
            std::get<Polygon3>(result).size() == 3;
    });
}
