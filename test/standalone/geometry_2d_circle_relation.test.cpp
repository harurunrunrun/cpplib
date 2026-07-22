// competitive-verifier: STANDALONE

#include <iostream>

#include "../../src/algorithm/geometry/2d/predicate/circle_relation.hpp"

int main(){
    int query_count;
    std::cin >> query_count;
    while(query_count--){
        Circle first;
        Circle second;
        std::cin >> first.center.x >> first.center.y >> first.radius
                 >> second.center.x >> second.center.y >> second.radius;
        std::cout << circle_relation(first, second) << '\n';
    }
}
