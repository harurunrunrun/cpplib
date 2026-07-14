#define PROBLEM "https://www.spoj.com/problems/QTREE3/"

#include <iostream>
#include <set>

#include "../../src/algorithm/tree/heavy_light_decomposition.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int vertex_count, query_count;
    std::cin >> vertex_count >> query_count;

    HeavyLightDecomposition decomposition(vertex_count);
    for(int edge = 1; edge < vertex_count; ++edge){
        int left, right;
        std::cin >> left >> right;
        decomposition.add_edge(left - 1, right - 1);
    }
    decomposition.build(0);

    std::set<int> black_positions;
    while(query_count-- != 0){
        int type, vertex;
        std::cin >> type >> vertex;
        --vertex;
        const int position = decomposition.in(vertex);
        if(type == 0){
            const auto iterator = black_positions.find(position);
            if(iterator == black_positions.end()){
                black_positions.insert(position);
            }else{
                black_positions.erase(iterator);
            }
            continue;
        }

        int answer = -1;
        for(const auto [left, right]:
            decomposition.path_query(0, vertex, true)){
            const auto iterator = black_positions.lower_bound(left);
            if(iterator != black_positions.end() && *iterator < right){
                answer = decomposition[*iterator] + 1;
                break;
            }
        }
        std::cout << answer << '\n';
    }
}
