// competitive-verifier: STANDALONE

#include <algorithm>
#include <cstddef>
#include <iostream>
#include <queue>
#include <set>
#include <utility>
#include <vector>

#include "../../src/algorithm/graph/connectivity/decomposition/core_decomposition.hpp"
#include "../../src/algorithm/graph/connectivity/decomposition/strong_orientation.hpp"
#include "../../src/algorithm/graph/traversal/degeneracy_ordering.hpp"
#include "../../src/algorithm/graph/traversal/transitive_reduction.hpp"
#include "../../src/algorithm/matching/bipartite/maximum_antichain.hpp"
#include "../../src/algorithm/matching/bipartite/minimum_path_cover_dag.hpp"

namespace{

bool is_strongly_connected(
    int vertex_count,
    const std::vector<std::pair<int, int>>& edges
){
    if(vertex_count == 0) return true;
    for(int reverse = 0; reverse < 2; ++reverse){
        std::vector<std::vector<int>> graph(
            static_cast<std::size_t>(vertex_count)
        );
        for(auto [from, to]: edges){
            if(reverse != 0) std::swap(from, to);
            graph[static_cast<std::size_t>(from)].push_back(to);
        }
        std::vector<unsigned char> seen(
            static_cast<std::size_t>(vertex_count), 0
        );
        std::queue<int> queue;
        seen[0] = 1;
        queue.push(0);
        while(!queue.empty()){
            int vertex = queue.front();
            queue.pop();
            for(int to: graph[static_cast<std::size_t>(vertex)]){
                if(seen[static_cast<std::size_t>(to)] != 0) continue;
                seen[static_cast<std::size_t>(to)] = 1;
                queue.push(to);
            }
        }
        for(unsigned char value: seen){
            if(value == 0) return false;
        }
    }
    return true;
}

std::vector<std::vector<unsigned char>> reachability(
    const std::vector<std::vector<int>>& graph
){
    const int n = static_cast<int>(graph.size());
    std::vector<std::vector<unsigned char>> result(
        static_cast<std::size_t>(n),
        std::vector<unsigned char>(static_cast<std::size_t>(n), 0)
    );
    for(int source = 0; source < n; ++source){
        std::vector<int> stack{source};
        while(!stack.empty()){
            int vertex = stack.back();
            stack.pop_back();
            for(int to: graph[static_cast<std::size_t>(vertex)]){
                if(result[static_cast<std::size_t>(source)]
                    [static_cast<std::size_t>(to)] != 0) continue;
                result[static_cast<std::size_t>(source)]
                    [static_cast<std::size_t>(to)] = 1;
                stack.push_back(to);
            }
        }
    }
    return result;
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
        std::vector<std::pair<int, int>> edges(
            static_cast<std::size_t>(edge_count)
        );
        for(auto& [from, to]: edges) std::cin >> from >> to;
        if(kind == 0){
            CoreDecompositionResult core =
                core_decomposition(vertex_count, edges);
            DegeneracyOrderingResult ordering =
                degeneracy_ordering(vertex_count, edges);
            if(core.degeneracy != ordering.degeneracy
                || core.degeneracy_order != ordering.order){
                return 2;
            }
            std::vector<int> rank(static_cast<std::size_t>(vertex_count));
            for(int index = 0; index < vertex_count; ++index){
                rank[static_cast<std::size_t>(
                    ordering.order[static_cast<std::size_t>(index)]
                )] = index;
            }
            for(int vertex = 0; vertex < vertex_count; ++vertex){
                int later = 0;
                for(auto [u, v]: edges){
                    if(u == v) continue;
                    if(v == vertex && rank[static_cast<std::size_t>(u)]
                        > rank[static_cast<std::size_t>(vertex)]) ++later;
                    if(u == vertex && rank[static_cast<std::size_t>(v)]
                        > rank[static_cast<std::size_t>(vertex)]) ++later;
                }
                if(later > ordering.degeneracy) return 3;
            }
            std::cout << core.degeneracy;
            for(int value: core.core_number) std::cout << ' ' << value;
            std::cout << '\n';
            continue;
        }
        if(kind == 1){
            auto orientation = strong_orientation(vertex_count, edges);
            if(orientation){
                if(orientation->size() != edges.size()) return 4;
                for(std::size_t index = 0; index < edges.size(); ++index){
                    auto [a, b] = edges[index];
                    auto [u, v] = (*orientation)[index];
                    if(!((a == u && b == v) || (a == v && b == u))){
                        return 5;
                    }
                }
                if(!is_strongly_connected(vertex_count, *orientation)) return 6;
            }
            std::cout << static_cast<int>(orientation.has_value()) << '\n';
            continue;
        }
        std::vector<std::vector<int>> graph(
            static_cast<std::size_t>(vertex_count)
        );
        for(auto [from, to]: edges){
            graph[static_cast<std::size_t>(from)].push_back(to);
        }
        if(kind == 2){
            auto reduced = transitive_reduction(graph);
            std::vector<std::vector<int>> reduced_graph(
                static_cast<std::size_t>(vertex_count)
            );
            for(auto [from, to]: reduced){
                reduced_graph[static_cast<std::size_t>(from)].push_back(to);
            }
            if(reachability(graph) != reachability(reduced_graph)) return 7;
            for(std::size_t removed = 0; removed < reduced.size(); ++removed){
                std::vector<std::vector<int>> smaller(
                    static_cast<std::size_t>(vertex_count)
                );
                for(std::size_t index = 0; index < reduced.size(); ++index){
                    if(index == removed) continue;
                    auto [from, to] = reduced[index];
                    smaller[static_cast<std::size_t>(from)].push_back(to);
                }
                if(reachability(smaller) == reachability(graph)) return 8;
            }
            std::cout << reduced.size();
            for(auto [from, to]: reduced){
                std::cout << ' ' << from << ' ' << to;
            }
            std::cout << '\n';
        }else if(kind == 3){
            MinimumPathCoverDagResult cover = minimum_path_cover_dag(graph);
            if(cover.count != static_cast<int>(cover.paths.size())) return 9;
            std::vector<int> seen(static_cast<std::size_t>(vertex_count), 0);
            std::set<std::pair<int, int>> edge_set(edges.begin(), edges.end());
            for(const auto& path: cover.paths){
                for(std::size_t index = 0; index < path.size(); ++index){
                    if(++seen[static_cast<std::size_t>(path[index])] != 1){
                        return 10;
                    }
                    if(index != 0
                        && !edge_set.contains({path[index - 1], path[index]})){
                        return 11;
                    }
                }
            }
            for(int value: seen) if(value != 1) return 12;
            std::cout << cover.count << '\n';
        }else{
            MaximumAntichainResult antichain = maximum_antichain(graph);
            auto reachable = reachability(graph);
            if(antichain.size
                != static_cast<int>(antichain.vertices.size())) return 13;
            for(int left: antichain.vertices){
                for(int right: antichain.vertices){
                    if(left != right
                        && reachable[static_cast<std::size_t>(left)]
                            [static_cast<std::size_t>(right)] != 0){
                        return 14;
                    }
                }
            }
            std::cout << antichain.size << '\n';
        }
    }
}
