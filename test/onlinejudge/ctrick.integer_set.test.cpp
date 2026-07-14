// competitive-verifier: PROBLEM https://www.spoj.com/problems/CTRICK/

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
        auto positions = std::make_unique<Int_Set<unsigned int, 20000U>>();
        for(int position = 0; position < n; position++){
            positions->insert(static_cast<unsigned int>(position));
        }

        std::vector<int> answer(static_cast<std::size_t>(n));
        int cursor = 0;
        int remaining = n;
        for(int card = 1; card <= n; card++){
            cursor = (cursor + card) % remaining;
            const auto position = positions->kth_ge(
                0U,
                static_cast<unsigned int>(cursor)
            );
            if(!position){
                return 0;
            }
            answer[*position] = card;
            positions->erase(*position);
            remaining--;
            if(remaining != 0){
                cursor %= remaining;
            }
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
