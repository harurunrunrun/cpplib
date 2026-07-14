// competitive-verifier: PROBLEM https://onlinejudge.u-aizu.ac.jp/problems/CGL_2_A

#include <iostream>

#include "../../src/algorithm/geometry/2d/geometry.hpp"

int main(){
    int query_count;
    std::cin >> query_count;
    while(query_count--){
        Line a, b;
        std::cin >> a.a.x >> a.a.y >> a.b.x >> a.b.y;
        std::cin >> b.a.x >> b.a.y >> b.b.x >> b.b.y;
        if(parallel(a, b)) std::cout << "2\n";
        else if(orthogonal(a, b)) std::cout << "1\n";
        else std::cout << "0\n";
    }
}
