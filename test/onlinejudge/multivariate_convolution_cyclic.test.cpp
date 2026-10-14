// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/multivariate_convolution_cyclic
#include <iostream>
#include <vector>
#include "../../src/algorithm/fft/transform/multivariate_convolution.hpp"
int main(){
    std::ios::sync_with_stdio(false);std::cin.tie(nullptr);
    unsigned int p;int k;std::cin>>p>>k;std::vector<int>shape(k);std::size_t size=1;
    for(auto& n:shape){std::cin>>n;size*=n;}
    std::vector<std::uint32_t>a(size),b(size);for(auto& x:a)std::cin>>x;for(auto& x:b)std::cin>>x;
    const auto c=multivariate_cyclic_convolution(shape,a,b,p);
    for(std::size_t i=0;i<size;++i)std::cout<<c[i]<<(i+1==size?'\n':' ');
}
