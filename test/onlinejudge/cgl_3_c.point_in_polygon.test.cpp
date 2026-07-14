// competitive-verifier: PROBLEM https://onlinejudge.u-aizu.ac.jp/problems/CGL_3_C

#include <iostream>
#include <vector>

#include "../../src/algorithm/geometry/2d/geometry.hpp"

int main(){
    int size;
    std::cin >> size;
    std::vector<Point> polygon(static_cast<std::size_t>(size));
    for(auto& point: polygon) std::cin >> point.x >> point.y;
    int query_count;
    std::cin >> query_count;
    while(query_count--){
        Point point;
        std::cin >> point.x >> point.y;
        std::cout << contains(polygon, point) << '\n';
    }
}
