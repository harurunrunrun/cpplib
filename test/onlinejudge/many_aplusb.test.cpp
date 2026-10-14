// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/many_aplusb
#include "../../src/structure/io/fastio.hpp"
int main(){
    fastio io;
    int count; io.readint(count);
    while(count--){
        long long a, b; io.readll(a); io.readll(b);
        io.writell(a + b); io.write('\n');
    }
}
