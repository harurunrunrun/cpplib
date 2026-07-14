// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/range_affine_range_sum_large_array


#include <iostream>

#include "../../src/structure/modint/modint.hpp"
#include "../../src/structure/segtree/range_affine_range_sum_large_array.hpp"


using namespace std;
using mint=Modint998244353;
int main(){
    ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int N,Q;
    cin>>N>>Q;
    RangeAffineRangeSumLargeArray<mint,1'000'000'000> queries(N);
    for(int i=0;i<Q;i++){
        int t;
        cin>>t;
        if(t==0){
            int l,r,b,c;
            cin>>l>>r>>b>>c;
            queries.apply(l,r,b,c);
        }else{
            int l,r;
            cin>>l>>r;
            cout<<queries.range_sum(l,r).val()<<"\n";
        }
    }
}
