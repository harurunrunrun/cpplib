// competitive-verifier: STANDALONE

#include <iostream>

#include "../../src/algorithm/geometry/2d/tangent_points.hpp"
#include "geometry_2d_test_util.hpp"

int main(){
    int query_count;
    std::cin >> query_count;
    while(query_count--){
        Circle circle;
        Point point;
        std::cin >> circle.center.x >> circle.center.y >> circle.radius
                 >> point.x >> point.y;
        print_geometry_points(tangent_points(circle, point));
    }
}
