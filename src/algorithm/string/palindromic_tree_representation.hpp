#ifndef CPPLIB_SRC_ALGORITHM_STRING_PALINDROMIC_TREE_REPRESENTATION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_STRING_PALINDROMIC_TREE_REPRESENTATION_HPP_INCLUDED

#include <cstddef>
#include <limits>
#include <memory>
#include <stdexcept>
#include <string_view>
#include <vector>

#include "palindromic_tree.hpp"

struct PalindromicTreeRelation{
    int parent;
    int suffix_link;

    friend constexpr bool operator==(
        const PalindromicTreeRelation&,
        const PalindromicTreeRelation&
    ) = default;
};

struct PalindromicTreeRepresentation{
    std::vector<PalindromicTreeRelation> nodes;
    std::vector<int> longest_suffix;
};

template<int MAX_LENGTH, int ALPHABET = 26, char OFFSET = 'a'>
PalindromicTreeRepresentation palindromic_tree_representation(
    std::string_view text
){
    static_assert(MAX_LENGTH >= 0);
    static_assert(MAX_LENGTH <= std::numeric_limits<int>::max() - 2);
    if(text.size() > static_cast<std::size_t>(MAX_LENGTH))[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: capacity exceeded "
            "(palindromic_tree_representation)."
        );
    }

    auto tree = std::make_unique<
        PalindromicTree<ALPHABET, MAX_LENGTH + 2, OFFSET>
    >();
    PalindromicTreeRepresentation result;
    result.longest_suffix.reserve(text.size());
    for(char character: text){
        result.longest_suffix.push_back(tree->add(character) - 1);
    }

    result.nodes.reserve(
        static_cast<std::size_t>(tree->distinct_palindromes())
    );
    const auto external_index = [](int node){
        return node == 0 ? -1 : node - 1;
    };
    for(int node = 2; node < tree->node_count(); ++node){
        result.nodes.push_back({
            external_index(tree->parent(node)),
            external_index(tree->link(node))
        });
    }
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_STRING_PALINDROMIC_TREE_REPRESENTATION_HPP_INCLUDED
