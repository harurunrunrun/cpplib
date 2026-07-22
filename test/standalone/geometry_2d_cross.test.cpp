// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/2d/scalar/cross.hpp"
#include "geometry_2d_test_common.hpp"

int main(){
    geometry_test_setup_output();
    int queries;
    std::cin >> queries;
    while(queries-- > 0){
        long double ax, ay, bx, by;
        std::cin >> ax >> ay >> bx >> by;
        geometry_test_print_real(cross({ax, ay}, {bx, by}));
    }
}
