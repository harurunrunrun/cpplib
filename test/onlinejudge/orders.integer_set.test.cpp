// competitive-verifier: PROBLEM https://www.spoj.com/problems/ORDERS/

#include <iostream>
#include <memory>
#include <vector>

#include "../../src/structure/tree/integer_set.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int test_count;
    std::cin >> test_count;
    while(test_count--){
        int n;
        std::cin >> n;
        std::vector<unsigned int> walked(static_cast<std::size_t>(n));
        for(unsigned int& distance : walked){
            std::cin >> distance;
        }

        auto ranks = std::make_unique<Int_Set<unsigned int, 200001U>>();
        for(int rank = 1; rank <= n; rank++){
            ranks->insert(static_cast<unsigned int>(rank));
        }

        std::vector<unsigned int> answer(static_cast<std::size_t>(n));
        for(int index = n - 1; index >= 0; index--){
            const auto rank = ranks->kth_le(
                static_cast<unsigned int>(n),
                walked[static_cast<std::size_t>(index)]
            );
            if(!rank){
                return 0;
            }
            answer[static_cast<std::size_t>(index)] = *rank;
            ranks->erase(*rank);
        }

        for(int index = 0; index < n; index++){
            if(index != 0){
                std::cout << ' ';
            }
            std::cout << answer[static_cast<std::size_t>(index)];
        }
        std::cout << '\n';
    }
}
