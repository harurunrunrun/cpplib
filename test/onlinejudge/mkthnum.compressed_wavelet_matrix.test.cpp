#define PROBLEM "https://www.spoj.com/problems/MKTHNUM/"

#include <iostream>
#include <vector>

#include "../../src/structure/wavelet_matrix/compressed_wavelet_matrix.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int size, query_count;
    std::cin >> size >> query_count;
    std::vector<int> values(static_cast<std::size_t>(size));
    for(int& value: values) std::cin >> value;
    CompressedWaveletMatrix<int, 100000> matrix(values);

    while(query_count-- != 0){
        int left, right, order;
        std::cin >> left >> right >> order;
        std::cout << matrix.kth_smallest(left - 1, right, order - 1) << '\n';
    }
}
