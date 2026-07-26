// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/convex_polygon3_cut.hpp"
#include "../../src/algorithm/geometry/3d/polygon3_area.hpp"
#include "../../src/algorithm/geometry/3d/polygon3_contains.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        const Polygon3 result = convex_polygon3_cut(
            {{0, 0, 0}, {2, 0, 0}, {2, 2, 0}, {0, 2, 0}},
            {{1, 0, 0}, {1, 0, 0}}
        );
        return result.size() == 4
            && geometry3d_api_close(polygon3_area(result), 2)
            && polygon3_contains(result, {1.5L, 1, 0}) == 2;
    });
}
