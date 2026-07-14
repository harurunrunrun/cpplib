// competitive-verifier: PROBLEM https://www.spoj.com/problems/NICEDAY/

#include <iostream>
#include <limits>
#include <memory>
#include <utility>
#include <vector>

#include "../../src/structure/segtree/segtree.hpp"
#include "../../src/structure/types/common_monoids.hpp"

namespace{

constexpr MinMonoid<int> minimum_monoid{};

} // namespace

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int test_count;
    std::cin >> test_count;
    while(test_count-- != 0){
        int competitor_count;
        std::cin >> competitor_count;
        std::vector<std::pair<int, int>> by_first(
            static_cast<std::size_t>(competitor_count)
        );
        for(int i = 0; i < competitor_count; ++i){
            int first;
            int second;
            int third;
            std::cin >> first >> second >> third;
            by_first[static_cast<std::size_t>(first - 1)] = {second, third};
        }

        auto minimum_third = std::make_unique<Segtree<minimum_monoid, 100000>>(
            competitor_count
        );
        int excellent = 0;
        for(const auto [second, third]: by_first){
            if(minimum_third->prod(0, second - 1) >= third){
                ++excellent;
            }
            minimum_third->set(second - 1, third);
        }
        std::cout << excellent << '\n';
    }
}
