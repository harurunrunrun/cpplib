// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/binomial_coefficient
#include "../../src/algorithm/math/combinatorics/binomial_tables.hpp"
#include "../../src/structure/io/fastio.hpp"
int main(){
    fastio io;int t,m;io.readint(t);io.readint(m);
    const math::BinomialCompositeTable table(m);
    while(t--){long long n,k;io.readll(n);io.readll(k);io.writell(table.binomial(n,k));io.write('\n');}
}
