#ifndef CPPLIB_SRC_ALGORITHM_GRAPH_COUNT_INCONSISTENT_MODULO_THREE_CONSTRAINTS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GRAPH_COUNT_INCONSISTENT_MODULO_THREE_CONSTRAINTS_HPP_INCLUDED

#include <numeric>
#include <stdexcept>
#include <vector>

struct ModularDifferenceConstraint{
    int difference;
    int left;
    int right;
};

inline int count_inconsistent_modulo_three_constraints(
    int variable_count,
    const std::vector<ModularDifferenceConstraint>& constraints
){
    if(variable_count < 0)[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: range violation "
            "(count_inconsistent_modulo_three_constraints)."
        );
    }
    std::vector<int> parent(static_cast<std::size_t>(variable_count));
    std::vector<int> size(static_cast<std::size_t>(variable_count), 1);
    std::vector<int> difference_to_parent(
        static_cast<std::size_t>(variable_count),
        0
    );
    std::iota(parent.begin(), parent.end(), 0);

    auto leader = [&](int start){
        int root = start;
        int difference = 0;
        while(parent[static_cast<std::size_t>(root)] != root){
            difference += difference_to_parent[static_cast<std::size_t>(root)];
            difference %= 3;
            root = parent[static_cast<std::size_t>(root)];
        }
        int prefix = 0;
        int vertex = start;
        while(parent[static_cast<std::size_t>(vertex)] != vertex){
            const int next = parent[static_cast<std::size_t>(vertex)];
            const int edge_difference =
                difference_to_parent[static_cast<std::size_t>(vertex)];
            parent[static_cast<std::size_t>(vertex)] = root;
            difference_to_parent[static_cast<std::size_t>(vertex)] =
                (difference - prefix + 3) % 3;
            prefix = (prefix + edge_difference) % 3;
            vertex = next;
        }
        return root;
    };

    int inconsistent_count = 0;
    for(const auto& constraint: constraints){
        if(constraint.left < 0 || variable_count <= constraint.left ||
           constraint.right < 0 || variable_count <= constraint.right){
            ++inconsistent_count;
            continue;
        }
        const int required_difference =
            (constraint.difference % 3 + 3) % 3;
        const int left_root = leader(constraint.left);
        const int right_root = leader(constraint.right);
        const int left_difference =
            difference_to_parent[static_cast<std::size_t>(constraint.left)];
        const int right_difference =
            difference_to_parent[static_cast<std::size_t>(constraint.right)];
        if(left_root == right_root){
            if((left_difference - right_difference + 3) % 3 !=
               required_difference){
                ++inconsistent_count;
            }
            continue;
        }
        const int root_difference =
            (required_difference - left_difference + right_difference + 6) % 3;
        if(size[static_cast<std::size_t>(left_root)] <=
           size[static_cast<std::size_t>(right_root)]){
            parent[static_cast<std::size_t>(left_root)] = right_root;
            difference_to_parent[static_cast<std::size_t>(left_root)] =
                root_difference;
            size[static_cast<std::size_t>(right_root)] +=
                size[static_cast<std::size_t>(left_root)];
        }else{
            parent[static_cast<std::size_t>(right_root)] = left_root;
            difference_to_parent[static_cast<std::size_t>(right_root)] =
                (3 - root_difference) % 3;
            size[static_cast<std::size_t>(left_root)] +=
                size[static_cast<std::size_t>(right_root)];
        }
    }
    return inconsistent_count;
}

#endif  // CPPLIB_SRC_ALGORITHM_GRAPH_COUNT_INCONSISTENT_MODULO_THREE_CONSTRAINTS_HPP_INCLUDED
