// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/2d/common_internal_tangents.hpp"

#include <iostream>

int main(){
    int queries;
    std::cin >> queries;
    while(queries-- > 0){
        Circle first, second;
        std::cin >> first.center.x >> first.center.y >> first.radius
                 >> second.center.x >> second.center.y >> second.radius;
        std::cout << common_internal_tangents(first, second).size() << '\n';
    }
}
