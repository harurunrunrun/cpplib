// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <string>
#include <vector>

#include "../../src/structure/other/double_ended_priority_queue.hpp"

void self_test(){
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
