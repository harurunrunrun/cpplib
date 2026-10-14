// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/binomial_coefficient_prime_mod
#include <algorithm>
#include <utility>
#include <vector>
#include "../../src/algorithm/math/combinatorics/binomial_tables.hpp"
#include "../../src/structure/io/fastio.hpp"
int main(){
    fastio io;int t,m,maximum=0;io.readint(t);io.readint(m);
    std::vector<std::pair<int,int>>queries(t);
    for(auto&[n,k]:queries){io.readint(n);io.readint(k);maximum=std::max(maximum,n);}
    const math::BinomialPrimeTable table(m,maximum);
    for(auto[n,k]:queries){io.writell(table.binomial(n,k));io.write('\n');}
}
