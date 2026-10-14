// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/min_cost_b_flow
#include <iostream>
#include "../../src/algorithm/matching/flow/minimum_cost_b_flow.hpp"
#include "../../src/structure/io/fastio.hpp"
int main(){
    fastio io;int n,m;io.readint(n);io.readint(m);
    MinimumCostBFlow graph(n);
    for(int v=0;v<n;++v){long long b;io.readll(b);graph.set_balance(v,-b);}
    for(int i=0;i<m;++i){
        int u,v;long long lower,upper,cost;
        io.readint(u);io.readint(v);io.readll(lower);io.readll(upper);io.readll(cost);
        graph.add_edge(u,v,lower,upper,cost);
    }
    const auto result=graph.solve();
    if(!result){io.write("infeasible\n");return 0;}
    io.writei128(result->cost);io.write('\n');
    for(auto p:graph.dual_potentials(result->flow)){io.writei128(p);io.write('\n');}
    for(auto f:result->flow){io.writell(f);io.write('\n');}
}
