// competitive-verifier: STANDALONE

#include <iostream>
#include <string>

#include "../../src/structure/range_query/aggregation/aggregate_queue.hpp"
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
    AggregateQueue<string_monoid> queue;
    while(operation_count-- > 0){
        int type;
        std::cin >> type;
        if(type == 0){
            std::string value;
            std::cin >> value;
            queue.push(std::move(value));
        }else if(type == 1){
            queue.pop();
        }else if(type == 2){
            std::cout << queue.front() << '\n';
        }else{
            std::cout << (queue.empty() ? "-" : queue.fold()) << '\n';
        }
    }
}
