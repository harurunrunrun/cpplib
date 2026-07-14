// competitive-verifier: PROBLEM https://www.spoj.com/problems/BOTTOM/

#include <iostream>
#include <vector>

#include "../../src/algorithm/graph/bottom_vertices.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    for(int size; std::cin >> size && size != 0;){
        int edge_count;
        std::cin >> edge_count;
        std::vector<std::vector<int>> graph(static_cast<std::size_t>(size));
        while(edge_count-- != 0){
            int from, to;
            std::cin >> from >> to;
            graph[static_cast<std::size_t>(from - 1)].push_back(to - 1);
        }
        const auto answer = bottom_vertices(graph);
        for(std::size_t index = 0; index < answer.size(); ++index){
            if(index != 0) std::cout << ' ';
            std::cout << answer[index] + 1;
        }
        std::cout << '\n';
    }
}
