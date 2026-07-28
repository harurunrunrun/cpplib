// competitive-verifier: STANDALONE

#include <cstddef>
#include <iostream>
#include <numeric>
#include <set>
#include <utility>
#include <vector>

#include "../../src/algorithm/graph/connectivity/decomposition/nagamochi_ibaraki_sparsification.hpp"
#include "../../src/algorithm/graph/specialized/subgraph_analysis/maximum_density_subgraph.hpp"

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
        int connectivity = 0;
        if(kind == 1) std::cin >> connectivity;
        std::vector<std::pair<int, int>> edges(
            static_cast<std::size_t>(edge_count)
        );
        for(auto& [left, right]: edges) std::cin >> left >> right;
        if(kind == 0){
            auto result = maximum_density_subgraph(vertex_count, edges);
            if(!result){
                std::cout << "none\n";
                continue;
            }
            std::vector<unsigned char> selected(
                static_cast<std::size_t>(vertex_count), 0
            );
            for(int vertex: result->vertices){
                selected[static_cast<std::size_t>(vertex)] = 1;
            }
            std::set<std::pair<int, int>> internal_edges;
            for(auto [left, right]: edges){
                if(left != right
                    && selected[static_cast<std::size_t>(left)] != 0
                    && selected[static_cast<std::size_t>(right)] != 0){
                    if(right < left) std::swap(left, right);
                    internal_edges.emplace(left, right);
                }
            }
            if(static_cast<long long>(internal_edges.size())
                    * result->denominator
                != result->numerator
                    * static_cast<long long>(result->vertices.size())){
                return 2;
            }
            std::cout << result->numerator << ' '
                      << result->denominator << '\n';
        }else{
            std::vector<int> selected =
                nagamochi_ibaraki_sparsification(
                    vertex_count, edges, connectivity
                );
            if(vertex_count <= 20){
                std::vector<unsigned char> kept(
                    static_cast<std::size_t>(edge_count), 0
                );
                for(int edge_id: selected){
                    kept[static_cast<std::size_t>(edge_id)] = 1;
                }
                const unsigned limit = 1U
                    << static_cast<unsigned>(vertex_count);
                for(unsigned mask = 0; mask < limit; ++mask){
                    int original_cut = 0;
                    int sparse_cut = 0;
                    for(int edge_id = 0; edge_id < edge_count; ++edge_id){
                        auto [left, right] =
                            edges[static_cast<std::size_t>(edge_id)];
                        if(((mask >> left) & 1U)
                            == ((mask >> right) & 1U)){
                            continue;
                        }
                        ++original_cut;
                        sparse_cut +=
                            kept[static_cast<std::size_t>(edge_id)] != 0;
                    }
                    if(std::min(connectivity, original_cut)
                        != std::min(connectivity, sparse_cut)){
                        return 3;
                    }
                }
            }
            std::cout << selected.size();
            for(int edge_id: selected) std::cout << ' ' << edge_id;
            std::cout << '\n';
        }
    }
}
