// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/range_set_range_composite


#include <iostream>
#include <vector>

#include "../../src/structure/modint/modint.hpp"
#include "../../src/structure/segtree/range_set_range_composite.hpp"


using namespace std;
using mint=Modint998244353;
int main(){
    ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int N,Q;
    cin>>N>>Q;
    vector<MonoidAffine<mint>> init(N);
    for(int i=0;i<N;i++){
        int a,b;
        cin>>a>>b;
        init[i]={a,b};
    }

    RangeSetRangeComposite<mint,500000> queries(init);
    for(int i=0;i<Q;i++){
        int t;
        cin>>t;
        if(t==0){
            int l,r,b,c;
            cin>>l>>r>>b>>c;
            queries.assign(l,r,b,c);
        }else{
            int l,r,x;
            cin>>l>>r>>x;
            cout<<queries.evaluate(l,r,mint{x}).val()<<"\n";
        }
    }
}
