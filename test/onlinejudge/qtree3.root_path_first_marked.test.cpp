// competitive-verifier: PROBLEM https://www.spoj.com/problems/QTREE3/

#include <iostream>
#include "../../src/structure/tree/root_path_first_marked.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int n, q;
    std::cin >> n >> q;
    RootPathFirstMarked<100000> tree(n);
    for(int edge = 1; edge < n; ++edge){
        int u, v;
        std::cin >> u >> v;
        tree.add_edge(u - 1, v - 1);
    }
    tree.build();
    while(q-- != 0){
        int type, vertex;
        std::cin >> type >> vertex;
        --vertex;
        if(type == 0) (void)tree.toggle(vertex);
        else{
            const int answer = tree.first_marked(vertex);
            std::cout << (answer == -1 ? -1 : answer + 1) << '\n';
        }
    }
}
