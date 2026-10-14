// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/enumerate_primes
#include <iostream>
#include "../../src/algorithm/math/number_theory/prime/enumerate_sampled_primes.hpp"
int main(){
    std::ios::sync_with_stdio(false); std::cin.tie(nullptr);
    int n,a,b; std::cin >> n >> a >> b;
    const auto result=math::enumerate_sampled_primes(n,a,b);
    std::cout << result.count << ' ' << result.primes.size() << '\n';
    for(std::size_t i=0;i<result.primes.size();++i)
        std::cout << result.primes[i] << (i+1==result.primes.size()?'\n':' ');
    if(result.primes.empty()) std::cout << '\n';
}
