// competitive-verifier: PROBLEM https://www.spoj.com/problems/POSTERS/

#include <algorithm>
#include <iostream>
#include <memory>
#include <vector>

#include "../../src/structure/segtree/dualsegtree.hpp"
#include "../../src/structure/types/common_monoids.hpp"

constexpr AssignMaxMonoidAct<int> poster_assignment{};

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int test_count;
    std::cin >> test_count;
    while(test_count--){
        int n;
        std::cin >> n;
        std::vector<std::pair<int, int>> intervals(static_cast<std::size_t>(n));
        std::vector<int> boundaries;
        boundaries.reserve(static_cast<std::size_t>(2 * n));
        for(auto& [left, right]: intervals){
            std::cin >> left >> right;
            boundaries.push_back(left);
            boundaries.push_back(right + 1);
        }
        std::sort(boundaries.begin(), boundaries.end());
        boundaries.erase(std::unique(boundaries.begin(), boundaries.end()), boundaries.end());

        std::vector<int> initial(boundaries.size() - 1, 0);
        auto seg = std::make_unique<DualSegtree<poster_assignment, 80000>>(initial);
        for(int i = 0; i < n; i++){
            const auto [left, right] = intervals[static_cast<std::size_t>(i)];
            const int compressed_left = static_cast<int>(
                std::lower_bound(boundaries.begin(), boundaries.end(), left) - boundaries.begin()
            );
            const int compressed_right = static_cast<int>(
                std::lower_bound(boundaries.begin(), boundaries.end(), right + 1) - boundaries.begin()
            );
            seg->apply(compressed_left, compressed_right, {true, i + 1});
        }

        std::vector<bool> visible(static_cast<std::size_t>(n + 1), false);
        for(int i = 0; i + 1 < static_cast<int>(boundaries.size()); i++){
            visible[static_cast<std::size_t>(seg->get(i))] = true;
        }
        int answer = 0;
        for(int i = 1; i <= n; i++) answer += visible[static_cast<std::size_t>(i)];
        std::cout << answer << '\n';
    }
}
