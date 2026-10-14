// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/dynamic_graph_vertex_add_component_sum
#include <iostream>
#include <vector>
#include "../../src/structure/graph/dynamic_connectivity/offline_dynamic_component_aggregate.hpp"
struct SumMonoid{
    using S=long long;
    constexpr S op(S a,S b)const{return a+b;}
    constexpr S e()const{return 0;}
};
int main(){
    std::ios::sync_with_stdio(false);std::cin.tie(nullptr);
    int n,q;std::cin>>n>>q;std::vector<long long> values(n);for(auto& x:values)std::cin>>x;
    std::vector<DynamicComponentQuery<SumMonoid{}>>queries(q);
    for(auto& x:queries){std::cin>>x.type>>x.first;if(x.type<=1)std::cin>>x.second;else if(x.type==2)std::cin>>x.value;}
    for(auto value:offline_dynamic_component_aggregate<SumMonoid{}>(values,queries))std::cout<<value<<'\n';
}
