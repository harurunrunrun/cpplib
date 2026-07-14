// competitive-verifier: PROBLEM https://br.spoj.com/SPOJ/problems/QTREE6/

#include <iostream>
#include <vector>

#include "../../src/structure/tree/dynamic_monochromatic_tree.hpp"
#include "../../src/structure/types/common_monoids.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int vertex_count;
    std::cin >> vertex_count;
    DynamicMonochromaticTree<AddMonoid<int>{}, 100000> tree(
        std::vector<int>(static_cast<std::size_t>(vertex_count), 1)
    );
    for(int edge = 1; edge < vertex_count; ++edge){
        int left, right;
        std::cin >> left >> right;
        tree.add_edge(left - 1, right - 1);
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
        }else{
            tree.toggle(vertex);
        }
    }
}
