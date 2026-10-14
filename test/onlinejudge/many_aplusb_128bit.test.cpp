// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/many_aplusb_128bit
#include "../../src/structure/io/fastio.hpp"
int main(){
    fastio io;
    int count; io.readint(count);
    while(count--){
        __int128 a, b; io.readi128(a); io.readi128(b);
        io.writei128(a + b); io.write('\n');
    }
}
