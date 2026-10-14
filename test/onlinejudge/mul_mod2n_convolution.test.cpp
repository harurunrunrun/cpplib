// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/mul_mod2n_convolution
#include <iostream>
#include <vector>
#include "../../src/algorithm/fft/transform/multiplicative_convolution.hpp"
int main(){
    std::ios::sync_with_stdio(false);std::cin.tie(nullptr);
    int n;std::cin>>n;const int size=1<<n;std::vector<Modint998244353>a(size),b(size);
    for(auto& x:a)std::cin>>x;for(auto& x:b)std::cin>>x;
    const auto c=power_two_multiplicative_convolution(a,b);
    for(int i=0;i<size;++i)std::cout<<c[i]<<(i+1==size?'\n':' ');
}
