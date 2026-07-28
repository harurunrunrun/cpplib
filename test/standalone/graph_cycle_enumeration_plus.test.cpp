// competitive-verifier: STANDALONE

#include <algorithm>
#include <cstddef>
#include <iostream>
#include <set>
#include <utility>
#include <vector>

#include "../../src/algorithm/graph/connectivity/decomposition/cactus_decomposition.hpp"
#include "../../src/algorithm/graph/constraints/selection/maximal_clique_enumeration.hpp"
#include "../../src/algorithm/graph/specialized/route_optimization/minimum_mean_cycle.hpp"
#include "../../src/algorithm/graph/traversal/elementary_cycle_enumeration.hpp"

namespace{

void print_sets(const std::vector<std::vector<int>>& sets){
    std::cout << sets.size();
    for(const auto& values: sets){
        std::cout << ' ' << values.size();
        for(int value: values) std::cout << ' ' << value;
    }
    std::cout << '\n';
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
        if(kind == 0){
            std::vector<MinimumMeanCycleEdge> edges(
                static_cast<std::size_t>(edge_count)
            );
            for(auto& edge: edges){
                std::cin >> edge.from >> edge.to >> edge.weight;
            }
            auto result = minimum_mean_cycle(vertex_count, edges);
            if(result){
                std::cout << result->numerator << ' '
                          << result->denominator << '\n';
            }else{
                std::cout << "none\n";
            }
            continue;
        }
        std::vector<std::pair<int, int>> edges(
            static_cast<std::size_t>(edge_count)
        );
        for(auto& [from, to]: edges) std::cin >> from >> to;
        if(kind == 1){
            auto cliques = enumerate_maximal_cliques(vertex_count, edges);
            std::set<std::pair<int, int>> edge_set;
            for(auto [left, right]: edges){
                edge_set.emplace(std::min(left, right), std::max(left, right));
            }
            for(const auto& clique: cliques){
                std::vector<unsigned char> used(
                    static_cast<std::size_t>(vertex_count), 0
                );
                for(int vertex: clique){
                    if(used[static_cast<std::size_t>(vertex)] != 0) return 2;
                    used[static_cast<std::size_t>(vertex)] = 1;
                }
                for(int left: clique){
                    for(int right: clique){
                        if(left < right
                            && !edge_set.contains({left, right})) return 3;
                    }
                }
                for(int outside = 0; outside < vertex_count; ++outside){
                    if(used[static_cast<std::size_t>(outside)] != 0) continue;
                    bool can_add = true;
                    for(int inside: clique){
                        if(!edge_set.contains({
                            std::min(outside, inside),
                            std::max(outside, inside)
                        })){
                            can_add = false;
                        }
                    }
                    if(can_add) return 4;
                }
            }
            print_sets(cliques);
        }else if(kind == 2){
            std::vector<std::vector<int>> graph(
                static_cast<std::size_t>(vertex_count)
            );
            std::set<std::pair<int, int>> edge_set(edges.begin(), edges.end());
            for(auto [from, to]: edges){
                graph[static_cast<std::size_t>(from)].push_back(to);
            }
            auto cycles = enumerate_elementary_cycles(std::move(graph));
            for(const auto& cycle: cycles){
                if(cycle.empty()) return 5;
                std::vector<unsigned char> used(
                    static_cast<std::size_t>(vertex_count), 0
                );
                for(std::size_t index = 0; index < cycle.size(); ++index){
                    int vertex = cycle[index];
                    if(used[static_cast<std::size_t>(vertex)] != 0) return 6;
                    used[static_cast<std::size_t>(vertex)] = 1;
                    int to = cycle[(index + 1) % cycle.size()];
                    if(!edge_set.contains({vertex, to})) return 7;
                }
            }
            print_sets(cycles);
        }else{
            auto result = cactus_decomposition(vertex_count, edges);
            if(result){
                std::vector<int> count(
                    static_cast<std::size_t>(edge_count), 0
                );
                for(int edge_id: result->bridge_edge_ids){
                    ++count[static_cast<std::size_t>(edge_id)];
                }
                for(const auto& cycle_edges: result->cycle_edge_ids){
                    for(int edge_id: cycle_edges){
                        ++count[static_cast<std::size_t>(edge_id)];
                    }
                }
                for(int value: count) if(value != 1) return 8;
            }
            std::cout << static_cast<int>(result.has_value()) << '\n';
        }
    }
}
