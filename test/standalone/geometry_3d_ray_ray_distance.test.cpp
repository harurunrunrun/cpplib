// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/ray_ray_distance.hpp"
#include "geometry_3d_linear_closest_extreme_test_common.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        return geometry3d_api_close(distance(
            Ray3{{}, {1, 0, 0}}, Ray3{{0, 2, 0}, {0, 3, 0}}
        ), 2) && geometry3d_api_close(distance(
            Ray3{{}, {1, 0, 0}}, Ray3{{1, -1, 0}, {1, 1, 0}}
        ), 0) && geometry3d_linear_test_close(
            distance(
                geometry3d_linear_test_ray(),
                Ray3{
                    {0, GEOMETRY3D_LINEAR_TEST_LOCAL,
                     GEOMETRY3D_LINEAR_TEST_TRANSLATION},
                    {GEOMETRY3D_LINEAR_TEST_LOCAL,
                     GEOMETRY3D_LINEAR_TEST_LOCAL,
                     GEOMETRY3D_LINEAR_TEST_TRANSLATION}
                }
            ),
            GEOMETRY3D_LINEAR_TEST_LOCAL
        );
    });
}
