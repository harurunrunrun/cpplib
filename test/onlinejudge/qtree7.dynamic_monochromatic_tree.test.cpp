// competitive-verifier: PROBLEM https://br.spoj.com/SPOJ/problems/QTREE7/

#include <iostream>

#include "../../src/structure/tree/dynamic_monochromatic_tree.hpp"
#include "../../src/structure/types/common_monoids.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int vertex_count;
    std::cin >> vertex_count;
    DynamicMonochromaticTree<MaxMonoid<long long>{}, 100000> tree(vertex_count);
    for(int edge = 1; edge < vertex_count; ++edge){
        int left, right;
        std::cin >> left >> right;
        tree.add_edge(left - 1, right - 1);
    }
    for(int vertex = 0; vertex < vertex_count; ++vertex){
        int color;
        std::cin >> color;
        (void)tree.set_color(vertex, color != 0);
    }
    for(int vertex = 0; vertex < vertex_count; ++vertex){
        long long value;
        std::cin >> value;
        tree.set(vertex, value);
    }
    tree.build();

    int query_count;
    std::cin >> query_count;
    while(query_count-- != 0){
        int operation, vertex;
        std::cin >> operation >> vertex;
        --vertex;
        if(operation == 0){
            std::cout << tree.component_prod(vertex) << '\n';
        }else if(operation == 1){
            tree.toggle(vertex);
        }else{
            long long value;
            std::cin >> value;
            tree.set(vertex, value);
        }
    }
}
