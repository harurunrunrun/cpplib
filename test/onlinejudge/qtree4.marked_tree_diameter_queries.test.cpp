// competitive-verifier: PROBLEM https://www.spoj.com/problems/QTREE4/

#include <iostream>
#include "../../src/structure/tree/marked_tree_diameter_queries.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int n;
    std::cin >> n;
    MarkedTreeDiameterQueries<long long, 100000> tree(n);
    for(int edge = 1; edge < n; ++edge){
        int u, v;
        long long weight;
        std::cin >> u >> v >> weight;
        tree.add_edge(u - 1, v - 1, weight);
    }
    tree.build();
    int q;
    std::cin >> q;
    while(q-- != 0){
        char operation;
        std::cin >> operation;
        if(operation == 'C'){
            int vertex;
            std::cin >> vertex;
            (void)tree.toggle(vertex - 1);
        }else{
            const auto answer = tree.diameter();
            if(answer) std::cout << *answer << '\n';
            else std::cout << "They have disappeared.\n";
        }
    }
}
