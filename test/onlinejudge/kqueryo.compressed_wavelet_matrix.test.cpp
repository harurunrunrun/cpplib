// competitive-verifier: PROBLEM https://www.spoj.com/problems/KQUERYO/

#include <algorithm>
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
    CompressedWaveletMatrix<int, 30000, 15> matrix(values);

    int query_count;
    std::cin >> query_count;
    int last_answer = 0;
    while(query_count-- != 0){
        int encoded_left, encoded_right, encoded_threshold;
        std::cin >> encoded_left >> encoded_right >> encoded_threshold;
        int left = encoded_left ^ last_answer;
        int right = encoded_right ^ last_answer;
        const int threshold = encoded_threshold ^ last_answer;
        left = std::max(left, 1);
        right = std::min(right, size);

        if(left > right){
            last_answer = 0;
        }else{
            const int less_or_equal = matrix.range_freq(
                left - 1,
                right,
                threshold + 1
            );
            last_answer = right - left + 1 - less_or_equal;
        }
        std::cout << last_answer << '\n';
    }
}
