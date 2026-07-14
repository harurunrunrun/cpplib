// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/2d/on_circle.hpp"

#include <iostream>

int main(){
    int queries;
    std::cin >> queries;
    while(queries-- > 0){
        Circle circle;
        Point point;
        std::cin >> circle.center.x >> circle.center.y >> circle.radius
                 >> point.x >> point.y;
        std::cout << on_circle(circle, point) << '\n';
    }
}
