// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/2d/unit.hpp"
#include "geometry_2d_test_common.hpp"

#include <stdexcept>

int main(){
    geometry_test_setup_output();
    int queries;
    std::cin >> queries;
    while(queries-- > 0){
        long double x, y;
        std::cin >> x >> y;
        try{
            geometry_test_print_point(unit({x, y}));
        }catch(const std::invalid_argument&){
            std::cout << "ERR\n";
        }
    }
}
