// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/2d/predicate/contains_circle.hpp"

#include <iostream>

int main(){
    int queries;
    std::cin >> queries;
    while(queries-- > 0){
        Circle circle;
        Point point;
        std::cin >> circle.center.x >> circle.center.y >> circle.radius
                 >> point.x >> point.y;
        std::cout << contains_circle(circle, point) << '\n';
    }
}
