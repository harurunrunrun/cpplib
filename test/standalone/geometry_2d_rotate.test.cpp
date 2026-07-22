// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/2d/point/rotate.hpp"
#include "geometry_2d_test_common.hpp"

int main(){
    geometry_test_setup_output();
    int queries;
    std::cin >> queries;
    while(queries-- > 0){
        long double x, y, theta;
        std::cin >> x >> y >> theta;
        geometry_test_print_point(rotate({x, y}, theta));
    }
}
