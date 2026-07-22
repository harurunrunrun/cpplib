// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/area_of_union_of_rectangles

#include <iostream>
#include <vector>
#include "../../src/algorithm/other/offline_query/rectangle_union.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int n;
    std::cin >> n;
    std::vector<RectangleUnionRectangle<long long>> rectangles(static_cast<std::size_t>(n));
    for(auto& rectangle: rectangles){
        std::cin >> rectangle.x1 >> rectangle.y1 >> rectangle.x2 >> rectangle.y2;
    }
    std::cout << rectangle_union_area(std::move(rectangles)) << '\n';
}
