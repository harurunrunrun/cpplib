// competitive-verifier: PROBLEM https://www.spoj.com/problems/FREQUENT/

#include <algorithm>
#include <iostream>
#include <memory>
#include <vector>

#include "../../src/structure/other/sparse_table.hpp"
#include "../../src/structure/types/common_monoids.hpp"

constexpr MaxMonoid<int> max_monoid{};

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int n;
    while(std::cin >> n && n != 0){
        int q;
        std::cin >> q;

        std::vector<int> values(n);
        for(int& value : values){
            std::cin >> value;
        }

        std::vector<int> group(n);
        std::vector<int> begin;
        std::vector<int> end;
        std::vector<int> frequency;
        for(int left = 0; left < n;){
            int right = left + 1;
            while(right < n && values[right] == values[left]){
                right++;
            }
            const int id = static_cast<int>(frequency.size());
            begin.push_back(left);
            end.push_back(right);
            frequency.push_back(right - left);
            std::fill(group.begin() + left, group.begin() + right, id);
            left = right;
        }

        auto table = std::make_unique<SparseTable<max_monoid, 100000>>(frequency);
        while(q--){
            int left, right;
            std::cin >> left >> right;
            left--;

            const int left_group = group[left];
            const int right_group = group[right - 1];
            if(left_group == right_group){
                std::cout << right - left << '\n';
                continue;
            }

            const int left_frequency = end[left_group] - left;
            const int right_frequency = right - begin[right_group];
            const int middle_frequency = table->prod(left_group + 1, right_group);
            std::cout << std::max({left_frequency, middle_frequency, right_frequency}) << '\n';
        }
    }
}
