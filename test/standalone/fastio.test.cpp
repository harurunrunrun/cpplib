// competitive-verifier: STANDALONE

#define PROBLEM "https://example.com/standalone/fastio"

#include "../../src/structure/other/fastio.hpp"

int main(){
    fastio io;
    int count;
    io.readint(count);
    for(int index = 0; index < count; ++index){
        int integer;
        long long long_integer;
        char word[128];
        io.readint(integer);
        io.readll(long_integer);
        io.readstr(word);
        io.write(word);
        io.write(':');
        io.writeint(integer);
        io.write(':');
        io.writell(long_integer);
        io.write('\n');
    }
}
