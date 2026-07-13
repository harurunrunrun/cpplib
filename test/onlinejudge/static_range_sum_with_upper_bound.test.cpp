// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/static_range_sum_with_upper_bound

#include <iostream>
#include <vector>

#include "../../src/structure/wavelet_matrix/weighted_wavelet_matrix.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int size, query_count;
    std::cin >> size >> query_count;
    std::vector<int> values(static_cast<std::size_t>(size));
    std::vector<long long> weights(static_cast<std::size_t>(size));
    for(int index = 0; index < size; ++index){
        std::cin >> values[static_cast<std::size_t>(index)];
        weights[static_cast<std::size_t>(index)] =
            values[static_cast<std::size_t>(index)];
    }
    WeightedWaveletMatrix<int, long long, 500000> matrix(values, weights);
    while(query_count--){
        int left, right, upper;
        std::cin >> left >> right >> upper;
        ++upper;
        std::cout << matrix.range_freq(left, right, upper) << ' '
                  << matrix.range_sum(left, right, upper) << '\n';
    }
}
