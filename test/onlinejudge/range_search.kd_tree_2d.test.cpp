// competitive-verifier: PROBLEM https://onlinejudge.u-aizu.ac.jp/problems/DSL_2_C

#include <algorithm>
#include <iostream>
#include <vector>

#include "../../src/structure/other/kd_tree_2d.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int n;
    std::cin >> n;
    std::vector<KdTreePoint2D<int>> points(static_cast<std::size_t>(n));
    for(int id = 0; id < n; id++){
        std::cin >> points[static_cast<std::size_t>(id)].x
                 >> points[static_cast<std::size_t>(id)].y;
        points[static_cast<std::size_t>(id)].id = id;
    }
    KdTree2D<int, 500000> tree(points);
    int q;
    std::cin >> q;
    while(q--){
        int min_x, max_x, min_y, max_y;
        std::cin >> min_x >> max_x >> min_y >> max_y;
        auto ids = tree.range_search(min_x, max_x, min_y, max_y);
        std::sort(ids.begin(), ids.end());
        for(int id: ids) std::cout << id << '\n';
        std::cout << '\n';
    }
}
