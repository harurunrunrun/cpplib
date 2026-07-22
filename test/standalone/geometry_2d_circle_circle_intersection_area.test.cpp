// competitive-verifier: STANDALONE

#include <iostream>

#include "../../src/algorithm/geometry/2d/scalar/circle_circle_intersection_area.hpp"
#include "geometry_2d_test_util.hpp"

int main(){
    int query_count;
    std::cin >> query_count;
    while(query_count--){
        Circle first;
        Circle second;
        std::cin >> first.center.x >> first.center.y >> first.radius
                 >> second.center.x >> second.center.y >> second.radius;
        print_geometry_scalar(circle_circle_intersection_area(first, second));
        std::cout << '\n';
    }
}
