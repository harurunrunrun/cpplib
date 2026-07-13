// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/advanced/convex_polygon_minkowski_sum.hpp"
#include "advanced_geometry_test_common.hpp"

#include <stdexcept>
#include <utility>

int main(){
    int queries;
    std::cin >> queries;
    while(queries-- > 0){
        std::vector<Point> first = advanced_geometry_read_polygon();
        std::vector<Point> second = advanced_geometry_read_polygon();
        try{
            advanced_geometry_print_polygon(
                convex_polygon_minkowski_sum(std::move(first), std::move(second))
            );
        }catch(const std::invalid_argument&){
            std::cout << "ERR\n";
        }
    }
}
