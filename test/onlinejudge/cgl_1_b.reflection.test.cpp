#define PROBLEM "https://onlinejudge.u-aizu.ac.jp/problems/CGL_1_B"

#include <iomanip>
#include <iostream>

#include "../../src/algorithm/geometry/geometry.hpp"

int main(){
    Line line;
    std::cin >> line.a.x >> line.a.y >> line.b.x >> line.b.y;
    int query_count;
    std::cin >> query_count;
    std::cout << std::fixed << std::setprecision(12);
    while(query_count--){
        Point point;
        std::cin >> point.x >> point.y;
        const Point answer = reflection(line, point);
        std::cout << answer.x << ' ' << answer.y << '\n';
    }
}
