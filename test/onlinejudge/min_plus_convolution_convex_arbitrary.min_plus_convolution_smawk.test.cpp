// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/min_plus_convolution_convex_arbitrary

#include <iostream>
#include <vector>

#include "../../src/algorithm/fft/min_plus_convolution_smawk.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int n, m;
    std::cin >> n >> m;
    std::vector<long long> convex(static_cast<std::size_t>(n));
    std::vector<long long> arbitrary(static_cast<std::size_t>(m));
    for(long long& value: convex) std::cin >> value;
    for(long long& value: arbitrary) std::cin >> value;

    const auto answer = min_plus_convolution_convex_arbitrary_smawk(
        convex, arbitrary
    );
    for(std::size_t index = 0; index < answer.size(); ++index){
        if(index != 0) std::cout << ' ';
        std::cout << answer[index];
    }
    std::cout << '\n';
}
