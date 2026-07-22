// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/2d/shape/bentley_ottmann_intersections.hpp"

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
        const auto intersections = bentley_ottmann_intersections(segments);
        bool valid = true;
        for(const SegmentIntersectionRecord& record: intersections){
            valid = valid && record.first < record.second
                && record.second < segments.size();
            if(!valid) continue;
            const auto geometry =
                advanced_geometry_detail::segment_intersection_set(
                    segments[record.first], segments[record.second]
                );
            if(record.kind == SEGMENT_INTERSECTION_POINT){
                valid = valid && geometry.size() == 1
                    && advanced_geometry_detail::point_equal(
                        record.first_point, geometry[0]
                    )
                    && advanced_geometry_detail::point_equal(
                        record.second_point, geometry[0]
                    );
            }else if(record.kind == SEGMENT_INTERSECTION_SEGMENT){
                valid = valid && geometry.size() == 2
                    && advanced_geometry_detail::point_equal(
                        record.first_point, geometry[0]
                    )
                    && advanced_geometry_detail::point_equal(
                        record.second_point, geometry[1]
                    );
            }else valid = false;
        }
        if(!valid){
            std::cout << "INVALID\n";
            continue;
        }
        std::cout << intersections.size() << '\n';
        for(const SegmentIntersectionRecord& record: intersections){
            std::cout << record.first << ' ' << record.second << ' '
                      << (record.kind == SEGMENT_INTERSECTION_POINT ? 'P' : 'S')
                      << '\n';
        }
    }
}
