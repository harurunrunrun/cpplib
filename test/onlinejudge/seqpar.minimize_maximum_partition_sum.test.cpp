#define PROBLEM "https://www.spoj.com/problems/SEQPAR/"

#include <cstddef>
#include <iostream>
#include <vector>

#include "../../src/algorithm/other/minimize_maximum_partition_sum.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int test_count;
    std::cin >> test_count;
    while(test_count-- > 0){
        int size;
        int partition_count;
        std::cin >> size >> partition_count;
        std::vector<long long> values(static_cast<std::size_t>(size));
        for(long long& value: values) std::cin >> value;
        std::cout << minimize_maximum_partition_sum(
            values,
            static_cast<std::size_t>(partition_count)
        ) << '\n';
    }
}
