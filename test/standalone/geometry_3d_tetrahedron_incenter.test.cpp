// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/point/tetrahedron_incenter.hpp"
#include "geometry_3d_center_radius_extreme_test_common.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        const Tetrahedron3 regular{
            {1, 1, 1}, {1, -1, -1}, {-1, 1, -1}, {-1, -1, 1}
        };
        if(!geometry3d_api_close(tetrahedron_incenter(regular), {})) return false;
        const long double factor =
            1.0L / (3.0L + std::sqrt(3.0L));
        for(const long double scale: {1e3000L, 1e-3000L}){
            const long double expected = factor * scale;
            const Point3 center = tetrahedron_incenter(
                geometry3d_extreme_right_tetrahedron(scale)
            );
            if(!geometry3d_extreme_relative_close(center.x, expected)
                || !geometry3d_extreme_relative_close(center.y, expected)
                || !geometry3d_extreme_relative_close(
                    center.z, expected
                )){
                return false;
            }
        }
        const auto translated =
            geometry3d_extreme_translated_tetrahedron();
        const Point3 translated_center =
            tetrahedron_incenter(translated.tetrahedron);
        const long double expected_translated = std::fma(
            factor, translated.edge, translated.translation
        );
        if(translated_center.x != expected_translated
            || translated_center.y != expected_translated
            || translated_center.z != expected_translated){
            return false;
        }
        try{ (void)tetrahedron_incenter({{}, {1, 0, 0}, {0, 1, 0}, {1, 1, 0}}); }
        catch(const std::invalid_argument&){ return true; }
        return false;
    });
}
