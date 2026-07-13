// competitive-verifier: STANDALONE

#include <cmath>
#include <iostream>
#include <vector>

#include "../../src/algorithm/geometry/2d/closest_pair.hpp"
#include "geometry_2d_test_util.hpp"

int main(){
    int query_count;
    std::cin >> query_count;
    while(query_count--){
        int size;
        std::cin >> size;
        std::vector<Point> points(static_cast<std::size_t>(size));
        for(Point& point: points) std::cin >> point.x >> point.y;
        const long double answer = closest_pair(std::move(points));
        if(std::isinf(answer)){
            std::cout << "INF\n";
        }else{
            print_geometry_scalar(answer);
            std::cout << '\n';
        }
    }
}
