// competitive-verifier: STANDALONE

#include <cmath>
#include <stdexcept>

#include "../../src/algorithm/geometry/3d/scalar/triangle/area.hpp"
#include "geometry_3d_test_common.hpp"

int main(){
    return geometry3d_test_main([](){
        if(!geometry3d_close_value(
            area({{0, 0, 0}, {2, 0, 0}, {0, 2, 0}}), 2
        )) return false;
        // Finite but huge coordinates whose area is not representable.
        const long double huge = 1e2470L;
        try{
            static_cast<void>(area({{0, 0, 0}, {huge, 0, 0}, {0, huge, 0}}));
            return false;
        }catch(const std::overflow_error&){
        }
        try{
            static_cast<void>(area({{0, 0, 0}, {NAN, 0, 0}, {0, 1, 0}}));
            return false;
        }catch(const std::invalid_argument&){
        }
        return true;
    });
}
