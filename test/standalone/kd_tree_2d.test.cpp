// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <iostream>
#include <stdexcept>
#include <vector>

#include "../../src/structure/other/kd_tree_2d.hpp"

void self_test(){
    std::vector<KdTreePoint2D<int>> points{
        {1, 2, 0}, {3, 4, 1}, {2, 3, 2}, {2, 3, 3}, {-1, 5, 4}
    };
    KdTree2D<int, 16> tree;
    assert(tree.empty());
    assert(tree.size() == 0);
    tree.build(points);
    assert(tree.size() == 5);
    auto ids = tree.range_search(1, 2, 2, 3);
    std::sort(ids.begin(), ids.end());
    assert((ids == std::vector<int>{0, 2, 3}));
    assert(tree.range_search(10, 20, 10, 20).empty());
    tree.clear();
    assert(tree.empty());
    tree.build(points);
    assert(tree.size() == 5);

    KdTree2D<int, 16> built(points);
    assert(built.size() == 5);
    assert(built.range_search(3, 3, 4, 4) == std::vector<int>{1});

    bool thrown = false;
    try{
        (void)tree.range_search(2, 1, 0, 1);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);

    thrown = false;
    try{
        KdTree2D<int, 2> too_small(points);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);
}

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int n, q;
    if(!(std::cin >> n >> q)){
        self_test();
        return 0;
    }
    std::vector<KdTreePoint2D<int>> points(static_cast<std::size_t>(n));
    for(auto& point: points) std::cin >> point.x >> point.y >> point.id;
    KdTree2D<int, 4096> tree(points);
    while(q--){
        int min_x, max_x, min_y, max_y;
        std::cin >> min_x >> max_x >> min_y >> max_y;
        auto ids = tree.range_search(min_x, max_x, min_y, max_y);
        std::sort(ids.begin(), ids.end());
        std::cout << ids.size();
        for(int id: ids) std::cout << ' ' << id;
        std::cout << '\n';
    }
}
