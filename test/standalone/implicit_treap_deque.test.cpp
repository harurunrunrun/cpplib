// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <stdexcept>
#include <string>

#include "../../src/structure/tree/implicit_treap_deque.hpp"
#include "../../src/structure/types/common_monoids.hpp"

void self_test(){
    ImplicitTreapDeque<AddMonoid<long long>{}, 8> deque;
    bool thrown = false;
    try{
        deque.pop_front();
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);
    thrown = false;
    try{
        deque.pop_back();
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);

    deque.push_front(2);
    deque.push_back(3);
    deque.push_front(1);
    assert(deque.size() == 3);
    assert(deque.get(0) == 1);
    assert(deque.get(2) == 3);
    deque.pop_front();
    deque.pop_back();
    assert(deque.get(0) == 2);
}

int main(){
    int query_count;
    if(!(std::cin >> query_count)){
        self_test();
        return 0;
    }
    ImplicitTreapDeque<AddMonoid<long long>{}, 4096> deque;
    while(query_count--){
        std::string type;
        std::cin >> type;
        if(type == "PF"){
            long long value;
            std::cin >> value;
            deque.push_front(value);
        }else if(type == "PB"){
            long long value;
            std::cin >> value;
            deque.push_back(value);
        }else if(type == "OF"){
            deque.pop_front();
        }else if(type == "OB"){
            deque.pop_back();
        }else if(type == "G"){
            int index;
            std::cin >> index;
            std::cout << deque.get(index) << '\n';
        }else{
            std::cout << deque.size() << '\n';
        }
    }
}
