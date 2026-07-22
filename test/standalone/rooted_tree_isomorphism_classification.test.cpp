// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <vector>

#include "../../src/algorithm/tree/isomorphism/rooted_tree_isomorphism_classification.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int size;
    if(!(std::cin >> size)){
        const auto empty = rooted_tree_isomorphism_classification({});
        assert(empty.class_count == 0 && empty.class_id.empty());
        const std::vector<std::vector<int>> graph = {{1, 2}, {0}, {0}};
        const auto result = rooted_tree_isomorphism_classification(graph);
        assert(result.class_count == 2);
        assert(result.class_id[1] == result.class_id[2]);
        assert(result.class_id[0] != result.class_id[1]);
        return 0;
    }

    std::vector<std::vector<int>> graph(static_cast<std::size_t>(size));
    for(int vertex = 1; vertex < size; ++vertex){
        int parent;
        std::cin >> parent;
        graph[static_cast<std::size_t>(parent)].push_back(vertex);
        graph[static_cast<std::size_t>(vertex)].push_back(parent);
    }
    const auto result = rooted_tree_isomorphism_classification(graph);
    std::cout << result.class_count << '\n';
    for(int left = 0; left < size; ++left){
        for(int right = 0; right < size; ++right){
            std::cout << static_cast<int>(
                result.class_id[static_cast<std::size_t>(left)] ==
                result.class_id[static_cast<std::size_t>(right)]
            );
        }
        std::cout << '\n';
    }
}
