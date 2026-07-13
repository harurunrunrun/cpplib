// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "../../src/algorithm/fft/divisor_convolution.hpp"

void test_basic(){
    const std::vector<long long> a = {1, 2, 3, 4};
    const std::vector<long long> b = {5, 6, 7, 8};
    assert((gcd_convolution(a, b) == std::vector<long long>{155, 52, 21, 32}));
    assert((lcm_convolution(a, b) == std::vector<long long>{5, 28, 43, 100}));
    assert(gcd_convolution(std::vector<int>{}, std::vector<int>{}).empty());

    bool thrown = false;
    try{
        (void)gcd_convolution(std::vector<int>{1}, std::vector<int>{1, 2});
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);
}

int main(){
    std::string type;
    int n;
    if(std::cin >> type >> n){
        std::vector<long long> a(static_cast<std::size_t>(n));
        std::vector<long long> b(static_cast<std::size_t>(n));
        for(auto& value: a) std::cin >> value;
        for(auto& value: b) std::cin >> value;
        const auto answer = type == "GCD" ? gcd_convolution(a, b) : lcm_convolution(a, b);
        for(int i = 0; i < n; i++){
            if(i != 0) std::cout << ' ';
            std::cout << answer[static_cast<std::size_t>(i)];
        }
        std::cout << '\n';
        return 0;
    }
    test_basic();
}
