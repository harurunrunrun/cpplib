// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/sparse_matrix_det
#include <iostream>
#include <vector>
#include "../../src/algorithm/math/linear_algebra/elimination/sparse_determinant.hpp"
#include "../../src/structure/modint/modint.hpp"
int main(){
    std::ios::sync_with_stdio(false);std::cin.tie(nullptr);
    int n,k;std::cin>>n>>k;
    std::vector<SparseMatrixEntry<Modint998244353>>entries(k);
    for(auto& e:entries)std::cin>>e.row>>e.column>>e.value;
    std::cout<<sparse_determinant(n,entries)<<'\n';
}
