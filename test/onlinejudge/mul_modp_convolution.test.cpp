// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/mul_modp_convolution
#include <iostream>
#include <vector>
#include "../../src/algorithm/fft/transform/multiplicative_convolution.hpp"
int main(){
    std::ios::sync_with_stdio(false);std::cin.tie(nullptr);
    int p;std::cin>>p;std::vector<Modint998244353>a(p),b(p);
    for(auto& x:a)std::cin>>x;for(auto& x:b)std::cin>>x;
    const auto c=prime_multiplicative_convolution(a,b);
    for(int i=0;i<p;++i)std::cout<<c[i]<<(i+1==p?'\n':' ');
}
