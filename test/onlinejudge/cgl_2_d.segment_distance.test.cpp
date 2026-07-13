#define PROBLEM "https://onlinejudge.u-aizu.ac.jp/problems/CGL_2_D"

#include <iomanip>
#include <iostream>

#include "../../src/algorithm/geometry/2d/geometry.hpp"

int main(){
    int query_count;
    std::cin >> query_count;
    std::cout << std::fixed << std::setprecision(12);
    while(query_count--){
        Segment a, b;
        std::cin >> a.a.x >> a.a.y >> a.b.x >> a.b.y;
        std::cin >> b.a.x >> b.a.y >> b.b.x >> b.b.y;
        std::cout << distance(a, b) << '\n';
    }
}
