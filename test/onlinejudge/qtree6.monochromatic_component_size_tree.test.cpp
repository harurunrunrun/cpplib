// competitive-verifier: PROBLEM https://br.spoj.com/SPOJ/problems/QTREE6/

#include <iostream>
#include "../../src/structure/tree/monochromatic_component_size_tree.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int n;
    std::cin >> n;
    MonochromaticComponentSizeTree<100000> tree(n);
    for(int edge = 1; edge < n; ++edge){
        int u, v;
        std::cin >> u >> v;
        tree.add_edge(u - 1, v - 1);
    }
    tree.build();
    int q;
    std::cin >> q;
    while(q-- != 0){
        int type, vertex;
        std::cin >> type >> vertex;
        --vertex;
        if(type == 0) std::cout << tree.component_size(vertex) << '\n';
        else (void)tree.toggle(vertex);
    }
}
