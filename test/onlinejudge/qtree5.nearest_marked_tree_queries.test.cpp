// competitive-verifier: PROBLEM https://br.spoj.com/SPOJ/problems/QTREE5/

#include <iostream>
#include "../../src/structure/tree/nearest_marked_tree_queries.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int n;
    std::cin >> n;
    NearestMarkedTreeQueries<int, 100000> tree(n);
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
        if(type == 0) (void)tree.toggle(vertex);
        else{
            const auto answer = tree.nearest_distance(vertex);
            std::cout << (answer ? *answer : -1) << '\n';
        }
    }
}
