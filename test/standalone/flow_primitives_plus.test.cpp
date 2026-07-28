// competitive-verifier: STANDALONE

#include <cstddef>
#include <iostream>
#include <vector>

#include "../../src/algorithm/matching/flow/capacity_scaling_max_flow.hpp"
#include "../../src/algorithm/matching/flow/circulation_with_lower_bounds.hpp"
#include "../../src/algorithm/matching/flow/flow_decomposition.hpp"

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
        if(kind == 0){
            int source;
            int sink;
            std::cin >> source >> sink;
            CapacityScalingMaxFlow<long long> flow(vertex_count);
            for(int index = 0; index < edge_count; ++index){
                int from;
                int to;
                long long capacity;
                std::cin >> from >> to >> capacity;
                flow.add_edge(from, to, capacity);
            }
            long long value = flow.max_flow(source, sink);
            auto side = flow.min_cut(source);
            long long cut = 0;
            for(std::size_t index = 0; index < flow.edges.size(); index += 2){
                const auto& edge = flow.edges[index];
                if(side[static_cast<std::size_t>(edge.from)] != 0
                    && side[static_cast<std::size_t>(edge.to)] == 0){
                    cut += edge.capacity;
                }
            }
            if(cut != value) return 2;
            std::cout << value << '\n';
        }else if(kind == 1){
            std::vector<LowerBoundCirculationEdge> edges(
                static_cast<std::size_t>(edge_count)
            );
            for(auto& edge: edges){
                std::cin >> edge.from >> edge.to
                         >> edge.lower >> edge.upper;
            }
            auto result = circulation_with_lower_bounds(vertex_count, edges);
            if(result){
                std::vector<long long> balance(
                    static_cast<std::size_t>(vertex_count), 0
                );
                for(std::size_t index = 0; index < edges.size(); ++index){
                    long long value = result->flow[index];
                    if(value < edges[index].lower
                        || edges[index].upper < value) return 3;
                    balance[static_cast<std::size_t>(edges[index].from)]
                        += value;
                    balance[static_cast<std::size_t>(edges[index].to)]
                        -= value;
                }
                for(long long value: balance) if(value != 0) return 4;
            }
            std::cout << static_cast<int>(result.has_value()) << '\n';
        }else{
            int source;
            int sink;
            std::cin >> source >> sink;
            std::vector<FlowDecompositionEdge> edges(
                static_cast<std::size_t>(edge_count)
            );
            long long input_sum = 0;
            long long path_total = 0;
            for(auto& edge: edges){
                std::cin >> edge.from >> edge.to >> edge.flow;
                input_sum += edge.flow;
                if(edge.from == source) path_total += edge.flow;
                if(edge.to == source) path_total -= edge.flow;
            }
            auto components =
                flow_decomposition(vertex_count, edges, source, sink);
            std::vector<long long> reconstructed(edges.size(), 0);
            long long reconstructed_path_total = 0;
            for(const auto& component: components){
                if(!component.is_cycle){
                    reconstructed_path_total += component.amount;
                }
                for(int edge_id: component.edge_ids){
                    reconstructed[static_cast<std::size_t>(edge_id)]
                        += component.amount;
                }
            }
            for(std::size_t index = 0; index < edges.size(); ++index){
                if(reconstructed[index] != edges[index].flow) return 5;
            }
            if(reconstructed_path_total != path_total) return 6;
            std::cout << path_total << ' ' << input_sum << '\n';
        }
    }
}
