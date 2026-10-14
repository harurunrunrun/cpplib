// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/matrix_det_arbitrary_mod
#include <iostream>
#include <vector>
#include "../../src/algorithm/math/linear_algebra/elimination/determinant_mod_composite.hpp"
int main(){
    std::ios::sync_with_stdio(false);std::cin.tie(nullptr);
    int n;long long modulus;std::cin>>n>>modulus;
    std::vector<std::vector<long long>> matrix(n,std::vector<long long>(n));
    for(auto& row:matrix)for(auto& value:row)std::cin>>value;
    std::cout<<determinant_mod_composite(std::move(matrix),modulus)<<'\n';
}
