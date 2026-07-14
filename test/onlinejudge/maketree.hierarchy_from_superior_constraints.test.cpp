// competitive-verifier: PROBLEM https://www.spoj.com/problems/MAKETREE/

#include <iostream>
#include <vector>

#include "../../src/algorithm/graph/hierarchy_from_superior_constraints.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int size, successful_count;
    std::cin >> size >> successful_count;
    std::vector<std::vector<int>> constraints(static_cast<std::size_t>(size));
    for(int student = 0; student < successful_count; ++student){
        int wish_count;
        std::cin >> wish_count;
        while(wish_count-- != 0){
            int subordinate;
            std::cin >> subordinate;
            constraints[static_cast<std::size_t>(student)].push_back(
                subordinate - 1
            );
        }
    }
    const auto parent = hierarchy_from_superior_constraints(constraints);
    for(const int boss: parent) std::cout << boss + 1 << '\n';
}
