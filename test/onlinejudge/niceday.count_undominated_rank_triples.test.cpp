// competitive-verifier: PROBLEM https://www.spoj.com/problems/NICEDAY/

#include <iostream>
#include <array>
#include <vector>

#include "../../src/algorithm/other/count_undominated_rank_triples.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int test_count;
    std::cin >> test_count;
    while(test_count-- != 0){
        int competitor_count;
        std::cin >> competitor_count;
        std::vector<std::array<int, 3>> ranks(
            static_cast<std::size_t>(competitor_count)
        );
        for(int i = 0; i < competitor_count; ++i){
            auto& rank = ranks[static_cast<std::size_t>(i)];
            std::cin >> rank[0] >> rank[1] >> rank[2];
            for(int& value: rank) --value;
        }

        std::cout << count_undominated_rank_triples(ranks) << '\n';
    }
}
