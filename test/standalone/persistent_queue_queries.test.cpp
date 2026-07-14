// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <limits>
#include <stdexcept>

#include "../../src/structure/other/persistent_queue.hpp"
#include "../../src/structure/other/persistent_queue_queries.hpp"
#include "../../src/structure/types/common_monoids.hpp"
using HeapBackedQueueQueries = PersistentQueueQueries<
    PersistentArrayQueue<int, 512, 512>,
    512
>;
static_assert(sizeof(HeapBackedQueueQueries) <= 4 * sizeof(void*));


template<class Queue>
void test_backend(){
    PersistentQueueQueries<Queue, 8> queue(8);
    assert(queue.processed_queries() == 0);

    queue.push(-1, 10);
    queue.push(0, 20);
    assert(queue.pop(1) == 10);
    queue.push(0, 30);
    assert(queue.pop(3) == 10);
    assert(queue.pop(2) == 20);
    assert(queue.processed_queries() == 6);

    const int processed = queue.processed_queries();
    bool thrown = false;
    try{
        queue.push(processed, 1);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);
    assert(queue.processed_queries() == processed);
}

void self_test(){
    test_backend<PersistentArrayQueue<int, 8, 8>>();
    test_backend<PersistentSegtreeQueue<MaxMonoid<int>{}, 8, 8>>();

    PersistentQueueQueries<
        PersistentArrayQueue<int, 2, 2>, 2
    > empty_queue(2);
    bool thrown = false;
    try{
        (void)empty_queue.pop(-1);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);
    assert(empty_queue.processed_queries() == 0);

    empty_queue.push(-1, std::numeric_limits<int>::min());
    assert(empty_queue.pop(0) == std::numeric_limits<int>::min());
    assert(empty_queue.processed_queries() == 2);
}

int main(){
    self_test();

    int query_count;
    if(!(std::cin >> query_count)) return 0;
    PersistentQueueQueries<
        PersistentArrayQueue<int, 256, 512>, 512
    > queue(256);
    for(int query = 0; query < query_count; ++query){
        int type, base_query;
        std::cin >> type >> base_query;
        if(type == 0){
            int value;
            std::cin >> value;
            queue.push(base_query, value);
        }else{
            std::cout << queue.pop(base_query) << '\n';
        }
    }
}
