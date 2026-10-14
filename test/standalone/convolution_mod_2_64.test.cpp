// competitive-verifier: STANDALONE

#include <cassert>
#include <cstdint>
#include <iostream>
#include <vector>

#include "../../src/algorithm/fft/transform/convolution_mod_2_64.hpp"
#include "../../src/algorithm/fft/transform/cyclic_correlation_64.hpp"

void test_cyclic_correlation(){
    const std::vector<std::uint64_t> a{UINT64_MAX, 3, 5}, b{7, 11, std::uint64_t(1) << 63};
    std::vector<std::uint64_t> ring(a.size()), field(a.size());
    const auto field_multiply = [](std::uint64_t x, std::uint64_t y){
        std::uint64_t result = 0;
        for(int bit = 0; bit < 64; ++bit){
            if(y & 1) result ^= x;
            const bool carry = x >> 63;
            x <<= 1;
            if(carry) x ^= 0x1b;
            y >>= 1;
        }
        return result;
    };
    for(std::size_t shift = 0; shift < a.size(); ++shift)
        for(std::size_t i = 0; i < a.size(); ++i){
            ring[shift] += a[i] * b[(i + shift) % a.size()];
            field[shift] ^= field_multiply(a[i], b[(i + shift) % a.size()]);
        }
    assert(cyclic_correlation_mod_2_64(a, b) == ring);
    assert(cyclic_correlation_gf2_64(a, b) == field);
    assert(cyclic_correlation_mod_2_64({}, {}).empty());
    assert(cyclic_correlation_gf2_64({}, {}).empty());
}

int main(){
    test_cyclic_correlation();
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int case_count;
    if(!(std::cin >> case_count)) return 2;
    while(case_count--){
        int first_size;
        int second_size;
        std::cin >> first_size >> second_size;
        std::vector<std::uint64_t> first(
            static_cast<std::size_t>(first_size)
        );
        std::vector<std::uint64_t> second(
            static_cast<std::size_t>(second_size)
        );
        for(std::uint64_t& value: first) std::cin >> value;
        for(std::uint64_t& value: second) std::cin >> value;
        const auto answer = convolution_mod_2_64(first, second);
        for(std::size_t index = 0; index < answer.size(); ++index){
            if(index) std::cout << ' ';
            std::cout << answer[index];
        }
        std::cout << '\n';
    }
}
