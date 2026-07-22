// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/shape/plane_plane_intersection.hpp"
#include "geometry_3d_test_common.hpp"

int main(){
    return geometry3d_test_main([](){
        const Line3 line = plane_plane_intersection(
            {{1, 0, 0}, {1, 0, 0}},
            {{0, 2, 0}, {0, 1, 0}}
        );
        if(!geometry3d_close_value(line.a.x, 1) ||
            !geometry3d_close_value(line.a.y, 2) ||
            !geometry3d_close_value(line.b.x, 1) ||
            !geometry3d_close_value(line.b.y, 2)){
            return false;
        }

        const long double tiny = 1e-3000L;
        const Line3 near_parallel = plane_plane_intersection(
            {{0, 0, 0}, {1, 0, 0}},
            {{0, 1, 0}, {1, tiny, 0}}
        );
        if(near_parallel.a.x != 0.0L || near_parallel.a.y != 1.0L){
            return false;
        }

        const Line3 anisotropic = plane_plane_intersection(
            {{0, 0, 0}, {1, 0, 0}},
            {{0, tiny, 0}, {0, 1e3000L, tiny}}
        );
        if(anisotropic.a.x != 0.0L
            || std::abs(anisotropic.a.y / tiny - 1.0L) > 1e-18L){
            return false;
        }
        return on_plane({{0, 0, 0}, {1, 0, 0}}, near_parallel.b)
            && on_plane({{0, 1, 0}, {1, tiny, 0}}, near_parallel.b)
            && on_plane({{0, 0, 0}, {1, 0, 0}}, anisotropic.b)
            && on_plane({{0, tiny, 0}, {0, 1e3000L, tiny}}, anisotropic.b);
    });
}
