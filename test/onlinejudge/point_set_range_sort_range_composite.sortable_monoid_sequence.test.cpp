// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/point_set_range_sort_range_composite

#include <iostream>
#include <vector>
#include "../../src/structure/bbst/sequence/sortable_monoid_sequence.hpp"
#include "../../src/structure/modint/modint.hpp"
#include "../../src/structure/types/monoid/common_monoids.hpp"
using mint = Modint998244353;
constexpr AffineCompositionMonoid<mint> composition;
int main(){
    std::ios::sync_with_stdio(false);std::cin.tie(nullptr);
    int n,q;std::cin>>n>>q;std::vector<int> keys(n);std::vector<MonoidAffine<mint>> a(n);
    for(int i=0;i<n;++i)std::cin>>keys[i]>>a[i].multiplier>>a[i].addend;
    SortableMonoidSequence<composition> sequence(1<<30,keys,a);
    while(q--){
        int t,l,r;std::cin>>t>>l>>r;
        if(t==0){mint x,y;std::cin>>x>>y;sequence.set(l,r,{x,y});}
        else if(t==1){mint x;std::cin>>x;auto f=sequence.prod(l,r);std::cout<<f.multiplier*x+f.addend<<'\n';}
        else sequence.sort(l,r,t==3);
    }
}
