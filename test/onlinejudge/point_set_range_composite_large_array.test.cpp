// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/point_set_range_composite_large_array


#include <iostream>

#include "../../src/structure/modint/modint.hpp"
#include "../../src/structure/segtree/point_set_range_composite_large_array.hpp"


using namespace std;
using mint=Modint998244353;

int main(){
    ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int N,Q;
    cin>>N>>Q;

    PointSetRangeCompositeLargeArray<mint,1000000000> queries(N);
    for(int i=0;i<Q;i++){
        int t;
        cin>>t;
        if(t==0){
            int p,c,d;
            cin>>p>>c>>d;
            queries.set(p,c,d);
        }else{
            int l,r,x;
            cin>>l>>r>>x;
            cout<<queries.evaluate(l,r,mint{x}).val()<<"\n";
        }
    }
}
