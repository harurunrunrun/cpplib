// competitive-verifier: PROBLEM https://br.spoj.com/SPOJ/problems/QTREE7/

#include <iostream>
#include "../../src/structure/tree/monochromatic_component_max_tree.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int n;
    std::cin >> n;
    MonochromaticComponentMaxTree<long long, 100000> tree(n);
    for(int edge = 1; edge < n; ++edge){
        int u, v;
        std::cin >> u >> v;
        tree.add_edge(u - 1, v - 1);
    }
    for(int vertex = 0; vertex < n; ++vertex){
        int color;
        std::cin >> color;
        (void)tree.set_color(vertex, color != 0);
    }
    for(int vertex = 0; vertex < n; ++vertex){
        long long value;
        std::cin >> value;
        tree.set_value(vertex, value);
    }
    tree.build();
    int q;
    std::cin >> q;
    while(q-- != 0){
        int type, vertex;
        std::cin >> type >> vertex;
        --vertex;
        if(type == 0) std::cout << tree.component_maximum(vertex) << '\n';
        else if(type == 1) (void)tree.toggle(vertex);
        else{
            long long value;
            std::cin >> value;
            tree.set_value(vertex, value);
        }
    }
}
