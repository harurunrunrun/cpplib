// competitive-verifier: STANDALONE

#include <iostream>

#include "../../src/algorithm/geometry/2d/circumcircle.hpp"
#include "geometry_2d_test_util.hpp"

int main(){
    int query_count;
    std::cin >> query_count;
    while(query_count--){
        Point first;
        Point second;
        Point third;
        std::cin >> first.x >> first.y >> second.x >> second.y
                 >> third.x >> third.y;
        const Circle circle = circumcircle(first, second, third);
        print_geometry_point(circle.center);
        std::cout << ' ';
        print_geometry_scalar(circle.radius);
        std::cout << '\n';
    }
}
