// competitive-verifier: PROBLEM https://www.spoj.com/problems/FASTFLOW/

#include <iostream>
#include <vector>

#include "../../src/algorithm/graph/undirected_max_flow.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int n, m;
    if(!(std::cin >> n >> m)) return 0;
    std::vector<UndirectedFlowEdge<long long>> edges;
    edges.reserve(static_cast<std::size_t>(m));
    for(int index = 0; index < m; index++){
        int from, to;
        long long capacity;
        std::cin >> from >> to >> capacity;
        edges.push_back({from - 1, to - 1, capacity});
    }
    std::cout << undirected_max_flow<long long>(n, edges, 0, n - 1) << '\n';
}
