// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/2d/point/line_intersection.hpp"
#include "geometry_2d_test_common.hpp"

#include <stdexcept>

int main(){
    geometry_test_setup_output();
    int queries;
    std::cin >> queries;
    while(queries-- > 0){
        long double ax, ay, bx, by, cx, cy, dx, dy;
        std::cin >> ax >> ay >> bx >> by >> cx >> cy >> dx >> dy;
        try{
            geometry_test_print_point(line_intersection({{ax, ay}, {bx, by}},
                                                         {{cx, cy}, {dx, dy}}));
        }catch(const std::exception&){
            std::cout << "ERR\n";
        }
    }
}
