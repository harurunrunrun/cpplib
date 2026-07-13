// competitive-verifier: STANDALONE

#include <iostream>
#include <stdexcept>

#include "../../src/algorithm/geometry/2d/circle_circle_cross_points.hpp"
#include "geometry_2d_test_util.hpp"

int main(){
    int query_count;
    std::cin >> query_count;
    while(query_count--){
        Circle first;
        Circle second;
        std::cin >> first.center.x >> first.center.y >> first.radius
                 >> second.center.x >> second.center.y >> second.radius;
        try{
            print_geometry_points(circle_circle_cross_points(first, second));
        }catch(const std::domain_error&){
            std::cout << "ERROR\n";
        }
    }
}
