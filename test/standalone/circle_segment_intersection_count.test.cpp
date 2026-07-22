// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/2d/scalar/circle_segment_intersection_count.hpp"

#include <iostream>

int main(){
    int queries;
    std::cin >> queries;
    while(queries-- > 0){
        Circle circle;
        Segment segment;
        std::cin >> circle.center.x >> circle.center.y >> circle.radius
                 >> segment.a.x >> segment.a.y >> segment.b.x >> segment.b.y;
        std::cout << circle_segment_intersection_count(circle, segment) << '\n';
    }
}
