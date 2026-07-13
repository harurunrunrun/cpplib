#define PROBLEM "https://judge.yosupo.jp/problem/vertex_add_path_sum"

#include <iostream>
#include <memory>

#include "../../src/algorithm/tree/heavy_light_decomposition.hpp"
#include "../../src/structure/other/fenwick_tree.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int vertex_count, query_count;
    std::cin >> vertex_count >> query_count;
    auto decomposition = std::make_unique<HeavyLightDecomposition>(
        vertex_count
    );
    auto values = std::make_unique<long long[]>(
        static_cast<std::size_t>(vertex_count)
    );
    for(int vertex = 0; vertex < vertex_count; ++vertex){
        std::cin >> values[static_cast<std::size_t>(vertex)];
    }
    for(int edge = 1; edge < vertex_count; ++edge){
        int first, second;
        std::cin >> first >> second;
        decomposition->add_edge(first, second);
    }
    decomposition->build();

    auto fenwick = std::make_unique<FenwickTree<long long, 500000>>(
        vertex_count
    );
    for(int vertex = 0; vertex < vertex_count; ++vertex){
        fenwick->add(
            decomposition->in(vertex),
            values[static_cast<std::size_t>(vertex)]
        );
    }
    while(query_count-- != 0){
        int type, first;
        long long second;
        std::cin >> type >> first >> second;
        if(type == 0){
            fenwick->add(decomposition->in(first), second);
        }else{
            long long answer = 0;
            for(const auto [left, right]: decomposition->path_query(
                first, static_cast<int>(second)
            )){
                answer += fenwick->sum(left, right);
            }
            std::cout << answer << '\n';
        }
    }
}
