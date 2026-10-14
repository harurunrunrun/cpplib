// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/multivariate_convolution
#include <iostream>
#include <vector>
#include "../../src/algorithm/fft/transform/multivariate_convolution.hpp"
int main(){
    std::ios::sync_with_stdio(false);std::cin.tie(nullptr);
    int k;std::cin>>k;std::vector<int>shape(k);std::size_t size=1;
    for(auto& n:shape){std::cin>>n;size*=n;}
    std::vector<Modint998244353>a(size),b(size);for(auto& x:a)std::cin>>x;for(auto& x:b)std::cin>>x;
    const auto c=multivariate_truncated_convolution(shape,a,b);
    for(std::size_t i=0;i<size;++i)std::cout<<c[i]<<(i+1==size?'\n':' ');
}
