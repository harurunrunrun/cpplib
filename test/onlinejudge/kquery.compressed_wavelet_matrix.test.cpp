// competitive-verifier: PROBLEM https://www.spoj.com/problems/KQUERY/

#include <iostream>
#include <vector>

#include "../../src/structure/wavelet_matrix/compressed_wavelet_matrix.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int size;
    std::cin >> size;
    std::vector<int> values(static_cast<std::size_t>(size));
    for(int& value: values) std::cin >> value;
    CompressedWaveletMatrix<int, 30000> matrix(values);

    int query_count;
    std::cin >> query_count;
    while(query_count-- != 0){
        int left, right, threshold;
        std::cin >> left >> right >> threshold;
        std::cout << matrix.count_greater(left - 1, right, threshold) << '\n';
    }
}
