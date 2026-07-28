// competitive-verifier: STANDALONE

#include <cstddef>
#include <iostream>
#include <optional>
#include <queue>
#include <utility>
#include <vector>

#include "../../src/algorithm/graph/flow_cut/gomory_hu_tree.hpp"
#include "../../src/algorithm/graph/flow_cut/minimum_vertex_cut.hpp"
#include "../../src/algorithm/graph/flow_cut/stoer_wagner_minimum_cut.hpp"

namespace{

bool disconnected_after_removal(
    int vertex_count,
    const std::vector<std::pair<int, int>>& edges,
    const std::vector<int>& removed_vertices,
    int source,
    int sink,
    bool undirected
){
    std::vector<unsigned char> removed(
        static_cast<std::size_t>(vertex_count), 0
    );
    for(int vertex: removed_vertices){
        removed[static_cast<std::size_t>(vertex)] = 1;
    }
    std::vector<std::vector<int>> graph(
        static_cast<std::size_t>(vertex_count)
    );
    for(auto [from, to]: edges){
        graph[static_cast<std::size_t>(from)].push_back(to);
        if(undirected && from != to){
            graph[static_cast<std::size_t>(to)].push_back(from);
        }
    }
    std::queue<int> queue;
    std::vector<unsigned char> seen(
        static_cast<std::size_t>(vertex_count), 0
    );
    seen[static_cast<std::size_t>(source)] = 1;
    queue.push(source);
    while(!queue.empty()){
        int vertex = queue.front();
        queue.pop();
        for(int to: graph[static_cast<std::size_t>(vertex)]){
            if(removed[static_cast<std::size_t>(to)] != 0
                || seen[static_cast<std::size_t>(to)] != 0){
                continue;
            }
            seen[static_cast<std::size_t>(to)] = 1;
            queue.push(to);
        }
    }
    return seen[static_cast<std::size_t>(sink)] == 0;
}

} // namespace

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int case_count;
    if(!(std::cin >> case_count)) return 1;
    while(case_count-- > 0){
        int kind;
        int vertex_count;
        int edge_count;
        std::cin >> kind >> vertex_count >> edge_count;
        if(kind <= 1){
            std::vector<StoerWagnerEdge> stoer_edges;
            std::vector<GomoryHuEdge> gomory_edges;
            stoer_edges.reserve(static_cast<std::size_t>(edge_count));
            gomory_edges.reserve(static_cast<std::size_t>(edge_count));
            for(int index = 0; index < edge_count; ++index){
                int from;
                int to;
                long long capacity;
                std::cin >> from >> to >> capacity;
                stoer_edges.push_back({from, to, capacity});
                gomory_edges.push_back({from, to, capacity});
            }
            if(kind == 0){
                StoerWagnerMinimumCutResult result =
                    stoer_wagner_minimum_cut(vertex_count, stoer_edges);
                std::vector<unsigned char> side(
                    static_cast<std::size_t>(vertex_count), 0
                );
                for(int vertex: result.side){
                    side[static_cast<std::size_t>(vertex)] = 1;
                }
                long long capacity = 0;
                for(const auto& edge: stoer_edges){
                    if(side[static_cast<std::size_t>(edge.from)]
                        != side[static_cast<std::size_t>(edge.to)]){
                        capacity += edge.capacity;
                    }
                }
                if(vertex_count > 1 && capacity != result.capacity) return 2;
                std::cout << result.capacity << '\n';
            }else{
                GomoryHuTree tree = gomory_hu_tree(
                    vertex_count, gomory_edges
                );
                if(tree.size() != vertex_count
                    || tree.edges().size()
                        != static_cast<std::size_t>(
                            vertex_count == 0 ? 0 : vertex_count - 1
                        )){
                    return 3;
                }
                bool first = true;
                for(int left = 0; left < vertex_count; ++left){
                    for(int right = left + 1; right < vertex_count; ++right){
                        if(!first) std::cout << ' ';
                        first = false;
                        std::cout << tree.min_cut_value(left, right);
                    }
                }
                std::cout << '\n';
            }
            continue;
        }
        int source;
        int sink;
        std::cin >> source >> sink;
        std::vector<long long> capacity(
            static_cast<std::size_t>(vertex_count)
        );
        for(long long& value: capacity) std::cin >> value;
        std::vector<std::pair<int, int>> edges(
            static_cast<std::size_t>(edge_count)
        );
        for(auto& [from, to]: edges) std::cin >> from >> to;
        std::optional<MinimumVertexCutResult> result = kind == 2
            ? directed_minimum_vertex_cut(capacity, edges, source, sink)
            : undirected_minimum_vertex_cut(capacity, edges, source, sink);
        if(result){
            long long sum = 0;
            for(int vertex: result->vertices){
                sum += capacity[static_cast<std::size_t>(vertex)];
            }
            if(sum != result->capacity
                || !disconnected_after_removal(
                    vertex_count,
                    edges,
                    result->vertices,
                    source,
                    sink,
                    kind == 3
                )){
                return 4;
            }
            std::cout << result->capacity << '\n';
        }else{
            std::cout << -1 << '\n';
        }
    }
}
