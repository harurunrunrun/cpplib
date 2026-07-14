// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/range_affine_range_sum


#include <iostream>
#include <vector>

#include "../../src/structure/modint/modint.hpp"
#include "../../src/structure/segtree/range_affine_range_sum.hpp"


using namespace std;
using mint=Modint998244353;
int main(){
    ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int N,Q;
    cin>>N>>Q;
    vector<mint> init(N);
    for(int i=0;i<N;i++){
        cin>>init[i];
    }

    RangeAffineRangeSum<mint,500000> queries(init);
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
