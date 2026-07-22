// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/scalar/ray_segment_distance.hpp"
#include "geometry_3d_linear_closest_extreme_test_common.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        return geometry3d_api_close(distance(
            Ray3{{}, {1, 0, 0}}, Segment3{{-1, -1, 0}, {-1, 1, 0}}
        ), 1) && geometry3d_api_close(distance(
            Segment3{{2, -1, 0}, {2, 1, 0}}, Ray3{{}, {1, 0, 0}}
        ), 0) && geometry3d_linear_test_close(
            distance(
                geometry3d_linear_test_ray(),
                Segment3{
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
