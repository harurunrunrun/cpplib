// competitive-verifier: PROBLEM https://www.spoj.com/problems/CORNET/

#include <cstdlib>
#include <iostream>
#include <vector>

#include "../../src/structure/dsu/weighted_dsu.hpp"

namespace{

int serving_center(int vertex, std::vector<int>& parent){
    int root = vertex;
    while(parent[static_cast<std::size_t>(root)] != root){
        root = parent[static_cast<std::size_t>(root)];
    }
    while(parent[static_cast<std::size_t>(vertex)] != vertex){
        const int next = parent[static_cast<std::size_t>(vertex)];
        parent[static_cast<std::size_t>(vertex)] = root;
        vertex = next;
    }
    return root;
}

}

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int test_count;
    std::cin >> test_count;
    while(test_count-- != 0){
        int enterprise_count;
        std::cin >> enterprise_count;
        WeightedDSU<int, 20000> network(enterprise_count);
        std::vector<int> parent(static_cast<std::size_t>(enterprise_count));
        for(int vertex = 0; vertex < enterprise_count; ++vertex){
            parent[static_cast<std::size_t>(vertex)] = vertex;
        }

        char command;
        while(std::cin >> command && command != 'O'){
            int enterprise;
            std::cin >> enterprise;
            --enterprise;
            if(command == 'E'){
                const int center = serving_center(enterprise, parent);
                std::cout << network.diff(center, enterprise) << '\n';
                continue;
            }

            int destination;
            std::cin >> destination;
            --destination;
            const int length = std::abs(enterprise - destination) % 1000;
            parent[static_cast<std::size_t>(enterprise)] = destination;
            (void)network.merge(destination, enterprise, length);
        }
    }
}
