// competitive-verifier: STANDALONE

#include <iostream>

#include "../../src/structure/dsu/weighted_parent_forest_distance.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int case_count;
    if(!(std::cin >> case_count)) return 0;
    while(case_count-- > 0){
        int vertex_count, operation_count;
        std::cin >> vertex_count >> operation_count;
        WeightedParentForestDistance<long long, 5000> forest(vertex_count);
        while(operation_count-- > 0){
            char operation;
            int vertex;
            std::cin >> operation >> vertex;
            if(operation == 'D'){
                std::cout << forest.distance_to_root(vertex) << '\n';
            }else if(operation == 'R'){
                std::cout << forest.root(vertex) << '\n';
            }else{
                int parent_vertex;
                long long weight;
                std::cin >> parent_vertex >> weight;
                forest.link_root(vertex, parent_vertex, weight);
            }
        }
    }
}
