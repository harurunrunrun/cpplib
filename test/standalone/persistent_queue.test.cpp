// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

#include "../../src/structure/other/persistent_queue.hpp"
#include "../../src/structure/types/common_monoids.hpp"

template<class Function>
void assert_runtime_error(Function function){
    bool thrown = false;
    try{
        function();
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);
}

void self_test(){
    using ArrayQueue = PersistentArrayQueue<int, 8, 32>;
    using SegtreeQueue = PersistentSegtreeQueue<MaxMonoid<int>{}, 8, 32>;
    ArrayQueue array_queue(3);
    SegtreeQueue segtree_queue(3);

    assert(array_queue.capacity() == 3);
    assert(array_queue.versions() == 1);
    assert(array_queue.latest_version() == 0);
    assert(array_queue.empty(0));
    assert(segtree_queue.empty(0));

    const int v1 = array_queue.push(0, 4);
    assert(segtree_queue.push(0, 4) == v1);
    const int v2 = array_queue.push(v1, -5);
    assert(segtree_queue.push(v1, -5) == v2);
    const int v3 = array_queue.pop(v2);
    assert(segtree_queue.pop(v2) == v3);
    const int v4 = array_queue.push(v3, 9);
    assert(segtree_queue.push(v3, 9) == v4);
    const int v5 = array_queue.pop(v4);
    assert(segtree_queue.pop(v4) == v5);
    const int v6 = array_queue.push(v5, 10);
    assert(segtree_queue.push(v5, 10) == v6);
    const int branch = array_queue.push(v1, 7);
    assert(segtree_queue.push(v1, 7) == branch);

    assert(array_queue.front(v1) == 4);
    assert(array_queue.front(v2) == 4);
    assert(array_queue.front(v3) == -5);
    assert(array_queue.front(v4) == -5);
    assert(array_queue.front(v5) == 9);
    assert(array_queue.front(v6) == 9);
    assert(array_queue.front(branch) == 4);
    assert(segtree_queue.front(v6) == 9);
    assert(segtree_queue.front(branch) == 4);
    assert(array_queue.size(v6) == 2);
    assert(array_queue.size(branch) == 2);

    const int full = array_queue.push(v2, 11);
    assert(segtree_queue.push(v2, 11) == full);
    assert_runtime_error([&]{ (void)array_queue.push(full, 12); });
    assert_runtime_error([&]{ (void)segtree_queue.push(full, 12); });
    assert_runtime_error([&]{ (void)array_queue.front(0); });
    assert_runtime_error([&]{ (void)array_queue.pop(0); });
    assert_runtime_error([&]{ (void)array_queue.size(100); });

    PersistentArrayQueue<int, 2, 2> limited(2);
    const int limited_v1 = limited.push(0, 1);
    const int limited_v2 = limited.pop(limited_v1);
    assert_runtime_error([&]{ (void)limited.push(limited_v2, 2); });

    PersistentArrayQueue<int, 2, 4> zero_capacity(0);
    assert(zero_capacity.empty(0));
    assert_runtime_error([&]{ (void)zero_capacity.push(0, 1); });
}

int main(){
    int capacity, query_count;
    if(!(std::cin >> capacity >> query_count)){
        self_test();
        return 0;
    }

    using ArrayQueue = PersistentArrayQueue<long long, 256, 2048>;
    using SegtreeQueue = PersistentSegtreeQueue<MaxMonoid<long long>{}, 256, 2048>;
    auto array_queue = std::make_unique<ArrayQueue>(capacity);
    auto segtree_queue = std::make_unique<SegtreeQueue>(capacity);

    while(query_count-- > 0){
        std::string operation;
        std::cin >> operation;
        if(operation == "PUSH"){
            int version;
            long long value;
            std::cin >> version >> value;
            const int first = array_queue->push(version, value);
            const int second = segtree_queue->push(version, value);
            if(first != second) throw std::runtime_error("backend version mismatch");
            std::cout << first << '\n';
        }else if(operation == "POP"){
            int version;
            std::cin >> version;
            const int first = array_queue->pop(version);
            const int second = segtree_queue->pop(version);
            if(first != second) throw std::runtime_error("backend version mismatch");
            std::cout << first << '\n';
        }else if(operation == "FRONT"){
            int version;
            std::cin >> version;
            const long long first = array_queue->front(version);
            const long long second = segtree_queue->front(version);
            if(first != second) throw std::runtime_error("backend value mismatch");
            std::cout << first << '\n';
        }else if(operation == "SIZE"){
            int version;
            std::cin >> version;
            const int first = array_queue->size(version);
            const int second = segtree_queue->size(version);
            if(first != second) throw std::runtime_error("backend size mismatch");
            std::cout << first << '\n';
        }else if(operation == "EMPTY"){
            int version;
            std::cin >> version;
            const bool first = array_queue->empty(version);
            const bool second = segtree_queue->empty(version);
            if(first != second) throw std::runtime_error("backend state mismatch");
            std::cout << static_cast<int>(first) << '\n';
        }else if(operation == "VERSIONS"){
            std::cout << array_queue->versions() << '\n';
        }else if(operation == "LATEST"){
            std::cout << array_queue->latest_version() << '\n';
        }else if(operation == "CAPACITY"){
            std::cout << array_queue->capacity() << '\n';
        }else{
            throw std::runtime_error("unknown operation");
        }
    }
}
