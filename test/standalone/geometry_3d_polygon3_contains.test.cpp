// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/predicate/polygon3_contains.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        const Polygon3 polygon{{0, 0, 5}, {4, 0, 5}, {4, 2, 5}, {0, 2, 5}};
        return polygon3_contains(polygon, {2, 1, 5}) == 2
            && polygon3_contains(polygon, {0, 1, 5}) == 1
            && polygon3_contains(polygon, {2, 1, 6}) == 0;
    });
}
