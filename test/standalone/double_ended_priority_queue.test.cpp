// competitive-verifier: STANDALONE

#include <cassert>
#include <functional>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "../../src/structure/other/double_ended_priority_queue.hpp"

void self_test(){
    DoubleEndedPriorityQueue<int, 16> empty;
    assert(empty.empty());
    assert(empty.size() == 0);

    DoubleEndedPriorityQueue<int, 16> queue(std::vector<int>{4, 1, 4, -2, 7});
    assert(queue.size() == 5);
    assert(queue.min() == -2);
    assert(queue.max() == 7);
    queue.push(-5);
    queue.push(9);
    assert(queue.pop_min() == -5);
    assert(queue.pop_max() == 9);
    assert(queue.pop_max() == 7);
    assert(queue.pop_min() == -2);
    assert(queue.min() == 1);
    assert(queue.max() == 4);
    queue.clear();
    assert(queue.empty());

    for(int operation = 0; operation < 4; ++operation){
        bool thrown = false;
        try{
            if(operation == 0) (void)queue.min();
            if(operation == 1) (void)queue.max();
            if(operation == 2) (void)queue.pop_min();
            if(operation == 3) (void)queue.pop_max();
        }catch(const std::runtime_error&){
            thrown = true;
        }
        assert(thrown);
    }

    DoubleEndedPriorityQueue<int, 4, std::greater<int>> reversed(
        std::vector<int>{4, 1, 7, -2}, std::greater<int>{});
    assert(reversed.min() == 7);
    assert(reversed.max() == -2);
    assert(reversed.pop_min() == 7);
    assert(reversed.pop_max() == -2);

    bool thrown = false;
    try{
        DoubleEndedPriorityQueue<int, 2> oversized(
            std::vector<int>{1, 2, 3});
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);

    DoubleEndedPriorityQueue<int, 4> full;
    full.push(1);
    full.push(2);
    full.push(3);
    full.push(4);
    thrown = false;
    try{
        full.push(5);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);
    assert(full.size() == 4);
}

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int n, q;
    if(!(std::cin >> n >> q)){
        self_test();
        return 0;
    }
    std::vector<int> initial(static_cast<std::size_t>(n));
    for(int& value: initial) std::cin >> value;
    DoubleEndedPriorityQueue<int, 8192> queue(initial);
    while(q--){
        std::string operation;
        std::cin >> operation;
        if(operation == "PUSH"){
            int value;
            std::cin >> value;
            queue.push(value);
        }else if(operation == "MIN"){
            std::cout << queue.min() << '\n';
        }else if(operation == "MAX"){
            std::cout << queue.max() << '\n';
        }else if(operation == "POPMIN"){
            std::cout << queue.pop_min() << '\n';
        }else if(operation == "POPMAX"){
            std::cout << queue.pop_max() << '\n';
        }else if(operation == "CLEAR"){
            queue.clear();
        }else if(operation == "SIZE"){
            std::cout << queue.size() << '\n';
        }else if(operation == "EMPTY"){
            std::cout << queue.empty() << '\n';
        }
    }
}
