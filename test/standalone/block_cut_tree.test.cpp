// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include "../../src/algorithm/graph/block_cut_tree.hpp"

void validate_forest(const BlockCutTreeResult& result){
    const int nodes = result.node_count();
    assert(!result.is_block_node(-1));
    assert(!result.is_block_node(result.node_count()));
    for(int node = 0; node < nodes; ++node){
        const bool original = node < result.original_vertex_count;
        const bool block =
            result.original_vertex_count <= node && node < result.node_count();
        assert(result.is_original_vertex(node) == original);
        assert(result.is_block_node(node) == block);
        assert(result.is_original_vertex(node) != result.is_block_node(node));
        if(block){
            assert(result.block_node(result.block_id(node)) == node);
        }
    }
    std::vector<char> visited(static_cast<std::size_t>(nodes), false);
    int component_count = 0;
    int edge_twice = 0;
    for(int start = 0; start < nodes; ++start){
        edge_twice += static_cast<int>(result.tree[static_cast<std::size_t>(start)].size());
        if(visited[static_cast<std::size_t>(start)]) continue;
        ++component_count;
        std::vector<std::pair<int, int>> stack = {{start, -1}};
        visited[static_cast<std::size_t>(start)] = true;
        while(!stack.empty()){
            auto [vertex, parent] = stack.back();
            stack.pop_back();
            for(int to: result.tree[static_cast<std::size_t>(vertex)]){
                assert(result.is_original_vertex(vertex) != result.is_original_vertex(to));
                if(to == parent) continue;
                assert(!visited[static_cast<std::size_t>(to)]);
                visited[static_cast<std::size_t>(to)] = true;
                stack.push_back({to, vertex});
            }
        }
    }
    assert(edge_twice % 2 == 0);
    assert(edge_twice / 2 == nodes - component_count);
}

bool share_block(const BlockCutTreeResult& result, int u, int v){
    const auto& first = result.vertex_blocks[static_cast<std::size_t>(u)];
    const auto& second = result.vertex_blocks[static_cast<std::size_t>(v)];
    std::size_t i = 0, j = 0;
    while(i < first.size() && j < second.size()){
        if(first[i] == second[j]) return true;
        if(first[i] < second[j]) ++i;
        else ++j;
    }
    return false;
}

void self_test(){
    const std::vector<std::pair<int, int>> edges = {
        {0, 1}, {1, 2}, {2, 0}, {1, 3}, {3, 4}, {1, 4}, {4, 5}
    };
    const auto result = block_cut_tree(7, edges);
    validate_forest(result);
    assert((result.articulation == std::vector<int>{1, 4}));
    assert(result.block_count == 4);
    assert(share_block(result, 0, 2));
    assert(share_block(result, 1, 4));
    assert(!share_block(result, 0, 4));
    assert(result.vertex_blocks[6].size() == 1);
    for(int block = 0; block < result.block_count; ++block){
        assert(result.block_id(result.block_node(block)) == block);
    }
}

int main(){
    int n, m, query_count;
    if(!(std::cin >> n >> m >> query_count)){
        self_test();
        return 0;
    }
    std::vector<std::pair<int, int>> edges(static_cast<std::size_t>(m));
    for(auto& [u, v]: edges) std::cin >> u >> v;
    const auto result = block_cut_tree(n, edges);
    validate_forest(result);
    std::vector<char> articulation(static_cast<std::size_t>(n), false);
    for(int vertex: result.articulation){
        articulation[static_cast<std::size_t>(vertex)] = true;
    }
    while(query_count--){
        std::string type;
        int u, v;
        std::cin >> type >> u;
        if(type == "ART"){
            std::cout << static_cast<int>(articulation[static_cast<std::size_t>(u)]) << '\n';
        }else{
            std::cin >> v;
            std::cout << static_cast<int>(share_block(result, u, v)) << '\n';
        }
    }
}
