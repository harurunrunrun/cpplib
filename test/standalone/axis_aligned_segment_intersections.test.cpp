// competitive-verifier: STANDALONE

#include <iostream>
#include <vector>

#include "../../src/algorithm/geometry/2d/axis_aligned_segment_intersections.hpp"

int main(){
    int n;
    if(!(std::cin >> n)) return 0;
    std::vector<Segment> segments(static_cast<std::size_t>(n));
    for(Segment& segment: segments){
        std::cin >> segment.a.x >> segment.a.y >> segment.b.x >> segment.b.y;
    }
    std::cout << count_axis_aligned_segment_intersections(segments) << '\n';
}
