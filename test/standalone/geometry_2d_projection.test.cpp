// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/2d/projection.hpp"
#include "geometry_2d_test_common.hpp"

#include <stdexcept>

int main(){
    geometry_test_setup_output();
    int queries;
    std::cin >> queries;
    while(queries-- > 0){
        long double ax, ay, bx, by, px, py;
        std::cin >> ax >> ay >> bx >> by >> px >> py;
        try{
            geometry_test_print_point(projection({{ax, ay}, {bx, by}}, {px, py}));
        }catch(const std::invalid_argument&){
            std::cout << "ERR\n";
        }
    }
}
