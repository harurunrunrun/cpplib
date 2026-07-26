// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/2d/segment_intersection_count.hpp"

#include <iostream>
#include <vector>

int main(){
    int queries;
    std::cin >> queries;
    while(queries-- > 0){
        int count;
        std::cin >> count;
        std::vector<Segment> segments(static_cast<std::size_t>(count));
        for(Segment& segment: segments){
            std::cin >> segment.a.x >> segment.a.y
                     >> segment.b.x >> segment.b.y;
        }
        std::cout << segment_intersection_count(segments) << '\n';
    }
}
