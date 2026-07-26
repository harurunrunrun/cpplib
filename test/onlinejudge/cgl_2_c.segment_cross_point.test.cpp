// competitive-verifier: PROBLEM https://onlinejudge.u-aizu.ac.jp/problems/CGL_2_C
// competitive-verifier: ERROR 1e-8

#include <iomanip>
#include <iostream>

#include "../../src/algorithm/geometry/2d/segment_intersection.hpp"

int main(){
    int query_count;
    std::cin >> query_count;
    std::cout << std::fixed << std::setprecision(10);
    while(query_count--){
        Segment a, b;
        std::cin >> a.a.x >> a.a.y >> a.b.x >> a.b.y;
        std::cin >> b.a.x >> b.a.y >> b.b.x >> b.b.y;
        const Point answer = segment_intersection(a, b);
        std::cout << answer.x << ' ' << answer.y << '\n';
    }
}
