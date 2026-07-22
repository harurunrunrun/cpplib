// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/intersection_of_f2_vector_spaces

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <vector>

#include "../../src/algorithm/math/linear_algebra/intersection_of_f2_vector_spaces.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int test_count;
    std::cin >> test_count;
    while(test_count-- > 0){
        std::size_t first_size;
        std::cin >> first_size;
        std::vector<std::uint32_t> first(first_size);
        for(auto& value: first) std::cin >> value;

        std::size_t second_size;
        std::cin >> second_size;
        std::vector<std::uint32_t> second(second_size);
        for(auto& value: second) std::cin >> value;

        const auto result = intersection_of_f2_vector_spaces(first, second);
        std::cout << result.size();
        for(std::uint32_t value: result) std::cout << ' ' << value;
        std::cout << '\n';
    }
}
