// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/scalar/signed_dihedral_angle.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        const Plane3 first{{}, {0, 1, 0}}, second{{}, {0, 0, 1}};
        return geometry3d_api_close(
            signed_dihedral_angle(first, second, {1, 0, 0}), GEOMETRY3D_PI / 2
        ) && geometry3d_api_close(
            signed_dihedral_angle(first, second, {-1, 0, 0}), -GEOMETRY3D_PI / 2
        );
    });
}
