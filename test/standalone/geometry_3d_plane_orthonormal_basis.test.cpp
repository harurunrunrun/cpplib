// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/point_collection/plane_orthonormal_basis.hpp"
#include "geometry_3d_test_common.hpp"

int main(){
    return geometry3d_test_main([](){
        const auto basis = plane_orthonormal_basis(
            {{0, 0, 0}, {0, 0, 2}}
        );
        return geometry3d_close_point(basis.first, {0, 1, 0}) &&
            geometry3d_close_point(basis.second, {-1, 0, 0});
    });
}
