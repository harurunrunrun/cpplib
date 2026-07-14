// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <vector>

#include "../../src/structure/wavelet_matrix/xor_decoded_range_greater_query.hpp"

void self_test(){
    XorDecodedRangeGreaterQuery<int, 4> query({1, 4, 2, 8});
    assert(query.size() == 4);
    assert(query.last_answer() == 0);
    assert(query.query(1, 4, 2) == 2);
    assert(query.query(2, 6, 0) == 2);
    query.reset();
    assert(query.last_answer() == 0);
    assert(query.query(5, 0, 7) == 0);
}

int main(){
    self_test();

    int size;
    if(!(std::cin >> size)) return 0;
    std::vector<int> values(static_cast<std::size_t>(size));
    for(auto& value: values) std::cin >> value;
    XorDecodedRangeGreaterQuery<int, 30000, 15> query(values);
    int query_count;
    std::cin >> query_count;
    while(query_count-- > 0){
        int left, right, threshold;
        std::cin >> left >> right >> threshold;
        std::cout << query.query(left, right, threshold) << '\n';
    }
}
