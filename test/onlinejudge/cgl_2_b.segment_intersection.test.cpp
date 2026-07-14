// competitive-verifier: PROBLEM https://onlinejudge.u-aizu.ac.jp/problems/CGL_2_B

#include <iostream>

#include "../../src/algorithm/geometry/2d/geometry.hpp"

int main(){
    int query_count;
    std::cin >> query_count;
    while(query_count--){
        Segment a, b;
        std::cin >> a.a.x >> a.a.y >> a.b.x >> a.b.y;
        std::cin >> b.a.x >> b.a.y >> b.b.x >> b.b.y;
        std::cout << intersect(a, b) << '\n';
    }
}
