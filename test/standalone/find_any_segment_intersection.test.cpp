// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/2d/find_any_segment_intersection.hpp"

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
        const auto answer = find_any_segment_intersection(segments);
        if(!answer.has_value()){
            std::cout << "0\n";
            continue;
        }
        const auto [first, second] = *answer;
        const bool valid = first < second && second < segments.size()
            && !advanced_geometry_detail::segment_intersection_set(
                segments[first], segments[second]
            ).empty();
        std::cout << (valid ? "1\n" : "INVALID\n");
    }
}
