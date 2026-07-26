// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/2d/circle_line_intersection_count.hpp"

#include <iostream>

int main(){
    int queries;
    std::cin >> queries;
    while(queries-- > 0){
        Circle circle;
        Line line;
        std::cin >> circle.center.x >> circle.center.y >> circle.radius
                 >> line.a.x >> line.a.y >> line.b.x >> line.b.y;
        std::cout << circle_line_intersection_count(circle, line) << '\n';
    }
}
