// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/tetrahedron_inradius.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        const Tetrahedron3 regular{
            {1, 1, 1}, {1, -1, -1}, {-1, 1, -1}, {-1, -1, 1}
        };
        if(!geometry3d_api_close(
            tetrahedron_inradius(regular), 1 / std::sqrt(3.0L)
        )) return false;
        try{ (void)tetrahedron_inradius({{}, {1, 0, 0}, {0, 1, 0}, {1, 1, 0}}); }
        catch(const std::invalid_argument&){ return true; }
        return false;
    });
}
