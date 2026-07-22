// competitive-verifier: STANDALONE

#include <iostream>

#include "../../src/algorithm/geometry/2d/point/circumcenter.hpp"
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
        print_geometry_point(circumcenter(first, second, third));
        std::cout << '\n';
    }
}
