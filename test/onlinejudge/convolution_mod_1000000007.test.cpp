// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/convolution_mod_1000000007

#include <iostream>
#include <vector>

#include "../../src/algorithm/fft/convolution.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    using mint = Modint<1000000007>;
    int left_size, right_size;
    std::cin >> left_size >> right_size;
    std::vector<mint> left(static_cast<std::size_t>(left_size));
    std::vector<mint> right(static_cast<std::size_t>(right_size));
    for(mint& value: left) std::cin >> value;
    for(mint& value: right) std::cin >> value;
    const auto result = arbitrary_mod_convolution(left, right);
    for(std::size_t index = 0; index < result.size(); ++index){
        if(index != 0) std::cout << ' ';
        std::cout << result[index];
    }
    std::cout << '\n';
}
