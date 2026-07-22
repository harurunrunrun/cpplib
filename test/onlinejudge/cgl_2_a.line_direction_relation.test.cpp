// competitive-verifier: PROBLEM https://onlinejudge.u-aizu.ac.jp/problems/CGL_2_A

#include <iostream>

#include "../../src/algorithm/geometry/2d/predicate/line_direction_relation.hpp"

int main(){
    int query_count;
    std::cin >> query_count;
    while(query_count--){
        Line first, second;
        std::cin >> first.a.x >> first.a.y >> first.b.x >> first.b.y;
        std::cin >> second.a.x >> second.a.y >> second.b.x >> second.b.y;
        const auto relation = line_direction_relation(first, second);
        std::cout << static_cast<int>(relation) << '\n';
    }
}
