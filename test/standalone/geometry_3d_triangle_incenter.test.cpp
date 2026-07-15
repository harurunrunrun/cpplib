// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/triangle_incenter.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        if(!geometry3d_api_close(
            triangle_incenter({{0, 0, 2}, {3, 0, 2}, {0, 4, 2}}),
            {1, 1, 2}
        )) return false;
        try{ (void)triangle_incenter({{}, {1, 0, 0}, {2, 0, 0}}); }
        catch(const std::invalid_argument&){ return true; }
        return false;
    });
}
