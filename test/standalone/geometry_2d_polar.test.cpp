// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/2d/polar.hpp"
#include "geometry_2d_test_common.hpp"

int main(){
    geometry_test_setup_output();
    int queries;
    std::cin >> queries;
    while(queries-- > 0){
        long double radius, theta;
        std::cin >> radius >> theta;
        geometry_test_print_point(polar(radius, theta));
    }
}
