// competitive-verifier: STANDALONE

#include <iostream>

#include "../../src/algorithm/geometry/2d/circle_line_cross_points.hpp"
#include "geometry_2d_test_util.hpp"

int main(){
    int query_count;
    std::cin >> query_count;
    while(query_count--){
        Circle circle;
        Line line;
        std::cin >> circle.center.x >> circle.center.y >> circle.radius
                 >> line.a.x >> line.a.y >> line.b.x >> line.b.y;
        print_geometry_points(circle_line_cross_points(circle, line));
    }
}
