// competitive-verifier: PROBLEM https://atcoder.jp/contests/abc075/tasks/abc075_c

#include <iostream>
#include <utility>
#include <vector>

#include "../../src/algorithm/graph/connectivity/lowlink.hpp"

int main(){
    int n, m;
    std::cin >> n >> m;
    std::vector<std::pair<int, int>> edges(static_cast<std::size_t>(m));
    for(auto& [u, v]: edges){
        std::cin >> u >> v;
        --u;
        --v;
    }
    std::cout << lowlink(n, edges).bridges.size() << '\n';
}
