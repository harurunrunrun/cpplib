// competitive-verifier: STANDALONE

#include <iostream>

#include "../../src/algorithm/geometry/2d/intersect_circle_circle.hpp"

int main(){
    int query_count;
    std::cin >> query_count;
    while(query_count--){
        Circle first;
        Circle second;
        std::cin >> first.center.x >> first.center.y >> first.radius
                 >> second.center.x >> second.center.y >> second.radius;
        std::cout << static_cast<int>(intersect_circle_circle(first, second))
                  << '\n';
    }
}
