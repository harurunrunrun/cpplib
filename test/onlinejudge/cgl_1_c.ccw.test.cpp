// competitive-verifier: PROBLEM https://onlinejudge.u-aizu.ac.jp/problems/CGL_1_C

#include <iostream>

#include "../../src/algorithm/geometry/2d/geometry.hpp"

int main(){
    Point a, b;
    std::cin >> a.x >> a.y >> b.x >> b.y;
    int query_count;
    std::cin >> query_count;
    while(query_count--){
        Point c;
        std::cin >> c.x >> c.y;
        const int result = ccw(a, b, c);
        if(result == COUNTER_CLOCKWISE) std::cout << "COUNTER_CLOCKWISE\n";
        else if(result == CLOCKWISE) std::cout << "CLOCKWISE\n";
        else if(result == ONLINE_BACK) std::cout << "ONLINE_BACK\n";
        else if(result == ONLINE_FRONT) std::cout << "ONLINE_FRONT\n";
        else std::cout << "ON_SEGMENT\n";
    }
}
