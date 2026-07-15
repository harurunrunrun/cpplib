// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/triangle_inradius.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        if(!geometry3d_api_close(
            triangle_inradius({{}, {3, 0, 0}, {0, 4, 0}}), 1
        )) return false;
        try{ (void)triangle_inradius({{}, {1, 1, 1}, {2, 2, 2}}); }
        catch(const std::invalid_argument&){ return true; }
        return false;
    });
}
