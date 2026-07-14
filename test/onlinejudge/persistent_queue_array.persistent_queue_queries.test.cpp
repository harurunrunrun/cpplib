// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/persistent_queue

#include <iostream>
#include "../../src/structure/other/persistent_queue.hpp"
#include "../../src/structure/other/persistent_queue_queries.hpp"

constexpr int MAX_Q = 500000;

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int Q;
    std::cin >> Q;

    PersistentQueueQueries<
        PersistentArrayQueue<int, MAX_Q, MAX_Q>, MAX_Q
    > queue(MAX_Q);
    for(int q = 0; q < Q; q++){
        int type, t;
        std::cin >> type >> t;
        if(type == 0){
            int x;
            std::cin >> x;
            queue.push(t, x);
        }else{
            std::cout << queue.pop(t) << '\n';
        }
    }
}
