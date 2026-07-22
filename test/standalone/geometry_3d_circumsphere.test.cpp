// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/shape/circumsphere.hpp"
#include "geometry_3d_center_radius_extreme_test_common.hpp"
#include "geometry_3d_test_common.hpp"

int main(){
    return geometry3d_test_main([](){
        const Sphere3 sphere = circumsphere({
            {0, 0, 0}, {1, 0, 0}, {0, 1, 0}, {0, 0, 1}
        });
        if(!geometry3d_close_point(sphere.center, {0.5L, 0.5L, 0.5L})
            || !geometry3d_close_value(
                sphere.radius, std::sqrt(3.0L) / 2
            )) return false;
        for(const long double scale: {1e3000L, 1e-3000L}){
            const Sphere3 extreme = circumsphere(
                geometry3d_extreme_right_tetrahedron(scale)
            );
            const long double coordinate = scale / 2;
            if(!geometry3d_extreme_relative_close(
                    extreme.center.x, coordinate)
                || !geometry3d_extreme_relative_close(
                    extreme.center.y, coordinate)
                || !geometry3d_extreme_relative_close(
                    extreme.center.z, coordinate)
                || !geometry3d_extreme_relative_close(
                    extreme.radius, std::sqrt(3.0L) * coordinate)){
                return false;
            }
        }
        const auto translated =
            geometry3d_extreme_translated_tetrahedron();
        const Sphere3 translated_sphere =
            circumsphere(translated.tetrahedron);
        const long double expected_center =
            translated.translation + translated.edge / 2;
        return translated_sphere.center.x == expected_center
            && translated_sphere.center.y == expected_center
            && translated_sphere.center.z == expected_center
            && geometry3d_extreme_relative_close(
                translated_sphere.radius,
                std::sqrt(3.0L) * translated.edge / 2
            );
    });
}
