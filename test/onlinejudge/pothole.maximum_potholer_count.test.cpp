// competitive-verifier: PROBLEM https://www.spoj.com/problems/POTHOLE/

#include <iostream>
#include <utility>
#include <vector>

#include "../../src/algorithm/graph/flow_cut/maximum_paths_with_distinct_terminal_edges.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int test_count;
    if(!(std::cin >> test_count)) return 0;
    while(test_count--){
        int vertex_count;
        std::cin >> vertex_count;
        std::vector<std::pair<int, int>> edges;
        for(int from = 0; from + 1 < vertex_count; ++from){
            int degree;
            std::cin >> degree;
            while(degree--){
                int to;
                std::cin >> to;
                edges.emplace_back(from, to - 1);
            }
        }
        std::cout << maximum_paths_with_distinct_terminal_edges(
            vertex_count, edges, 0, vertex_count - 1
        ) << '\n';
    }
}
