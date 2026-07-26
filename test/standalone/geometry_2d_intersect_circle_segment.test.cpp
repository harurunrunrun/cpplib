// competitive-verifier: STANDALONE

#include <iostream>

#include "../../src/algorithm/geometry/2d/intersect_circle_segment.hpp"

int main(){
    int query_count;
    std::cin >> query_count;
    while(query_count--){
        Circle circle;
        Segment segment;
        std::cin >> circle.center.x >> circle.center.y >> circle.radius
                 >> segment.a.x >> segment.a.y >> segment.b.x >> segment.b.y;
        std::cout << static_cast<int>(intersect_circle_segment(circle, segment))
                  << '\n';
    }
}
