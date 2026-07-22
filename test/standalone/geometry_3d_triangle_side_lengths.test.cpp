// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/scalar/triangle_side_lengths.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        const auto lengths = triangle_side_lengths({{}, {3, 0, 0}, {0, 4, 0}});
        return geometry3d_api_close(lengths[0], 3) &&
            geometry3d_api_close(lengths[1], 5) &&
            geometry3d_api_close(lengths[2], 4);
    });
}
