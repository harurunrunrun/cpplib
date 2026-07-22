// competitive-verifier: STANDALONE

#include <iostream>

#include "../../src/algorithm/geometry/2d/point_collection/circle_segment_cross_points.hpp"
#include "geometry_2d_test_util.hpp"

int main(){
    int query_count;
    std::cin >> query_count;
    while(query_count--){
        Circle circle;
        Segment segment;
        std::cin >> circle.center.x >> circle.center.y >> circle.radius
                 >> segment.a.x >> segment.a.y >> segment.b.x >> segment.b.y;
        print_geometry_points(circle_segment_cross_points(circle, segment));
    }
}
