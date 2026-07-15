// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/epa_3d.hpp"
#include "geometry_3d_collision_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64& random, std::size_t rounds){
        const Point3 half{1, 1, 1};
        EPAResult3 result = epa_3d(
            geometry3d_collision_box({}, half),
            geometry3d_collision_box({1.5L, 0, 0}, half)
        );
        if(!result.intersects || !result.converged
            || !geometry3d_api_close(result.penetration_depth, 0.5L, 1e-7L)
            || !geometry3d_api_close(result.normal, {1, 0, 0}, 1e-7L)){
            return false;
        }
        if(epa_3d(
            geometry3d_collision_box({}, half),
            geometry3d_collision_box({4, 0, 0}, half)
        ).intersects) return false;
        const EPAResult3 tetra_result = epa_3d(
            geometry3d_collision_tetra(),
            geometry3d_collision_tetra({0.1L, 0.1L, 0.1L})
        );
        if(!tetra_result.intersects || !tetra_result.converged
            || !(tetra_result.penetration_depth > 0.0L)) return false;
        std::uniform_real_distribution<long double> offset(-1.8L, 1.8L);
        for(std::size_t iteration = 0; iteration < rounds; ++iteration){
            const Point3 center{offset(random), offset(random), offset(random)};
            const long double expected = std::min({
                2.0L - std::abs(center.x), 2.0L - std::abs(center.y),
                2.0L - std::abs(center.z)
            });
            result = epa_3d(
                geometry3d_collision_box({}, half),
                geometry3d_collision_box(center, half)
            );
            if(!result.intersects
                || !geometry3d_api_close(
                    result.penetration_depth, expected, 1.0e-7L
                )) return false;
        }
        return true;
    });
}
