#pragma once

#include <optional>
#include <vector>

#include "link_cut_tree.hpp"
#include "../types/common_monoids.hpp"

template<int MAX_SIZE>
struct DynamicForestVertexSetPathSum{
private:
    LinkCutTree<AddMonoid<long long>{}, MAX_SIZE> forest;

public:
    explicit DynamicForestVertexSetPathSum(
        const std::vector<long long>& values
    ): forest(values){}

    int size() const{ return forest.size(); }

    bool connected(int left, int right){
        return forest.connected(left, right);
    }

    bool link(int left, int right){
        return forest.link(left, right);
    }

    bool cut(int left, int right){
        return forest.cut(left, right);
    }

    void set(int vertex, long long value){
        forest.set(vertex, value);
    }

    long long get(int vertex){
        return forest.get(vertex);
    }

    std::optional<long long> path_sum(int left, int right){
        if(!forest.connected(left, right)) return std::nullopt;
        return forest.path_prod(left, right);
    }
};
