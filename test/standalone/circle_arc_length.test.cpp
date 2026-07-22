// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/2d/scalar/circle_arc_length.hpp"
#include "geometry_2d_test_common.hpp"

#include <stdexcept>

int main(){
    geometry_test_setup_output();
    int queries;
    std::cin >> queries;
    while(queries-- > 0){
        Circle circle;
        long double angle;
        std::cin >> circle.center.x >> circle.center.y >> circle.radius >> angle;
        try{
            geometry_test_print_real(circle_arc_length(circle, angle));
        }catch(const std::invalid_argument&){
            std::cout << "ERR\n";
        }
    }
}
