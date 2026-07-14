// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <functional>
#include <iostream>
#include <random>
#include <set>
#include <stdexcept>
#include <vector>

#include "../../src/structure/heap/persistent_leftist_heap.hpp"

void self_test(){
    {
        PersistentLeftistHeap<int, 32, 128> heap;
        const std::size_t one = heap.push(0, 5);
        const std::size_t two = heap.push(one, 2);
        const std::size_t three = heap.push(one, 7);
        assert(heap.nodes_used() > 0);
        assert(heap.versions() == 4);
        assert(heap.top(one) == 5);
        assert(heap.top(two) == 2);
        assert(heap.top(three) == 5);

        const std::size_t joined = heap.meld(two, three);
        assert(heap.size(joined) == 4);
        assert(heap.top(joined) == 2);
        const std::size_t popped = heap.pop(joined);
        assert(heap.top(popped) == 5);
        assert(heap.size(popped) == 3);

        const std::size_t doubled = heap.meld(two, two);
        std::vector<int> values;
        for(std::size_t version = doubled; !heap.empty(version);){
            values.push_back(heap.top(version));
            version = heap.pop(version);
        }
        assert((values == std::vector<int>{2, 2, 5, 5}));
        assert(heap.top(two) == 2);
    }
    {
        PersistentLeftistHeap<int, 16, 64, std::greater<int>> heap;
        std::size_t version = heap.push(0, 1);
        version = heap.push(version, 8);
        version = heap.push(version, 3);
        assert(heap.top(version) == 8);
    }
    {
        struct Compare{
            bool maximum;
            bool operator()(int left, int right) const{
                return maximum ? left > right : left < right;
            }
        };
        PersistentLeftistHeap<int, 8, 32, Compare> heap(Compare{true});
        std::size_t version = heap.push(0, 1);
        version = heap.push(version, 9);
        assert(heap.top(version) == 9);
    }
    {
        PersistentLeftistHeap<int, 4, 2> heap;
        const std::size_t version = heap.push(0, 1);
        bool node_capacity = false;
        try{
            (void)heap.push(version, 2);
        }catch(const std::runtime_error&){
            node_capacity = true;
        }
        assert(node_capacity);
        assert(heap.versions() == 2);
        assert(heap.top(version) == 1);
    }
    {
        PersistentLeftistHeap<int, 2, 8> heap;
        const std::size_t version = heap.fork(0);
        bool version_capacity = false;
        try{
            (void)heap.fork(version);
        }catch(const std::runtime_error&){
            version_capacity = true;
        }
        assert(version_capacity);
    }
    {
        PersistentLeftistHeap<int, 8, 8> heap;
        bool empty_top = false;
        bool empty_pop = false;
        bool bad_version = false;
        try{
            (void)heap.top(0);
        }catch(const std::runtime_error&){
            empty_top = true;
        }
        try{
            (void)heap.pop(0);
        }catch(const std::runtime_error&){
            empty_pop = true;
        }
        try{
            (void)heap.size(1);
        }catch(const std::runtime_error&){
            bad_version = true;
        }
        assert(empty_top && empty_pop && bad_version);
    }

    static PersistentLeftistHeap<long long, 12000, 400000> heap;
    std::vector<std::multiset<long long>> reference(1);
    std::mt19937_64 random(2026071314);
    for(int iteration = 0; iteration < 10000; ++iteration){
        const std::size_t first = static_cast<std::size_t>(
            random() % reference.size()
        );
        const int operation = static_cast<int>(random() % 4);
        std::size_t version;
        std::multiset<long long> next;
        if(operation == 0){
            const long long value = static_cast<long long>(random() % 2001) - 1000;
            version = heap.push(first, value);
            next = reference[first];
            next.insert(value);
        }else if(operation == 1 && !reference[first].empty()){
            version = heap.pop(first);
            next = reference[first];
            next.erase(next.begin());
        }else if(operation == 2){
            const std::size_t second = static_cast<std::size_t>(
                random() % reference.size()
            );
            if(reference[first].size() + reference[second].size() <= 1200){
                version = heap.meld(first, second);
                next = reference[first];
                next.insert(reference[second].begin(), reference[second].end());
            }else{
                version = heap.fork(first);
                next = reference[first];
            }
        }else{
            version = heap.fork(first);
            next = reference[first];
        }
        assert(version == reference.size());
        reference.push_back(std::move(next));
        assert(heap.size(version) == reference.back().size());
        assert(heap.empty(version) == reference.back().empty());
        if(!reference.back().empty()){
            assert(heap.top(version) == *reference.back().begin());
        }

        const std::size_t old = static_cast<std::size_t>(random() % reference.size());
        assert(heap.size(old) == reference[old].size());
        if(!reference[old].empty()) assert(heap.top(old) == *reference[old].begin());
    }

    constexpr int large_size = 30000;
    static PersistentLeftistHeap<int, 60002, 1000000> large;
    std::vector<int> permutation(static_cast<std::size_t>(large_size));
    for(int index = 0; index < large_size; ++index){
        permutation[static_cast<std::size_t>(index)] = index;
    }
    std::shuffle(permutation.begin(), permutation.end(), random);
    std::size_t version = 0;
    for(int value: permutation) version = large.push(version, value);
    for(int expected = 0; expected < large_size; ++expected){
        assert(large.top(version) == expected);
        version = large.pop(version);
    }
    assert(large.empty(version));
}

int main(){
    int query_count;
    if(!(std::cin >> query_count)){
        self_test();
        return 0;
    }

    static PersistentLeftistHeap<long long, 200005, 2000000> heap;
    for(int query = 0; query < query_count; ++query){
        int type;
        std::cin >> type;
        if(type == 0){
            std::size_t version;
            long long value;
            std::cin >> version >> value;
            std::cout << heap.push(version, value) << '\n';
        }else if(type == 1){
            std::size_t left;
            std::size_t right;
            std::cin >> left >> right;
            std::cout << heap.meld(left, right) << '\n';
        }else if(type == 2){
            std::size_t version;
            std::cin >> version;
            std::cout << heap.top(version) << ' ' << heap.pop(version) << '\n';
        }else{
            std::size_t version;
            std::cin >> version;
            std::cout << heap.fork(version) << '\n';
        }
    }
}
