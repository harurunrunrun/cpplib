// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/min_plus_convolution_convex_arbitrary

#include <algorithm>
#include <iostream>
#include <limits>
#include <vector>

#include "../../src/algorithm/other/smawk.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int n, m;
    std::cin >> n >> m;
    std::vector<long long> convex(static_cast<std::size_t>(n));
    std::vector<long long> arbitrary(static_cast<std::size_t>(m));
    for(long long& value: convex) std::cin >> value;
    for(long long& value: arbitrary) std::cin >> value;

    const int output_size = n + m - 1;
    const auto column = smawk_row_minima(
        output_size,
        m,
        [&](int row, int candidate){
            const int convex_index = row - candidate;
            if(convex_index < 0 || n <= convex_index){
                return std::numeric_limits<long long>::max();
            }
            return convex[static_cast<std::size_t>(convex_index)]
                + arbitrary[static_cast<std::size_t>(candidate)];
        }
    );
    for(int index = 0; index < output_size; ++index){
        if(index != 0) std::cout << ' ';
        const int candidate = column[static_cast<std::size_t>(index)];
        std::cout << convex[static_cast<std::size_t>(index - candidate)]
                  + arbitrary[static_cast<std::size_t>(candidate)];
    }
    std::cout << '\n';
}
