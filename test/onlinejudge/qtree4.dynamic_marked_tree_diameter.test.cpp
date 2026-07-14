// competitive-verifier: PROBLEM https://www.spoj.com/problems/QTREE4/

#include <iostream>
#include <string>

#include "../../src/structure/tree/dynamic_marked_tree_diameter.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int vertex_count;
    std::cin >> vertex_count;
    DynamicMarkedTreeDiameter<long long, 100000> tree(vertex_count);
    for(int edge = 1; edge < vertex_count; ++edge){
        int left, right;
        long long weight;
        std::cin >> left >> right >> weight;
        tree.add_edge(left - 1, right - 1, weight);
    }
    tree.build(true);

    int query_count;
    std::cin >> query_count;
    while(query_count-- != 0){
        char operation;
        std::cin >> operation;
        if(operation == 'C'){
            int vertex;
            std::cin >> vertex;
            tree.toggle(vertex - 1);
        }else{
            const auto answer = tree.diameter();
            if(answer) std::cout << *answer << '\n';
            else std::cout << "They have disappeared.\n";
        }
    }
}
