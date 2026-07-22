// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/st_numbering

#include <iostream>
#include <utility>
#include <vector>

#include "../../src/algorithm/graph/connectivity/st_numbering.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int test_count;
    std::cin >> test_count;
    while(test_count--){
        int n, m, source, sink;
        std::cin >> n >> m >> source >> sink;
        std::vector<std::pair<int, int>> edges(static_cast<std::size_t>(m));
        for(auto& [left, right]: edges) std::cin >> left >> right;
        const auto result = st_numbering(n, edges, source, sink);
        if(!result){
            std::cout << "No\n";
            continue;
        }
        std::cout << "Yes\n";
        for(int vertex = 0; vertex < n; ++vertex){
            if(vertex != 0) std::cout << ' ';
            std::cout << result->number[static_cast<std::size_t>(vertex)];
        }
        std::cout << '\n';
    }
}
