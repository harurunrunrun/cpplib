#pragma once

#include <algorithm>
#include <memory>
#include <utility>
#include <vector>

#include "../../structure/segtree/dualsegtree.hpp"
#include "../../structure/types/common_monoids.hpp"

template<class Coordinate, int MAX_ATOMS>
int count_visible_overlaid_intervals(
    const std::vector<std::pair<Coordinate, Coordinate>>& closed_intervals
){
    constexpr AssignMaxMonoidAct<int> assignment{};
    std::vector<std::pair<Coordinate, Coordinate>> intervals = closed_intervals;
    std::vector<Coordinate> boundaries;
    boundaries.reserve(intervals.size() * 2);
    for(auto& [left, right]: intervals){
        if(right < left) std::swap(left, right);
        boundaries.push_back(left);
        boundaries.push_back(right);
    }
    std::sort(boundaries.begin(), boundaries.end());
    boundaries.erase(std::unique(boundaries.begin(), boundaries.end()), boundaries.end());
    if(boundaries.empty()) return 0;
    const std::size_t atom_count = boundaries.size() * 2 - 1;
    auto tree = std::make_unique<DualSegtree<assignment, MAX_ATOMS>>(
        std::vector<int>(atom_count, 0)
    );
    for(int index = 0; index < static_cast<int>(intervals.size()); ++index){
        const auto [left, right] = intervals[static_cast<std::size_t>(index)];
        const int first = static_cast<int>(
            std::lower_bound(boundaries.begin(), boundaries.end(), left)
            - boundaries.begin()
        ) * 2;
        const int last = static_cast<int>(
            std::upper_bound(boundaries.begin(), boundaries.end(), right)
            - boundaries.begin()
        ) * 2 - 1;
        tree->apply(first, last, {true, index + 1});
    }
    std::vector<bool> visible(intervals.size() + 1, false);
    for(int index = 0; index < static_cast<int>(atom_count); ++index){
        visible[static_cast<std::size_t>(tree->get(index))] = true;
    }
    return static_cast<int>(std::count(visible.begin() + 1, visible.end(), true));
}
