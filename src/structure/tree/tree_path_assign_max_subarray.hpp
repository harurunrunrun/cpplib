#ifndef CPPLIB_SRC_STRUCTURE_TREE_TREE_PATH_ASSIGN_MAX_SUBARRAY_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_TREE_TREE_PATH_ASSIGN_MAX_SUBARRAY_HPP_INCLUDED

#include <vector>

#include "lazy_link_cut_tree.hpp"
#include "../types/common_monoids.hpp"

template<class Value, int MAX_SIZE>
struct TreePathAssignMaxSubarray{
private:
    static constexpr AssignMaxSubarrayMonoidAct<Value> action{};
    LazyLinkCutTree<action, MAX_SIZE> tree;

public:
    explicit TreePathAssignMaxSubarray(const std::vector<Value>& values):
        tree([&]{
            std::vector<MaxSubarrayAggregate<Value>> aggregates;
            aggregates.reserve(values.size());
            for(const Value& value: values) aggregates.push_back(max_subarray_singleton(value));
            return aggregates;
        }()){}

    bool add_edge(int left, int right){ return tree.link(left, right); }

    Value path_max_subarray(int left, int right){
        return tree.path_prod(left, right).best;
    }

    void assign_path(int left, int right, Value value){
        tree.path_apply(left, right, MonoidAssignment<Value>{true, value});
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_TREE_TREE_PATH_ASSIGN_MAX_SUBARRAY_HPP_INCLUDED
