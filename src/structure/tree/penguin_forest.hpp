#pragma once

#include <optional>
#include <vector>

#include "link_cut_tree.hpp"
#include "../types/common_monoids.hpp"

template<int MAX_SIZE>
struct PenguinForest{
private:
    LinkCutTree<AddMonoid<long long>{}, MAX_SIZE> forest;

public:
    explicit PenguinForest(const std::vector<long long>& penguins): forest(penguins){}

    int size() const{ return forest.size(); }

    bool bridge(int left, int right){ return forest.link(left, right); }

    void set_penguins(int vertex, long long count){ forest.set(vertex, count); }

    std::optional<long long> excursion_sum(int left, int right){
        if(!forest.connected(left, right)) return std::nullopt;
        return forest.path_prod(left, right);
    }
};
