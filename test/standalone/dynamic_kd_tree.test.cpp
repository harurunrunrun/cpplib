// competitive-verifier: STANDALONE
#include <algorithm>
#include <array>
#include <cassert>
#include <iostream>
#include <vector>
#include "../../src/structure/spatial/dynamic_kd_tree.hpp"
int main() {
    DynamicKdTree<int, 2> tree;
    assert(tree.insert(0, {1, 2}) && tree.insert(1, {3, 4}) && tree.insert(2, {2, 3}));
    auto ids = tree.range_search({1, 2}, {2, 3});
    std::sort(ids.begin(), ids.end());
    assert((ids == std::vector<int>{0, 2}));
    assert(tree.nearest({4, 4})->id == 1);
    assert(tree.erase(1) && tree.nearest({4, 4})->id == 2);
    std::cout << "OK\n";
}
