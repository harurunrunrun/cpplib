// competitive-verifier: PROBLEM https://www.spoj.com/problems/CORNET/

#include <iostream>

#include "../../src/structure/dsu/weighted_parent_forest_distance.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int test_count;
    std::cin >> test_count;
    while(test_count-- != 0){
        int vertex_count;
        std::cin >> vertex_count;
        WeightedParentForestDistance<long long, 20000> forest(vertex_count);
        char command;
        while(std::cin >> command && command != 'O'){
            int vertex;
            std::cin >> vertex;
            --vertex;
            if(command == 'E'){
                std::cout << forest.distance_to_root(vertex) << '\n';
            }else{
                int parent;
                std::cin >> parent;
                --parent;
                const long long difference =
                    static_cast<long long>(vertex) - parent;
                const long long weight =
                    (difference < 0 ? -difference : difference) % 1000;
                forest.link_root(vertex, parent, weight);
            }
        }
    }
}
