// competitive-verifier: PROBLEM https://www.spoj.com/problems/CCHESS/

#include <iostream>

#include "../../src/structure/graph/coordinate_product_knight_distances.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    CoordinateProductKnightDistances distances(8);
    int start_x, start_y, target_x, target_y;
    while(std::cin >> start_x >> start_y >> target_x >> target_y){
        std::cout << distances.distance(start_x, start_y, target_x, target_y)
                         .value_or(-1)
                  << '\n';
    }
}
