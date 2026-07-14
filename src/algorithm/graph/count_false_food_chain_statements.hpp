#pragma once

#include <numeric>
#include <stdexcept>
#include <vector>

struct FoodChainStatement{
    int relation;
    int left;
    int right;
};

inline int count_false_food_chain_statements(
    int animal_count,
    const std::vector<FoodChainStatement>& statements
){
    if(animal_count < 0)[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: range violation "
            "(count_false_food_chain_statements)."
        );
    }
    std::vector<int> parent(static_cast<std::size_t>(animal_count));
    std::vector<int> size(static_cast<std::size_t>(animal_count), 1);
    std::vector<int> difference_to_parent(static_cast<std::size_t>(animal_count), 0);
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
            const int edge_difference = difference_to_parent[static_cast<std::size_t>(vertex)];
            parent[static_cast<std::size_t>(vertex)] = root;
            difference_to_parent[static_cast<std::size_t>(vertex)] =
                (difference - prefix + 3) % 3;
            prefix = (prefix + edge_difference) % 3;
            vertex = next;
        }
        return root;
    };

    int false_count = 0;
    for(const auto& statement: statements){
        if(statement.relation != 1 && statement.relation != 2)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: invalid relation "
                "(count_false_food_chain_statements)."
            );
        }
        if(statement.left < 0 || animal_count <= statement.left ||
           statement.right < 0 || animal_count <= statement.right ||
           (statement.relation == 2 && statement.left == statement.right)){
            false_count++;
            continue;
        }
        const int left_root = leader(statement.left);
        const int right_root = leader(statement.right);
        const int required_difference = statement.relation == 1 ? 0 : 2;
        const int left_difference = difference_to_parent[static_cast<std::size_t>(statement.left)];
        const int right_difference = difference_to_parent[static_cast<std::size_t>(statement.right)];
        if(left_root == right_root){
            if((left_difference - right_difference + 3) % 3 != required_difference){
                false_count++;
            }
            continue;
        }
        int root_difference =
            (required_difference - left_difference + right_difference + 6) % 3;
        if(size[static_cast<std::size_t>(left_root)] <=
           size[static_cast<std::size_t>(right_root)]){
            parent[static_cast<std::size_t>(left_root)] = right_root;
            difference_to_parent[static_cast<std::size_t>(left_root)] = root_difference;
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
    return false_count;
}
