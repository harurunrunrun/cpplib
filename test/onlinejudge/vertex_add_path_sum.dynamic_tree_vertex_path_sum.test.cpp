// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/vertex_add_path_sum

#include <iostream>
#include <utility>
#include <vector>

#include "../../src/structure/tree/dynamic_tree_vertex_path_sum.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int vertex_count, query_count;
    std::cin >> vertex_count >> query_count;
    std::vector<long long> values(static_cast<std::size_t>(vertex_count));
    for(long long& value: values) std::cin >> value;
    std::vector<std::pair<int, int>> edges;
    edges.reserve(static_cast<std::size_t>(
        vertex_count > 0 ? vertex_count - 1 : 0
    ));
    for(int edge = 1; edge < vertex_count; ++edge){
        int first, second;
        std::cin >> first >> second;
        edges.push_back({first, second});
    }

    DynamicTreeVertexPathSum<long long> tree(values, edges);
    while(query_count-- != 0){
        int type, vertex;
        std::cin >> type >> vertex;
        if(type == 0){
            long long delta;
            std::cin >> delta;
            tree.add(vertex, delta);
        }else{
            int target;
            std::cin >> target;
            std::cout << tree.path_sum(vertex, target) << '\n';
        }
    }
}
