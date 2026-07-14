// competitive-verifier: PROBLEM https://www.spoj.com/problems/KQUERYO/

#include <iostream>
#include <vector>

#include "../../src/structure/wavelet_matrix/xor_decoded_range_greater_query.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int size;
    std::cin >> size;
    std::vector<int> values(static_cast<std::size_t>(size));
    for(int& value: values) std::cin >> value;
    XorDecodedRangeGreaterQuery<int, 30000, 15> query(values);

    int query_count;
    std::cin >> query_count;
    while(query_count-- != 0){
        int encoded_left, encoded_right, encoded_threshold;
        std::cin >> encoded_left >> encoded_right >> encoded_threshold;
        std::cout << query.query(
                         encoded_left, encoded_right, encoded_threshold
                     )
                  << '\n';
    }
}
