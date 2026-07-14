// competitive-verifier: PROBLEM https://www.spoj.com/problems/DAGCNT/

#include <cstddef>
#include <iostream>
#include <string>
#include <vector>

#include "../../src/algorithm/graph/count_directed_spanning_arborescences.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int vertex_count;
    while(std::cin >> vertex_count && vertex_count != 0){
        std::vector<std::string> adjacency(
            static_cast<std::size_t>(vertex_count)
        );
        for(auto& row: adjacency) std::cin >> row;
        std::cout << count_directed_spanning_arborescences(adjacency) << '\n';
    }
}
