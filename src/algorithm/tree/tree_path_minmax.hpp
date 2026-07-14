#pragma once

#include <algorithm>
#include <limits>
#include <utility>

#include "static_tree_path_fold.hpp"
#include "../../structure/types/monoid.hpp"

template<class Weight>
constexpr std::pair<Weight, Weight> tree_path_minmax_op(
    std::pair<Weight, Weight> left,
    std::pair<Weight, Weight> right
){
    return {std::min(left.first, right.first), std::max(left.second, right.second)};
}

template<class Weight>
constexpr std::pair<Weight, Weight> tree_path_minmax_identity(){
    return {std::numeric_limits<Weight>::max(), std::numeric_limits<Weight>::lowest()};
}

template<class Weight, int MAX_SIZE>
struct TreePathMinmax{
private:
    static constexpr auto monoid = Monoid<
        tree_path_minmax_op<Weight>,
        tree_path_minmax_identity<Weight>
    >{};
    StaticTreePathFold<monoid, MAX_SIZE> tree;

public:
    explicit TreePathMinmax(int n): tree(n){}
    void add_edge(int left, int right, Weight weight){ tree.add_edge(left, right, {weight, weight}); }
    void build(int root = 0){ tree.build(root); }
    std::pair<Weight, Weight> path_minmax(int left, int right) const{ return tree.prod(left, right); }
};
