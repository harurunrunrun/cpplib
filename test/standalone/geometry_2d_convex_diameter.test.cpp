// competitive-verifier: STANDALONE

#include <iostream>
#include <vector>

#include "../../src/algorithm/geometry/2d/convex_diameter.hpp"
#include "geometry_2d_test_util.hpp"

int main(){
    int query_count;
    std::cin >> query_count;
    while(query_count--){
        int size;
        std::cin >> size;
        std::vector<Point> polygon(static_cast<std::size_t>(size));
        for(Point& point: polygon) std::cin >> point.x >> point.y;
        print_geometry_scalar(convex_diameter(polygon));
        std::cout << '\n';
    }
}
