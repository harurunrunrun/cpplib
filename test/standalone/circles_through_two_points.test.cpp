// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/2d/shape/circles_through_two_points.hpp"
#include "geometry_2d_test_common.hpp"

#include <stdexcept>

int main(){
    geometry_test_setup_output();
    int queries;
    std::cin >> queries;
    while(queries-- > 0){
        Point first, second;
        long double radius;
        std::cin >> first.x >> first.y >> second.x >> second.y >> radius;
        try{
            const auto circles = circles_through_two_points(
                first, second, radius
            );
            std::cout << circles.size() << '\n';
            for(const Circle& circle: circles){
                std::cout << geometry_test_clean(circle.center.x) << ' '
                          << geometry_test_clean(circle.center.y) << ' '
                          << geometry_test_clean(circle.radius) << '\n';
            }
        }catch(const std::domain_error&){
            std::cout << "ERR\n";
        }
    }
}
