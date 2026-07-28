// competitive-verifier: STANDALONE

#include <iostream>
#include <string>

#include "../../src/structure/range_query/aggregation/aggregate_deque.hpp"
#include "../../src/structure/types/monoid/monoid.hpp"

constexpr std::string concatenate(std::string left, std::string right){
    return left + right;
}
constexpr std::string empty_string(){ return {}; }
constexpr Monoid<concatenate, empty_string> string_monoid{};

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int operation_count;
    if(!(std::cin >> operation_count)) return 1;
    AggregateDeque<string_monoid> deque;
    while(operation_count-- > 0){
        int type;
        std::cin >> type;
        if(type == 0 || type == 1){
            std::string value;
            std::cin >> value;
            if(type == 0) deque.push_front(value);
            else deque.push_back(value);
        }else if(type == 2){
            deque.pop_front();
        }else if(type == 3){
            deque.pop_back();
        }else if(type == 4){
            std::cout << deque.front() << '\n';
        }else if(type == 5){
            std::cout << deque.back() << '\n';
        }else{
            std::cout << (deque.empty() ? "-" : deque.fold()) << '\n';
        }
    }
}
