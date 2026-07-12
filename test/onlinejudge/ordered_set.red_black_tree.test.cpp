// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/ordered_set

#include "../../src/structure/bbst/red_black_tree.hpp"
#include "../../src/structure/other/fastio.hpp"

int main(){
    fastio io;
    int n, q;
    io.readint(n);
    io.readint(q);
    RedBlackSet<int, 1000000> set;
    for(int i = 0; i < n; i++){
        int x;
        io.readint(x);
        set.insert(x);
    }
    while(q--){
        int type, x;
        io.readint(type);
        io.readint(x);
        if(type == 0){
            set.insert(x);
        }else if(type == 1){
            set.erase(x);
        }else if(type == 2){
            auto val = set.kth(x - 1);
            io.writeint(val ? *val : -1);
            io.write('\n');
        }else if(type == 3){
            io.writeint(set.order_of_key_upper(x));
            io.write('\n');
        }else if(type == 4){
            auto val = set.max_leq(x);
            io.writeint(val ? *val : -1);
            io.write('\n');
        }else{
            auto val = set.lower_bound(x);
            io.writeint(val ? *val : -1);
            io.write('\n');
        }
    }
}
