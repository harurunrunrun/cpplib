// competitive-verifier: STANDALONE

#include <cstddef>
#include <iostream>
#include <vector>

#include "../../src/approximate/routing/tour_cost.hpp"
#include "../../src/approximate/routing/tree_tsp.hpp"

namespace {

void print_vector(const std::vector<int>& values){
    std::cout << values.size();
    for(const int value : values) std::cout << ' ' << value;
}

}  // namespace

int main(){
    using namespace approximate::routing;
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int query_count = 0;
    std::cin >> query_count;
    while(query_count-- > 0){
        std::size_t size = 0;
        std::size_t root = 0;
        std::cin >> size >> root;
        std::vector<std::vector<long long>> distance(
            size,
            std::vector<long long>(size)
        );
        for(auto& row : distance){
            for(long long& value : row) std::cin >> value;
        }
        const auto parent = minimum_spanning_tree_parents(distance, root);
        const auto double_tree = double_tree_tsp_tour(distance, root);
        const auto christofides = christofides_tsp_tour(distance, root);
        print_vector(parent);
        std::cout << " | ";
        print_vector(double_tree);
        std::cout << ' ' << tour_cost(distance, double_tree) << " | ";
        print_vector(christofides);
        std::cout << ' ' << tour_cost(distance, christofides) << '\n';
    }
}
