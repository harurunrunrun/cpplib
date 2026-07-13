// competitive-verifier: PROBLEM https://atcoder.jp/contests/abc231/tasks/abc231_f

#include <algorithm>
#include <iostream>
#include <tuple>
#include <vector>

#include "../../src/structure/other/fenwick_tree.hpp"

int main(){
    int n;
    std::cin >> n;
    std::vector<int> a(static_cast<std::size_t>(n));
    std::vector<int> b(static_cast<std::size_t>(n));
    for(int& value: a) std::cin >> value;
    for(int& value: b) std::cin >> value;
    std::vector<int> compressed = b;
    std::sort(compressed.begin(), compressed.end());
    compressed.erase(std::unique(compressed.begin(), compressed.end()), compressed.end());
    std::vector<std::pair<int, int>> candidates(static_cast<std::size_t>(n));
    for(int i = 0; i < n; ++i){
        candidates[static_cast<std::size_t>(i)] = {a[static_cast<std::size_t>(i)], b[static_cast<std::size_t>(i)]};
    }
    std::sort(candidates.begin(), candidates.end(), [](const auto& lhs, const auto& rhs){
        return lhs.first > rhs.first;
    });

    FenwickTree<int, 200000> fenwick(static_cast<int>(compressed.size()));
    long long answer = 0;
    for(int begin = 0; begin < n; ){
        int end = begin;
        while(end < n && candidates[static_cast<std::size_t>(end)].first ==
            candidates[static_cast<std::size_t>(begin)].first){
            const int value = candidates[static_cast<std::size_t>(end)].second;
            const int index = static_cast<int>(
                std::lower_bound(compressed.begin(), compressed.end(), value) - compressed.begin()
            );
            fenwick.add(index, 1);
            ++end;
        }
        for(int i = begin; i < end; ++i){
            const int value = candidates[static_cast<std::size_t>(i)].second;
            const int right = static_cast<int>(
                std::upper_bound(compressed.begin(), compressed.end(), value) - compressed.begin()
            );
            answer += fenwick.prefix_sum(right);
        }
        begin = end;
    }
    std::cout << answer << '\n';
}
