// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/range_set_range_composite


#include <iostream>
#include <utility>

#include "../src/structure/modint/modint.hpp"
#include "../src/structure/segtree/lazysegtree.hpp"


using namespace std;
using mint=Modint998244353;


struct S {
    mint a = 1, b = 0; // F(x)=a*x+b
    int len = 0;
};

struct T {
    mint a = 1, b = 0; // f(x)=a*x+b
    bool has = false;  // false なら何もしない
};

// g(f(x))
pair<mint, mint> compose_affine(pair<mint, mint> g, pair<mint, mint> f) {
    return {
        g.first * f.first,
        g.first * f.second + g.second
    };
}

// f を n 回合成した関数
pair<mint, mint> affine_pow(mint a, mint b, int n) {
    pair<mint, mint> res = {1, 0};
    pair<mint, mint> base = {a, b};

    while (n > 0) {
        if (n & 1) {
            res = compose_affine(base, res);
        }
        base = compose_affine(base, base);
        n >>= 1;
    }

    return res;
}

// 左区間 l, 右区間 r を結合する
// 全体は r(l(x))
S op(S l, S r) {
    return {
        r.a * l.a,
        r.a * l.b + r.b,
        l.len + r.len
    };
}

S e() {
    return {1, 0, 0};
}

// 区間内の関数をすべて f(x)=a*x+b に代入する
S mapping(T f, S x) {
    if (!f.has) return x;

    auto [a, b] = affine_pow(f.a, f.b, x.len);
    return {a, b, x.len};
}

// 遅延代入の合成
// 後から来た代入が前の代入を上書きする
T composition(T f, T g) {
    if (!f.has) return g;
    return f;
}

T id() {
    return {1, 0, false};
}

constexpr Monoid_Act<op, e, mapping, composition, id> monoid;

int main(){
    ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int N,Q;
    cin>>N>>Q;
    vector<S> init(N);
    for(int i=0;i<N;i++){
        int a,b;
        cin>>a>>b;
        init[i]=S{a,b,1};
    }

    LazySegtree<monoid,500000> seg(init);
    for(int i=0;i<Q;i++){
        int t;
        cin>>t;
        if(t==0){
            int l,r,b,c;
            cin>>l>>r>>b>>c;
            seg.apply(l,r,T{b,c,true});
        }else{
            int l,r,x;
            cin>>l>>r>>x;
            S res=seg.prod(l,r);
            cout<<(res.a*x+res.b).val()<<"\n";
        }
    }
}