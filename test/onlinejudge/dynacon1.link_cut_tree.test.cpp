// competitive-verifier: PROBLEM https://www.spoj.com/problems/DYNACON1/

#include <iostream>
#include <string>

#include "../../src/structure/tree/link_cut_tree.hpp"
#include "../../src/structure/types/common_monoids.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int vertex_count, query_count;
    std::cin >> vertex_count >> query_count;
    LinkCutTree<AddMonoid<int>{}, 100000> forest(vertex_count);
    while(query_count-- != 0){
        std::string command;
        int left, right;
        std::cin >> command >> left >> right;
        --left;
        --right;
        if(command == "add"){
            (void)forest.link(left, right);
        }else if(command == "rem"){
            (void)forest.cut(left, right);
        }else{
            std::cout << (forest.connected(left, right) ? "YES\n" : "NO\n");
        }
    }
}
