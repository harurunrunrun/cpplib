// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/2d/advanced/normalize_convex_polygon.hpp"
#include "advanced_geometry_test_common.hpp"

#include <stdexcept>
#include <utility>

int main(){
    int queries;
    std::cin >> queries;
    while(queries-- > 0){
        try{
            advanced_geometry_print_polygon(
                normalize_convex_polygon(advanced_geometry_read_polygon())
            );
        }catch(const std::invalid_argument&){
            std::cout << "ERR\n";
        }
    }
}
