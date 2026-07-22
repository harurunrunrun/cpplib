#ifndef CPPLIB_SRC_ALGORITHM_TREE_ROOTED_TREE_ISOMORPHISM_CLASSIFICATION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_TREE_ROOTED_TREE_ISOMORPHISM_CLASSIFICATION_HPP_INCLUDED

#include <algorithm>
#include <stdexcept>
#include <utility>
#include <vector>

#include "tree_isomorphism.hpp"

struct RootedTreeIsomorphismClassification{
    int class_count = 0;
    std::vector<int> class_id;
};

inline RootedTreeIsomorphismClassification
rooted_tree_isomorphism_classification(
    const std::vector<std::vector<int>>& graph,
    int root = 0
){
    if(graph.empty()){
        if(root != 0)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation "
                "(rooted_tree_isomorphism_classification)."
            );
        }
        return {};
    }
    std::vector<int> labels = rooted_tree_isomorphism_labels(graph, root);
    const int class_count = *std::max_element(labels.begin(), labels.end());
    for(int& label: labels) --label;
    return {class_count, std::move(labels)};
}

#endif  // CPPLIB_SRC_ALGORITHM_TREE_ROOTED_TREE_ISOMORPHISM_CLASSIFICATION_HPP_INCLUDED
