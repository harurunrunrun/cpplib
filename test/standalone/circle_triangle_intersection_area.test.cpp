// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/2d/circle_triangle_intersection_area.hpp"
#include "geometry_2d_test_common.hpp"

int main(){
    geometry_test_setup_output();
    int queries;
    std::cin >> queries;
    while(queries-- > 0){
        Circle circle;
        Point first, second, third;
        std::cin >> circle.center.x >> circle.center.y >> circle.radius
                 >> first.x >> first.y >> second.x >> second.y
                 >> third.x >> third.y;
        geometry_test_print_real(circle_triangle_intersection_area(
            circle, first, second, third
        ));
    }
}
