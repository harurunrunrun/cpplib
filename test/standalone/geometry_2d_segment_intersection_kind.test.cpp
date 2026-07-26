// competitive-verifier: STANDALONE

#include <concepts>
#include <iostream>

#include "../../src/algorithm/geometry/2d/segment_intersection_kind_result.hpp"
#include "../../src/algorithm/geometry/2d/segment_intersection_kind_segments.hpp"
#include "../../src/algorithm/geometry/2d/segment_intersection_kind.hpp"

int main(){
    static_assert(SEGMENT_INTERSECTION_NONE == 0);
    static_assert(SEGMENT_INTERSECTION_POINT == 1);
    static_assert(SEGMENT_INTERSECTION_SEGMENT == 2);
    static_assert(std::same_as<
        decltype(segment_intersection_kind(Segment{}, Segment{})),
        SegmentIntersectionKind
    >);

    int query_count;
    std::cin >> query_count;
    while(query_count--){
        Segment first;
        Segment second;
        std::cin >> first.a.x >> first.a.y >> first.b.x >> first.b.y;
        std::cin >> second.a.x >> second.a.y >> second.b.x >> second.b.y;
        std::cout << segment_intersection_kind(first, second) << '\n';
    }
}
