// competitive-verifier: STANDALONE

#include <cstddef>
#include <iostream>
#include <vector>

#include "../../src/algorithm/fft/online/online_convolution.hpp"
#include "../../src/algorithm/fft/online/relaxed_convolution.hpp"

using mint = Modint998244353;

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int case_count;
    if(!(std::cin >> case_count)) return 1;
    while(case_count-- > 0){
        int kind;
        std::size_t count;
        std::cin >> kind >> count;
        if(kind == 0){
            RelaxedConvolution<998244353> convolution(count);
            for(std::size_t index = 0; index < count; ++index){
                mint left;
                mint right;
                std::cin >> left >> right;
                if(index != 0) std::cout << ' ';
                std::cout << convolution.append(left, right);
            }
            std::cout << '\n';
        }else{
            std::size_t kernel_size;
            std::cin >> kernel_size;
            std::vector<mint> kernel(kernel_size);
            for(mint& value: kernel) std::cin >> value;
            OnlineConvolution<998244353> convolution(std::move(kernel));
            for(std::size_t index = 0; index < count; ++index){
                mint value;
                std::cin >> value;
                if(index != 0) std::cout << ' ';
                std::cout << convolution.append(value);
            }
            std::cout << '\n';
        }
    }
}
