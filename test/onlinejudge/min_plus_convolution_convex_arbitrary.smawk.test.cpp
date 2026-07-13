// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/min_plus_convolution_convex_arbitrary

#include <algorithm>
#include <cstdint>
#include <iostream>
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

    // Padding the invalid band with a single infinity does not preserve total
    // monotonicity. Extend the convex sequence instead: the resulting full
    // Toeplitz matrix is Monge, and every padded entry is larger than every
    // valid entry.
    const auto [minimum_convex, maximum_convex] =
        std::minmax_element(convex.begin(), convex.end());
    const auto [minimum_arbitrary, maximum_arbitrary] =
        std::minmax_element(arbitrary.begin(), arbitrary.end());
    const __int128_t penalty =
        static_cast<__int128_t>(*maximum_convex)
        - static_cast<__int128_t>(*minimum_convex)
        + static_cast<__int128_t>(*maximum_arbitrary)
        - static_cast<__int128_t>(*minimum_arbitrary)
        + 1;

    const int output_size = n + m - 1;
    const auto column = smawk_row_minima(
        output_size,
        m,
        [&](int row, int candidate){
            const int convex_index = row - candidate;
            __int128_t convex_value;
            if(convex_index < 0){
                convex_value = static_cast<__int128_t>(convex.front())
                    - static_cast<__int128_t>(convex_index) * penalty;
            }else if(n <= convex_index){
                convex_value = static_cast<__int128_t>(convex.back())
                    + static_cast<__int128_t>(convex_index - n + 1) * penalty;
            }else{
                convex_value = convex[static_cast<std::size_t>(convex_index)];
            }
            return convex_value
                + static_cast<__int128_t>(arbitrary[static_cast<std::size_t>(candidate)]);
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
