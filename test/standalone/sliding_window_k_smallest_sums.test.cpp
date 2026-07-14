// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <stdexcept>
#include <vector>

#include "../../src/algorithm/sequence/sliding_window_k_smallest_sums.hpp"

void self_test(){
    assert((sliding_window_k_smallest_sums<int, long long>({4, 1, 3, 2}, 3, 2)
        == std::vector<long long>{4, 3}));
    assert((sliding_window_k_smallest_sums<int, long long>({-2, 5, -2}, 2, 0)
        == std::vector<long long>{0, 0}));

    bool thrown = false;
    try{
        (void)sliding_window_k_smallest_sums<int>({}, 0, 0);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);
}

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int n, window_size, k;
    if(!(std::cin >> n >> window_size >> k)){
        self_test();
        return 0;
    }
    std::vector<long long> values(static_cast<std::size_t>(n));
    for(long long& value: values) std::cin >> value;
    const auto answer = sliding_window_k_smallest_sums<long long, long long>(
        values,
        static_cast<std::size_t>(window_size),
        static_cast<std::size_t>(k)
    );
    for(std::size_t index = 0; index < answer.size(); ++index){
        if(index != 0) std::cout << ' ';
        std::cout << answer[index];
    }
    std::cout << '\n';
}
