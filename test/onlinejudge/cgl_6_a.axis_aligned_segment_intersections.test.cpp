// competitive-verifier: PROBLEM https://onlinejudge.u-aizu.ac.jp/problems/CGL_6_A

#include <cstddef>
#include <iostream>
#include <vector>

#include "../../src/algorithm/geometry/2d/scalar/axis_aligned_segment_intersections.hpp"

int main(){
    int count;
    std::cin >> count;
    std::vector<Segment> segments(static_cast<std::size_t>(count));
    for(Segment& segment: segments){
        std::cin >> segment.a.x >> segment.a.y
                 >> segment.b.x >> segment.b.y;
    }
    std::cout << count_axis_aligned_segment_intersections(segments) << '\n';
}
