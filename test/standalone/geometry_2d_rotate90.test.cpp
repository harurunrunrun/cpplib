// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/2d/point/rotate90.hpp"
#include "geometry_2d_test_common.hpp"

int main(){
    geometry_test_setup_output();
    int queries;
    std::cin >> queries;
    while(queries-- > 0){
        long double x, y;
        std::cin >> x >> y;
        geometry_test_print_point(rotate90({x, y}));
    }
}
