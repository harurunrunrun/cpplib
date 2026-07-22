// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/scalar/tetrahedron_inradius.hpp"
#include "geometry_3d_center_radius_extreme_test_common.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        const Tetrahedron3 regular{
            {1, 1, 1}, {1, -1, -1}, {-1, 1, -1}, {-1, -1, 1}
        };
        if(!geometry3d_api_close(
            tetrahedron_inradius(regular), 1 / std::sqrt(3.0L)
        )) return false;
        const long double factor =
            1.0L / (3.0L + std::sqrt(3.0L));
        for(const long double scale: {1e3000L, 1e-3000L}){
            if(!geometry3d_extreme_relative_close(
                tetrahedron_inradius(
                    geometry3d_extreme_right_tetrahedron(scale)
                ),
                factor * scale
            )){
                return false;
            }
        }
        const auto translated =
            geometry3d_extreme_translated_tetrahedron();
        if(!geometry3d_extreme_relative_close(
            tetrahedron_inradius(translated.tetrahedron),
            factor * translated.edge
        )){
            return false;
        }
        try{ (void)tetrahedron_inradius({{}, {1, 0, 0}, {0, 1, 0}, {1, 1, 0}}); }
        catch(const std::invalid_argument&){ return true; }
        return false;
    });
}
