// competitive-verifier: STANDALONE

#include <cstddef>
#include <iostream>

#include "../../src/algorithm/matching/flow/mpm_max_flow.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int case_count;
    if(!(std::cin >> case_count)) return 1;
    while(case_count-- > 0){
        int vertex_count;
        int edge_count;
        int source;
        int sink;
        std::cin >> vertex_count >> edge_count >> source >> sink;
        MPMMaxFlow<long long> flow(vertex_count);
        for(int index = 0; index < edge_count; ++index){
            int from;
            int to;
            long long capacity;
            std::cin >> from >> to >> capacity;
            flow.add_edge(from, to, capacity);
        }
        const long long value = flow.max_flow(source, sink);
        const auto side = flow.min_cut(source);
        long long cut = 0;
        for(std::size_t index = 0; index < flow.edges.size(); index += 2){
            const auto& edge = flow.edges[index];
            if(side[static_cast<std::size_t>(edge.from)] != 0
                && side[static_cast<std::size_t>(edge.to)] == 0){
                cut += edge.capacity;
            }
            if(edge.flow < 0 || edge.capacity < edge.flow) return 2;
        }
        if(cut != value) return 3;
        std::cout << value << '\n';
    }
}
