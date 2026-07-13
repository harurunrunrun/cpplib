// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/double_ended_priority_queue

#include <vector>

#include "../../src/structure/other/double_ended_priority_queue.hpp"
#include "../../src/structure/other/fastio.hpp"

int main(){
    fastio io;
    int n, q;
    io.readint(n);
    io.readint(q);
    std::vector<int> values(static_cast<std::size_t>(n));
    for(int& value: values) io.readint(value);
    DoubleEndedPriorityQueue<int, 1000000> queue(values);
    while(q--){
        int type;
        io.readint(type);
        if(type == 0){
            int value;
            io.readint(value);
            queue.push(value);
        }else if(type == 1){
            io.writeint(queue.pop_min());
            io.write('\n');
        }else{
            io.writeint(queue.pop_max());
            io.write('\n');
        }
    }
}
