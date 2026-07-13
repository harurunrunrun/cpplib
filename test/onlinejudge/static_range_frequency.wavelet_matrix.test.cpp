// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/static_range_frequency

#include <iostream>
#include <vector>
#include "../../src/structure/wavelet_matrix/compressed_wavelet_matrix.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int n, q;
    std::cin >> n >> q;
    std::vector<int> values(static_cast<std::size_t>(n));
    for(int& value: values) std::cin >> value;
    const CompressedWaveletMatrix<int, 500000> matrix(values);
    while(q--){
        int left, right, value;
        std::cin >> left >> right >> value;
        std::cout << matrix.rank(value, left, right) << '\n';
    }
}
