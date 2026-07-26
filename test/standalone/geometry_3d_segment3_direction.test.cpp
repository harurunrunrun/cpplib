// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/segment3_direction.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        if(!geometry3d_api_close(
            segment3_direction({{1, 2, 3}, {4, 6, 8}}), {3, 4, 5}
        )) return false;
        try{ (void)segment3_direction({{1, 2, 3}, {1, 2, 3}}); }
        catch(const std::invalid_argument&){ return true; }
        return false;
    });
}
