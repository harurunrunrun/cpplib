// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/find_linear_recurrence
#include <iostream>
#include <vector>
#include "../../src/algorithm/math/sequence/linear_recurrence.hpp"
int main(){
    std::ios::sync_with_stdio(false); std::cin.tie(nullptr);
    int n; std::cin >> n;
    std::vector<Modint998244353> a(n);
    for(auto& x:a) std::cin >> x;
    const auto recurrence=berlekamp_massey<Modint998244353,10000>(a);
    std::cout << recurrence.degree() << '\n';
    for(int i=0;i<recurrence.degree();++i)
        std::cout << recurrence.coefficient(i) << (i+1==recurrence.degree()?'\n':' ');
    if(recurrence.degree()==0) std::cout << '\n';
}
