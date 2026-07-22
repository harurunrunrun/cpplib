// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/2d/scalar/circle_circle_intersection_count.hpp"

#include <iostream>

int main(){
    int queries;
    std::cin >> queries;
    while(queries-- > 0){
        Circle first, second;
        std::cin >> first.center.x >> first.center.y >> first.radius
                 >> second.center.x >> second.center.y >> second.radius;
        std::cout << circle_circle_intersection_count(first, second) << '\n';
    }
}
