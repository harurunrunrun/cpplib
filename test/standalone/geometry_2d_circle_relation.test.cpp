// competitive-verifier: STANDALONE

#include <iostream>

#include "../../src/algorithm/geometry/2d/circle_relation_result.hpp"
#include "../../src/algorithm/geometry/2d/circle_relation_circles.hpp"
#include "../../src/algorithm/geometry/2d/circle_relation.hpp"

int main(){
    static_assert(CIRCLE_INSIDE == 0);
    static_assert(CIRCLE_INTERNALLY_TANGENT == 1);
    static_assert(CIRCLE_INTERSECT == 2);
    static_assert(CIRCLE_EXTERNALLY_TANGENT == 3);
    static_assert(CIRCLE_SEPARATE == 4);
    static_assert(CIRCLE_COINCIDENT == 5);

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
